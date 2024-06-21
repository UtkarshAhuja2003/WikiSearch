#include "ParseIndex.h"

void ParseIndex::start(void *userData, const char *name, const char **args)
{
    this->tagStack.push(name);
}

void ParseIndex::value(void *userData, const char *val, int len)
{
    try
    {
        const std::string& currentTag = this->tagStack.top();
    
        WikiPage& currentPage = this->currentWikiPage;
        if(currentTag == "id" && currentPage.getPageId().empty())
        {
            currentPage.setPageId(std::string(val, len));
        }
        else if(currentTag == "title")
        {
            currentPage.setPageTitle(std::string(val, len));
        }
        else if(currentTag == "text")
        {
            currentPage.getPageText().append(val, len);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error Traversing Wikipedia Dump, WikiPage: " << this->currentWikiPage.getPageId() << "\n";
        std::cerr << e.what();
    }
    
}

void ParseIndex::end(void *userData, const char *name)
{
    this->tagStack.pop();
    if(strcmp(name, "page") == 0)
    {
        this->currentWikiPage.clear();
    }
    else if(strcmp(name, "text") == 0)
    {
        this->parseWikiPage();
    }
}


void ParseIndex::parseWikiPage()
{
    try
    {
        docIdTitle.push_back({this->currentWikiPage.getPageId(), this->currentWikiPage.getPageTitle()});
        const std::string& text = this->currentWikiPage.getPageText();
        const int textLength = text.length();
        char word[1000] = "";
        
        for(int i = 0; i < textLength; ++i)
        {
            if(strlen(word) > 500)
            {
                word[0] = '\0';
            }
            char currentChar = text[i];
            if (isalpha(currentChar))
            {
                currentChar = tolower(currentChar);
                strncat(word, &currentChar, 1);
            }
            else if(currentChar == '{' && text[i+1] =='{')
            {
                int count = 2;
                i += 2;
                while(i < textLength)
                {
                    if(text[i] == '{')++count;
                    else if(text[i] == '}')--count;
                    if(count == 0)break;
                    ++i;
                }
            }
            else if(currentChar == '[' && text[i+1] ==']')
            {
                int count = 2;
                i += 2;
                while(i < textLength)
                {
                    if(text[i] == '[')++count;
                    else if(text[i] == ']')--count;
                    if(count == 0)break;
                    ++i;
                }
            }
            else if(currentChar == '=' && text[i+1] =='=')
            {
                i += 2;
                while(i < textLength)
                {
                    if(text[i] == '=' && text[i+1] == '=')break;
                    ++i;
                }
            }
            else
            {
                processWord(word);
                word[0] = '\0';
            }
        }
        if(numberOfPages%2000 == 0)
        {
            dumpInvertedIndexToDisk();
            tempFileNumber++;
            invertedIndex.clear();
        }
        if(numberOfPages == 50000)
        {
            file.writeL1Metadata(docIdTitle);
            numberOfPages = 0;
            docIdTitle.clear();
        }
        numberOfPages++;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error Parsing WikiPage: " << this->currentWikiPage.getPageId() << "\n";
        std::cerr << e.what();
    }
    
}

void ParseIndex::processWord(char word[])
{
    this->stemWord(word);
    if(strlen(word) > 2 && !this->classifiers.isStopWord(word))
    {
        invertedIndex[word][currentWikiPage.getPageId()]++;
    }
}

void ParseIndex::dumpInvertedIndexToDisk()
{
    std::unordered_map<std::string,std::unordered_map<std::string,int>> &invertedIndex = getInvertedIndex();
    for(auto &index : invertedIndex)
    {
        std::string data = index.first + ':';
        for(auto docId_freq : index.second)
        {
            data.append(docId_freq.first);
            data.append("-");
            data.append(std::to_string(docId_freq.second));
            data.append(";");
        }
        data.append("\n");
        file.writeDataToTemporaryFile(data, tempFileNumber);
    }
    file.dumpTemporaryFileToDisk();
}

void ParseIndex::buildIndex()
{
    XML_Parser parser = XML_ParserCreate(NULL);
    if(!parser)
    {
        throw std::runtime_error("Error creating XML parser");
    }

    XMLCall(parser, XML_SetElementHandler(parser,
        [](void* userData, const char* name, const char** args) {
            static_cast<ParseIndex*>(userData)->start(userData, name, args);
        },
        [](void* userData, const char* name) {
            static_cast<ParseIndex*>(userData)->end(userData, name);
        }
    ));

    XMLCall(parser, XML_SetCharacterDataHandler(parser,
        [](void* userData, const char* val, int len) {
            static_cast<ParseIndex*>(userData)->value(userData, val, len);
        }
    ));

    XMLCall(parser, XML_SetUserData(parser, this));

    std::ifstream stream(this->wikiDump, std::ios::in | std::ios::binary);
    if (!stream) throw std::runtime_error("Error Opening Wiki Dump");

    file.initialise();
    this->initializeStemmer();

    int done;
    do {
        void* buff = XMLCall(parser, XML_GetBuffer(parser, BUFFER_SIZE));
        if (buff == NULL) throw std::runtime_error("Error: Empty Buffer");

        stream.read((char*)buff, BUFFER_SIZE);
        std::streamsize len = stream.gcount();
        if (len < 0) throw std::runtime_error("Error Reading Wiki Dump");
        done = len < BUFFER_SIZE;

        XMLCall(parser, XML_ParseBuffer(parser, (int)len, done));
    } while (!done);

    this->dumpInvertedIndexToDisk();

    file.writeL1Metadata(docIdTitle);

    this->freeStemmer();

    XMLCall(parser, XML_ParserFree(parser));
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

std::unordered_map<std::string,std::unordered_map<std::string,int>>& ParseIndex::getInvertedIndex()
{
    return this->invertedIndex;
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

int ParseIndex::getNumberOfTempFiles()
{
    return tempFileNumber;
}
