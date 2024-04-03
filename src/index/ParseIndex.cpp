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
        currentPage.setPageId(std::string(val,len));
    }
    else if(currentTag == "title")
    {
        currentPage.setPageTitle(std::string(val,len));
    }
    else if(currentTag == "text")
    {
        currentPage.getPageText().append(std::string(val,len));
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
    if(endTag == "text")
    {
        this->parseWikiPage();
    }
}


void ParseIndex::parseWikiPage()
{
    const std::string& text = this->currentWikiPage.getPageText();
    char word[1000] = "";
    
    for(char c : text)
    {
        if (isalpha(c))
        {
            c = tolower(c);
            strncat(word, &c, 1);
        }
        else
        {
            this->stemWord(word);
            std::string wordString(word);
            if(wordString.size() > 1 && !this->classifiers.isStopWord(wordString))
            {
                this->wikiIndexes[wordString].insert(this->currentWikiPage.getPageId());
            }
            word[0] = '\0';
        }
    }
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

    std::ifstream stream;
    std::filebuf *wikiData = stream.rdbuf();
    wikiData->open(this->wikiDump, std::ios::in);
    
    if(!wikiData) throw std::runtime_error("Error Opening Wiki Dump");

    this->initializeStemmer();

    int done;
    do {
        void *buff = XMLCall(parser, XML_GetBuffer(parser, BUFFER_SIZE));
        if(buff == NULL) throw std::runtime_error("Error: Empty Buffer");

        int len = (int)wikiData->sgetn((char*)buff, BUFFER_SIZE);
        if(len < 0) throw std::runtime_error("Error Reading Wiki Dump");
        done = len < BUFFER_SIZE;

        XMLCall(parser, XML_ParseBuffer(parser, len, done));
    } while (!done);

    wikiData->close();

    this->freeStemmer();

    XMLCall(parser, XML_ParserFree(parser));

}


const std::map<std::string, std::set<std::string>>& ParseIndex::getWikiIndexes() const {
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

void ParseIndex::initializeStemmer()
{
    this->stemmer = create_stemmer();
}

void ParseIndex::stemWord(char word[])
{
    word[stem(this->stemmer, word, strlen(word) - 1) + 1] = '\0';
}

void ParseIndex::freeStemmer()
{
    free_stemmer(this->stemmer);
}