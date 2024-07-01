#include "Search.h"

std::vector<std::pair<std::string,int>> Search::getPostingListForSingleTerm(std::string word)
{
    std::vector<std::pair<std::string,int>> postingList;
    if(invertedIndex.find(word) == invertedIndex.end()) return postingList;
    int offset = invertedIndex[word];
    int startChar = word[0] - 'a';
    if(startChar < 0 || startChar >= 26) throw std::out_of_range("Invalid start character");

    // Get the posting line in format ID-freq;Id-freq;Id-freq;
    postingListStreams[startChar]->seekg(offset);
    std::string posting;
    std::getline(*postingListStreams[startChar], posting);

    // Convert the posting line into vector of pairs, where each pair contains a document ID and it's frequency
    try
    {
        std::pair<std::string,int> docId_freq;
        std::string freq;
        std::string docId;
        bool startFreq = false;
        for(char c : posting)
        {
            if(c == ';')
            {
                docId_freq.first = docId;
                docId_freq.second = std::stoi(freq);
                postingList.emplace_back(docId_freq);
                docId.clear();
                freq.clear();
                startFreq = false;
            }
            else if(c == '-')
            {
                startFreq = true;
            }
            else if(startFreq)
            {
                freq.push_back(c);
            }
            else
            {
                docId.push_back(c);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception occurred in converting file content to posting list: " << e.what() << std::endl;
    }

    return postingList;
}

void Search::getPostingList(std::unordered_map<std::string, int> &searchTerms, std::unordered_map<std::string, double> &docTfidfMap)
{
    for(std::pair<const std::string, int> &currentTerm : searchTerms)
    {
        std::vector<std::pair<std::string,int>> currentPosting = getPostingListForSingleTerm(currentTerm.first);
        
        // For all ID-freq pairs, calculate current tf-idf and add to previous tf-idf of that word
        for(std::pair<std::string,int> docId_freq : currentPosting)
        {
            int termFrequency = docId_freq.second;
            double prevTfidf = (docTfidfMap.find(docId_freq.first) == docTfidfMap.end()) ? 0 : docTfidfMap[docId_freq.first];
            double currTfidf = tfidf(termFrequency, currentPosting.size());
            docTfidfMap[docId_freq.first] = (prevTfidf + currTfidf);
        }
    }
}

void Search::calculateTopKDocs(std::unordered_map<std::string, double> &docTfidfMap, std::vector<std::pair<std::string, std::string>> &topKDocIds)
{

    std::priority_queue<std::pair<double, std::string>> tempQueue;
    std::string docId;
    std::string title;

    // Add all tfidf-DocID pairs to priority queue(Max-Heap), and get top k ID-Title based on Tf-Idf decreasing order
    for(std::pair<const std::string, double> currDocTfidf : docTfidfMap)
    {
        tempQueue.push({currDocTfidf.second, currDocTfidf.first});
    }
    
    for(int i = 0; (!tempQueue.empty() && i < MAX_DOCS_LIMIT); i++)
    {
        docId = tempQueue.top().second;
        title = getTitleFromDocId(docId);
        topKDocIds.push_back({docId, title});
        tempQueue.pop();
    }
}

void Search::processSearchQuery(std::string query, std::unordered_map<std::string, int> &searchTerms)
{
    char word[1000] = "";

    for(char c : query)
    {
        if (isalpha(c))
        {
            c = tolower(c);
            strncat(word, &c, 1);
        }
        else
        {
            // After tokenisation, stem and remove stop words.
            this->stemWord(word);
            std::string data(word);
            if(data.size() > 2 && !this->classifiers.isStopWord(data))
            {
                searchTerms[data]++;
            }
            word[0] = '\0';
        }
    }

    // If the word is not traversed
    if(word[0] != '\0')
    {
        this->stemWord(word);
        std::string data(word);
        if(data.size() > 2 && !this->classifiers.isStopWord(data))
        {
            searchTerms[data]++;
        }
        word[0] = '\0';
    }
}

void Search::search()
{
    std::string searchQuery; // Input query by user
    std::unordered_map<std::string, int> searchTerms; // Tokenised Word-Freq pairs
    std::unordered_map<std::string, double> docTfidfMap; // docID-tfIdf pairs
    std::vector<std::pair<std::string, std::string>> topKDocs; // Top k ID-Title pairs based on Tf-Idf

    // Prompts user to enter word or "q"/"exit" to terminate
    while(searchQuery != "q" || searchQuery != "exit")
    {
        std::cout << "\n\nEnter the query to search: ";
        std::getline(std::cin, searchQuery);
        if (searchQuery == "q" || searchQuery == "exit")
            break;
        
        std::cout << "Search results for query: " << searchQuery << "\n";

        // Tokenise -> Convert to DocID-tfidf -> Calculate Top K results
        processSearchQuery(searchQuery, searchTerms);
        getPostingList(searchTerms, docTfidfMap);
        if(docTfidfMap.empty())
        {
            std::cout << "\nNo results found\n";
            continue;
        }
        calculateTopKDocs(docTfidfMap, topKDocs);
        for(auto docId : topKDocs)
        {
            std::cout << "id: "<<  docId.first << " Title: " << docId.second << "\n" ;
        }

        searchTerms.clear();
        docTfidfMap.clear();
        topKDocs.clear();
        
    }

    this->freeStemmer();
}

std::vector<std::pair<std::string, std::string>> Search::search(std::string searchQuery)
{
    std::unordered_map<std::string, int> searchTerms;
    std::unordered_map<std::string, double> docTfidfMap;
    std::vector<std::pair<std::string, std::string>> topKDocs;

    processSearchQuery(searchQuery, searchTerms);
    getPostingList(searchTerms, docTfidfMap);

    if(!docTfidfMap.empty())
    {
       calculateTopKDocs(docTfidfMap, topKDocs);
    }

    return topKDocs;
}

std::string Search::getTitleFromDocId(std::string docId)
{
    try
    {
        int id = std::stoi(docId);
        int prevOffset = 0;

        // Fetch the previous smaller DocID and offset
        for(auto it = l1MetadataMap.begin(); it != l1MetadataMap.end(); ++it)
        {
            if(it->first > id)
            {
                break;
            }
            prevOffset = it->second;
        }

        return searchL2Metadata(id, prevOffset);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error searching L1 Metadata" << e.what() << '\n';
    }

    return "Error Finding Title for the DocID\n";
}

std::string Search::searchL2Metadata(int docId, int offset)
{
    try{
        std::filebuf *l2MetadataBuffer = metadataFileBuffers[2];
        l2MetadataBuffer->pubseekpos(offset);
        int i = 0;
        std::string prevOffset = "0";
        std::string line, id, off;

        // Search from given offset upto L2_METADATA_LIMIT, and fetch previous smaller ID and offset
        while(l2MetadataBuffer->sgetc() != EOF && i < L2_METADATA_LIMIT)
        {
            char c;
            while((c = l2MetadataBuffer->sbumpc()) != '\n')
            {
                line.push_back(c);
            }
            id = line.substr(0, line.find(":"));
            off = line.substr(line.find(":") + 1);

            if(std::stoi(id) > docId)
            {
                break;
            }
            prevOffset = off;
            i++;
            line.clear();
        }

        return searchL3Metadata(docId, std::stoi(prevOffset));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error searching L2 Metadata" << e.what() << '\n';
    }
    
    return "Error Finding Title for the DocID\n";
}

std::string Search::searchL3Metadata(int docId, int offset)
{
    try{
        std::filebuf *l3MetadataBuffer = metadataFileBuffers[3];
        l3MetadataBuffer->pubseekpos(offset);
        int i = 0;
        std::string prevOffset = "0";
        std::string line, id, off;

        // Search from given offset upto L4METADATA_LIMIT, and fetch previous smaller ID and offset
        while(l3MetadataBuffer->sgetc() != EOF && i < L3_METADATA_LIMIT)
        {
            char c;
            while((c = l3MetadataBuffer->sbumpc()) != '\n')
            {
                line.push_back(c);
            }
            id = line.substr(0, line.find(":"));
            off = line.substr(line.find(":") + 1);

            if(std::stoi(id) > docId)
            {
                break;
            }
            prevOffset = off;
            i++;
            line.clear();
        }

        return searchMetadata(docId, std::stoi(prevOffset));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error searching L3 Metadata" << e.what() << '\n';
    }
    
    return "Error Finding Title for the DocID\n";
}

std::string Search::searchMetadata(int docId, int offset)
{
    try{
        std::filebuf *metadataBuffer = metadataFileBuffers[0];
        metadataBuffer->pubseekpos(offset);
        int i = 0;
        std::string line, id, title;

        // Search from given offset upto METADATA_LIMIT, and finds the given ID in the Metadata file.
        while(metadataBuffer->sgetc() != EOF && i < METADATA_LIMIT)
        {
            char c;
            while((c = metadataBuffer->sbumpc()) != '\n')
            {
                line.push_back(c);
            }
            id = line.substr(0, line.find(":"));
            title = line.substr(line.find(":") + 1);

            if(std::stoi(id) == docId)
            {
                return title;
            }
            i++;
            line.clear();
        }

        return "Not Found";
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error searching ID Title Map" << e.what() << '\n';
    }
    
    return "Error Finding Title for the DocID\n";
}

void Search::loadInvertedIndex(FileIO &file)
{
    std::cout << "Loading Search Engine...\n";
    std::cout << "Enter q or exit to quit search\n\n";

    file.initialiseDictFiles(std::ios::in);
    file.initialisePostingLists(std::ios::in);
    dictStreams = file.getDictStreams();
    postingListStreams = file.getPostingListStreams();

    this->initializeStemmer();

    // Search from A-Z Dict files and add ID-offset pairs to Inverted Index map.
    for(int i = 0; i < 26; i++)
    {
        std::string line; 
        while(std::getline(*dictStreams[i], line))
        {
            std::string word = line.substr(0, line.find(":"));
            if(word != "")
            {
                int offset = std::stoi(line.substr(line.find(":") + 1));
                invertedIndex[word] = offset;
            }
            line.clear();
        }
    }
}

void Search::loadL1Metadata(FileIO &file)
{
    std::pair<std::pair<std::filebuf *, std::vector<std::filebuf *>>, std::string> initialised;
    try {
        initialised = file.initialise();
    } catch (const std::exception& e) {
        std::cerr << "Error: Unable to initialise FileIO" << e.what() << std::endl;
    }
    std::string resourcesFolderPath = initialised.second;
    metadataFileBuffers = initialised.first.second;

    // Open all Metadata buffers
    std::string l1MetadataPath = resourcesFolderPath + "/meta/l1metadata.txt";
    std::string l2MetadataPath = resourcesFolderPath + "/meta/l2metadata.txt";
    std::string l3MetadataPath = resourcesFolderPath + "/meta/l3metadata.txt";
    std::string MetadataPath = resourcesFolderPath + "/meta/id_title_map.txt";

    std::filebuf *l1MetadataBuffer = metadataFileBuffers[1];
    l1MetadataBuffer->open(l1MetadataPath, std::ios::in);
    if(!l1MetadataBuffer->is_open()) throw std::runtime_error("Unable to open L1 Metadata file");
    metadataFileBuffers[2]->open(l2MetadataPath, std::ios::in);
    if(!metadataFileBuffers[2]->is_open()) throw std::runtime_error("Unable to open L2 Metadata file");
    metadataFileBuffers[3]->open(l3MetadataPath, std::ios::in);
    if(!metadataFileBuffers[3]->is_open()) throw std::runtime_error("Unable to open L3 Metadata file");
    metadataFileBuffers[0]->open(MetadataPath, std::ios::in);
    if(!metadataFileBuffers[0]->is_open()) throw std::runtime_error("Unable to open Metadata file");

    std::string docId, offset, line;

    // Insert ID-offset from L1 metadata into L1 Metadata Map
    while(l1MetadataBuffer->sgetc() != EOF)
    {
        char c;
        while((c = l1MetadataBuffer->sbumpc()) != '\n')
        {
            line.push_back(c);
        }
        docId = line.substr(0, line.find(":"));
        offset = line.substr(line.find(":") + 1);
        l1MetadataMap[std::stoi(docId)] = std::stoi(offset);
        line.clear();
    }
}

double Search::tfidf(int tf, int df)
{
    return (1+std::log10(tf)) * std::log10(14128976/(float)df);
}

void Search::initializeStemmer()
{
    this->stemmer = create_stemmer();
}

void Search::stemWord(char word[])
{
    // Stems the word and replace given word with stemmed word.
    word[stem(this->stemmer, word, strlen(word) - 1) + 1] = '\0';
}

void Search::freeStemmer()
{
    free_stemmer(this->stemmer);
}
