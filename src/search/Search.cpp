#include "Search.h"

std::vector<std::pair<std::string,int>> Search::getPostingListForSingleTerm(std::string word)
{
    std::vector<std::pair<std::string,int>> postingList;
    if(invertedIndex.find(word) == invertedIndex.end()) return postingList;
    int offset = invertedIndex[word];
    int startChar = word[0] - 'a';
    if(startChar < 0 || startChar >= 26) throw std::out_of_range("Invalid start character");
    if(postingListsBuffer[startChar] == nullptr) throw std::runtime_error("Posting list buffer is null");
    postingListsBuffer[startChar]->pubseekpos(offset);
    std::string posting;
    char c;
    while((c = postingListsBuffer[startChar]->sbumpc()) != '\n')
    {
        posting.push_back(c);
    }

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
    for(auto currentTerm : searchTerms)
    {
        std::vector<std::pair<std::string,int>> currentPosting = getPostingListForSingleTerm(currentTerm.first);
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

    for(auto currDocTfidf : docTfidfMap)
    {
        tempQueue.push({currDocTfidf.second, currDocTfidf.first});
    }

    std::string docId;
    std::string title;
    
    for(int i = 0; (!tempQueue.empty() && i < maxDocsLimit); i++)
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
            this->stemWord(word);
            std::string data(word);
            if(data.size() > 2 && !this->classifiers.isStopWord(data))
            {
                searchTerms[data]++;
            }
            word[0] = '\0';
        }
    }

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
    std::string searchQuery;
    std::unordered_map<std::string, int> searchTerms;
    std::unordered_map<std::string, double> docTfidfMap;
    std::vector<std::pair<std::string, std::string>> topKDocs;
    while(searchQuery != "q" || searchQuery != "exit")
    {
        std::cout << "\n\nEnter the query to search: ";
        std::getline(std::cin, searchQuery);
        if (searchQuery == "q" || searchQuery == "exit")
            break;
        
        std::cout << "Search results for query: " << searchQuery << "\n";

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

std::string& Search::getTitleFromDocId(std::string docId)
{
    return docIdTitleMap[docId];
}

void Search::loadInvertedIndex(FileIO &file)
{
    std::cout << "Loading Search Engine...\n";
    std::cout << "Enter q or exit to quit search\n\n";

    file.initialiseDictFiles(std::ios::in);
    file.initialisePostingLists(std::ios::in);
    dictBuffer = file.getDictBuffer();
    postingListsBuffer = file.getPostingListBuffer();

    this->initializeStemmer();

    for(int i = 0; i < 26; i++)
    {
        int lineNumber = 0;
        while(dictBuffer[i]->sgetc() != EOF)
        {
            std::string line;
            char c;
            while((c = dictBuffer[i]->sbumpc()) != '\n')
            {
                line.push_back(c);
            }
            std::string word = line.substr(0, line.find(":"));
            if(word != "")
            {
                int offset = std::stoi(line.substr(line.find(":") + 1)) + lineNumber;
                invertedIndex[word] = offset;
            }
            lineNumber++;
        }
    }
}

void Search::loadMetadata(FileIO &file)
{
    auto initialised = file.initialise();
    std::string indexFolderPath = initialised.second;
    std::filebuf *MetadataBuffer = initialised.first.second;

    std::string metaPath = indexFolderPath + "/meta/id_title.txt";
    MetadataBuffer->open(metaPath, std::ios::in);
    if(!MetadataBuffer->is_open()) throw std::runtime_error("Unable to open Metadata file");

    std::string docId;
    std::string title;

    while(MetadataBuffer->sgetc() != EOF)
    {
        std::string line;
        char c;
        while((c = MetadataBuffer->sbumpc()) != '\n')
        {
            line.push_back(c);
        }
        docId = line.substr(0, line.find(":"));
        title = line.substr(line.find(":") + 1);
        docIdTitleMap[docId] = title;
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
    word[stem(this->stemmer, word, strlen(word) - 1) + 1] = '\0';
}

void Search::freeStemmer()
{
    free_stemmer(this->stemmer);
}
