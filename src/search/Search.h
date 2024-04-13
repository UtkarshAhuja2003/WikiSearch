#ifndef Search_H
#define Search_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "../utils/Classifiers.h"
#include "../utils/FileIO.h"
extern "C" {
    #include "../utils/Stemmer.h"
}

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
        std::vector<std::string> getPostingList(std::string query);
        void loadInvertedIndex();
        void initializeStemmer();
        void stemWord(char word[]);
        void freeStemmer();
};


#endif