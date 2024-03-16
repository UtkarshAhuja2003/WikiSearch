#include <iostream>
#include "ParseIndex.h"

void start(void *userData, const char *name, const char *args[])
{

}

void value(void *userData, const char *val, int len)
{

}

void end(void *userData, const char *name){

}

void ParseIndex::parseWikiPage(const WikiPage& wikiPage)
{
    
}

void ParseIndex::buildIndex()
{
    XML_Parser parser = XML_ParserCreate(NULL);
    if(!parser)
    {
        throw std::runtime_error("Error creating XML parser");
    }

    XMLCall(parser, XML_SetElementHandler(parser, start, end));

    XMLCall(parser, XML_SetCharacterDataHandler(parser, value));

    XMLCall(parser, XML_ParserFree(parser));

}
