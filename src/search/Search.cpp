#include "Search.h"

std::vector<std::string> Search::getPostingListForSingleTerm(std::string word)
{
    std::vector<std::string> postingList;
    if(invertedIndex.find(word) == invertedIndex.end()) return postingList;
    int offset = invertedIndex[word];
    int startChar = word[0] - 'a';
    postingListsBuffer[startChar]->pubseekpos(offset);
    std::string posting;
    char c;
    while((c = postingListsBuffer[startChar]->sbumpc()) != '\n')
    {
        posting.push_back(c);
    }

    std::string index;
    for(char c : posting)
    {
        if(c == ';')
        {
            postingList.emplace_back(index);
            index.clear();
        }
        else{
            index.push_back(c);
        }
    }

    return postingList;
}

void Search::getPostingList(std::unordered_map<std::string, int> &searchTerms, std::unordered_map<std::string, double> &docTfidfMap)
{
    for(auto currentTerm : searchTerms)
    {
        std::vector<std::string> currentPosting = getPostingListForSingleTerm(currentTerm.first);
        for(std::string docId : currentPosting)
        {
            // TODO: Take original termfrequency
            int termFrequency = 1;
            double prevTfidf = (docTfidfMap.find(docId) == docTfidfMap.end()) ? 0 : docTfidfMap[docId];
            double currTfidf = tfidf(termFrequency, currentPosting.size());
            docTfidfMap[docId] = (prevTfidf + currTfidf);
        }
    }
}

void Search::calculateTopKDocs(std::unordered_map<std::string, double> &docTfidfMap, std::vector<std::string> &topKDocIds)
{

    std::priority_queue<std::pair<double, std::string>> tempQueue;

    for(auto currDocTfidf : docTfidfMap)
    {
        tempQueue.push({currDocTfidf.second, currDocTfidf.first});
    }
    for(int i = 0; (!tempQueue.empty() && i < K); i++)
    {
        topKDocIds.push_back(tempQueue.top().second);
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
    std::vector<std::string> topKDocIds;
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
        calculateTopKDocs(docTfidfMap, topKDocIds);
        for(const std::string& docId : topKDocIds)
        {
            std::cout << docId << " ";
        }

        searchTerms.clear();
        docTfidfMap.clear();
        topKDocIds.clear();
        
    }

    this->freeStemmer();
}

void Search::loadInvertedIndex()
{
    std::cout << "Loading Search Engine...\n";
    std::cout << "Enter q or exit to quit search\n\n";

    FileIO file("../../res");
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
