#ifndef PARSE_INDEX_H
#define PARSE_INDEX_H

#include <expat/lib/expat.h>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include "WikiPage.h"
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
        std::map<std::string, std::set<std::string>> wikiIndexes;
        std::stack<std::string> tagStack;
        WikiPage currentWikiPage;
        Classifiers classifiers;
        struct stemmer *stemmer;

    public:
        void buildIndex();
        void parseWikiPage();
        ParseIndex(const std::string& wikiFilePath);
        const std::map<std::string, std::set<std::string>>& getWikiIndexes() const;
        const std::string& getWikiDump() const;
        void setWikiDump(const std::string& wikiDump);
        void start(void *userData, const char *name, const char **args);
        void value(void *userData, const char *val, int len);
        void end(void *userData, const char *name);
        void initializeStemmer();
        void stemWord(char word[]);
        void freeStemmer();
};


#endif
