#ifndef Search_H
#define Search_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <map>
#include <math.h>
#include "../utils/Classifiers.h"
#include "../utils/FileIO.h"
extern "C" {
    #include "../utils/Stemmer.h"
}

#define maxDocsLimit 15

class Search
{
    private:
        struct stemmer *stemmer;
        Classifiers classifiers;
        std::vector<std::fstream> postingListStreams;
        std::vector<std::fstream> dictStreams;
        std::unordered_map<std::string, int> invertedIndex;
        std::map<int, int> l1MetadataMap;
        std::vector<std::filebuf *> metadataFileBuffers;
        
    public:
        void search();
        std::vector<std::pair<std::string, std::string>> search(std::string searchQuery);
        std::vector<std::pair<std::string,int>> getPostingListForSingleTerm(std::string word);
        void getPostingList(std::unordered_map<std::string, int> &searchTerms, std::unordered_map<std::string, double> &docTfidfMap);
        void processSearchQuery(std::string query, std::unordered_map<std::string, int> &searchTerms);
        void calculateTopKDocs(std::unordered_map<std::string, double> &docTfidfMap, std::vector<std::pair<std::string, std::string>> &topKDocIds);
        void loadInvertedIndex(FileIO &file);
        std::string getTitleFromDocId(std::string docId);
        std::string searchL2Metadata(int docId, int offset);
        std::string searchL3Metadata(int docId, int offset);
        std::string searchMetadata(int docId, int offset);
        void loadL1Metadata(FileIO &file);
        double tfidf(int tf, int df);
        void initializeStemmer();
        void stemWord(char word[]);
        void freeStemmer();
};


#endif