#ifndef PARSE_INDEX_H

#include <expat/lib/expat.h>
#include <map>
#include "WikiPage.h"

#define PARSE_INDEX_H
#define ASSERT(x) if(!(x)) throw std::runtime_error("Error in parsing XML");
#define XMLCall(p, x) x;\
    ASSERT(XMLLogCall(p, #x, __FILE__, __LINE__))

/**
 * @brief Logs XML parsing errors and returns whether an error occurred.
 * 
 * @param parser XML parser object.
 * @param functionName name of the function where the error occurred.
 * @param fileName name of the file where the error occurred.
 * @param lineNumber line number where the error occurred.
 * @return true if no error occurred, false otherwise.
 */
static bool XMLLogCall(XML_Parser parser, const char* functionName, const char* fileName, int lineNumber) {
    const XML_Error errorCode =  XML_GetErrorCode(parser);
    if(errorCode == XML_ERROR_NONE) return true;

    const XML_LChar* errorString = XML_ErrorString(errorCode);
    std::cerr << "[XML Error] (" << errorCode << ": " << errorString
              << ") in function " << functionName << " at " << fileName << ":" << lineNumber
              << " (Line: " << lineNumber << ")" << std::endl;
    return false;
}


class ParseIndex
{
    private:
        std::string wikiDump;
        std::map<std::string, long> wikiIndexes;

    public:
        void buildIndex();
        void parseWikiPage(const WikiPage& wikiPage);
        ParseIndex(const std::string& wikiFilePath);
        const std::map<std::string, long>& getWikiIndexes() const;
        const std::string& getWikiDump() const;
        void setWikiDump(const std::string& wikiDump);

};

const std::map<std::string, long>& ParseIndex::getWikiIndexes() const {
    return wikiIndexes;
}

ParseIndex::ParseIndex(const std::string& wikiFilePath)
{
    this->wikiDump = wikiFilePath;
}

const std::string& ParseIndex::getWikiDump() const {
    return wikiDump;
}

void ParseIndex::setWikiDump(const std::string& wikiDump) {
    this->wikiDump = wikiDump;
}



#endif
