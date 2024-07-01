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
#include <unordered_map>
#include "WikiPage.h"
#include "../utils/FileIO.h"
#include "../utils/Classifiers.h"
#include "../utils/ErrorHandler.h"
extern "C" {
    #include "../utils/Stemmer.h"
}

constexpr int BUFFER_SIZE = 1024; // Size in KB.

class ParseIndex
{
    private:
        std::string wikiDump; // Path to Wikipedia Dump
        int tempFileNumber = 0;
        int numberOfPages = 0;
        std::map<std::string,std::unordered_map<std::string,char>> invertedIndex; // Word - [ID - Freq]
        std::vector<std::pair<std::string, std::string>> docIdTitle; // ID - Title
        std::stack<std::string> tagStack; // Tags of the current Wikipage in a stack (LIFO).
        WikiPage currentWikiPage;
        Classifiers classifiers;
        struct stemmer *stemmer;
        FileIO file{"../res"};

    public:
        /**
         * @brief Builds the index by parsing the XML file.
         * 
         * @throws std::runtime_error if there is an error creating the XML parser, opening the XML file,
         *                            reading the XML file, or parsing the XML buffer.
         */
        void buildIndex();

        /**
         * @brief Starts the parsing process.
         * 
         * This function is called when the parser encounters the start of an element.
         * It pushes the name of the element onto the tag stack.
         * 
         * @param userData A pointer to user-defined data.
         * @param name The name of the element.
         * @param args An array of attribute name-value pairs.
         */
        void start(void *userData, const char *name, const char **args);

        /**
         * @brief Sets the value of the current tag in the XML document.
         * 
         * This function is called when parsing an XML document and sets the value of the current tag
         * based on the provided `val` and `len` parameters. It updates the corresponding properties
         * of the `currentPage` object based on the current tag.
         * 
         * @param userData A pointer to user-defined data.
         * @param val The value of the current tag.
         * @param len The length of the value.
         */
        void value(void *userData, const char *val, int len);

        /**
         * @brief This function is called when the end of an XML element is encountered during parsing.
         * 
         * @param userData A pointer to user-defined data.
         * @param name The name of the XML element.
         */
        void end(void *userData, const char *name);

        /**
         * @brief Parses a wiki page and processes the words and title in the page.
         * 
         * It processes the words in the text,
         * filtering out unwanted part and storing the processed words in the inverted index.
         * 
         * @throws std::exception if there is an error parsing the wiki page.
         */
        void parseWikiPage();

        /**
         * Processes a word and updates the inverted index.
         * 
         * @param word The word to be processed.
         * @param id The identifier associated with the word.
         */
        void processWord(char word[], const std::string& id);
 
        /**
         * @brief Dumps the inverted index to disk.
         * 
         * The data is written in the following format: `term:docId1-freq1;docId2-freq2;`
         * 
         * @note This function assumes that the file object has been properly initialized and opened before calling this function.
         */
        void dumpInvertedIndexToDisk();

        std::map<std::string,std::unordered_map<std::string,char>>& getInvertedIndex();
        ParseIndex(const std::string& wikiFilePath);
        const std::string& getWikiDump() const;
        void setWikiDump(const std::string& wikiDump);
        void initializeStemmer();
        void stemWord(char word[]);
        void freeStemmer();
        int getNumberOfTempFiles();
};


#endif
