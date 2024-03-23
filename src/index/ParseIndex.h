#ifndef PARSE_INDEX_H
#define PARSE_INDEX_H

#include <expat/lib/expat.h>
#include <iostream>
#include <cstring>
#include <map>
#include <stack>
#include "../utils/ErrorHandler.h"
#include "WikiPage.h"

#define BUFFER_SIZE (5 * 1024 * 1024) // 5 MB

class ParseIndex
{
    private:
        std::string wikiDump;
        std::map<std::string, long> wikiIndexes;
        std::stack<std::string> tagStack;
        WikiPage currentWikiPage;

    public:
        void buildIndex();
        void parseWikiPage();
        ParseIndex(const std::string& wikiFilePath);
        const std::map<std::string, long>& getWikiIndexes() const;
        const std::string& getWikiDump() const;
        void setWikiDump(const std::string& wikiDump);
        void start(void *userData, const char *name, const char **args);
        void value(void *userData, const char *val, int len);
        void end(void *userData, const char *name);
};


#endif
