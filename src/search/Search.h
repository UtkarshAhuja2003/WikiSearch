#ifndef Search_H
#define Search_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <math.h>
#include "../utils/Classifiers.h"
#include "../utils/FileIO.h"
extern "C" {
    #include "../utils/Stemmer.h"
}

#define K 4

class Search
{
    private:
        struct stemmer *stemmer;
        Classifiers classifiers;
        std::vector<std::filebuf *> postingListsBuffer;
        std::vector<std::filebuf *> dictBuffer;
        std::unordered_map<std::string, int> invertedIndex;
    public:
        void search();
        std::vector<std::string> getPostingListForSingleTerm(std::string word);
        void getPostingList(std::unordered_map<std::string, int> &searchTerms, std::unordered_map<std::string, double> &docTfidfMap);
        void processSearchQuery(std::string query, std::unordered_map<std::string, int> &searchTerms);
        void calculateTopKDocs(std::unordered_map<std::string, double> &docTfidfMap, std::vector<std::string> &topKDocIds);
        void loadInvertedIndex();
        double tfidf(int tf, int df);
        void initializeStemmer();
        void stemWord(char word[]);
        void freeStemmer();
};


#endif