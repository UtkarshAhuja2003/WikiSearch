#ifndef PARSE_INDEX_H
#define PARSE_INDEX_H

#include <expat/lib/expat.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <unordered_set>
#include "WikiPage.h"
#include "../utils/FileIO.h"
#include "../utils/Classifiers.h"
#include "../utils/ErrorHandler.h"
extern "C" {
    #include "../utils/Stemmer.h"
}

#define BUFFER_SIZE (1024)

class ParseIndex
{
    private:
        std::string wikiDump;
        int tempFileNumber = 0;
        int numberOfPages = 0;
        std::map<std::string,std::unordered_set<std::string>> invertedIndex;
        std::stack<std::string> tagStack;
        WikiPage currentWikiPage;
        Classifiers classifiers;
        struct stemmer *stemmer;
        FileIO file{"../res"};

    public:
        void buildIndex();
        void parseWikiPage();
        void dumpInvertedIndexToDisk();
        std::map<std::string,std::unordered_set<std::string>>& getInvertedIndex();
        ParseIndex(const std::string& wikiFilePath);
        const std::string& getWikiDump() const;
        void setWikiDump(const std::string& wikiDump);
        void start(void *userData, const char *name, const char **args);
        void value(void *userData, const char *val, int len);
        void end(void *userData, const char *name);
        void initializeStemmer();
        void stemWord(char word[]);
        void freeStemmer();
        int getNumberOfTempFiles();
};


#endif
