#include "ParseIndex.h"

void ParseIndex::start(void *userData, const char *name, const char **args)
{
    this->tagStack.push(name);
}

void ParseIndex::value(void *userData, const char *val, int len)
{
    std::string currentTag = this->tagStack.top();
    
    WikiPage& currentPage = this->currentWikiPage;
    if(currentTag == "id" && currentPage.getPageId() == "")
    {
        currentPage.setPageId(val);
    }
    else if(currentTag == "title")
    {
        currentPage.setPageTitle(val);
    }
    else if(currentTag == "text")
    {
        currentPage.setPageText(val);
        this->parseWikiPage();
    }
}

void ParseIndex::end(void *userData, const char *name)
{
    std::string endTag = this->tagStack.top();
    this->tagStack.pop();
    if(endTag == "page")
    {
        this->currentWikiPage.clear();
    }
}


void ParseIndex::parseWikiPage()
{
    
}


void ParseIndex::buildIndex()
{
    XML_Parser parser = XML_ParserCreate(NULL);
    if(!parser)
    {
        throw std::runtime_error("Error creating XML parser");
    }

    XMLCall(parser, XML_SetElementHandler(parser, 
        [](void* userData, const char* name, const char** args) 
        {
            static_cast<ParseIndex*>(userData)->start(userData, name, args);
        },
        [](void* userData, const char* name) 
        {
            static_cast<ParseIndex*>(userData)->end(userData, name);
        }
    ));

    XMLCall(parser, XML_SetCharacterDataHandler(parser, 
        [](void* userData, const char* val, int len) 
        {
            static_cast<ParseIndex*>(userData)->value(userData, val, len);
        }
    ));

    XMLCall(parser, XML_SetUserData(parser, this));

    FILE *wikiData = fopen(this->wikiDump.c_str(),"r");
    if(!wikiData) throw std::runtime_error("Error Opening Wiki Dump");

    int done;
    do {
        void *buff = XMLCall(parser, XML_GetBuffer(parser, BUFFER_SIZE));
        if(buff == NULL) throw std::runtime_error("Error: Empty Buffer");

        int len = (int)fread(buff, 1, BUFFER_SIZE, wikiData);
        if(len < 0) throw std::runtime_error("Error Reading Wiki Dump");
        done = len < BUFFER_SIZE;

        XMLCall(parser, XML_ParseBuffer(parser, len, done));
    } while (!done);

    fclose(wikiData);

    XMLCall(parser, XML_ParserFree(parser));

}


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
