#ifndef Search_H
#define Search_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <map>
#include <math.h>
#include "../utils/Classifiers.h"
#include "../utils/FileIO.h"
extern "C" {
    #include "../utils/Stemmer.h"
}

constexpr int MAX_DOCS_LIMIT = 15; // Maximum number of Top K doc results

/**
 * @class Search
 * @brief The Search class is responsible for performing search operations on the SearchQuery.
 * 
 * This class provides methods to search for documents based on a search query, retrieve posting lists for single terms,
 * process search queries, calculate top K documents, load inverted index and metadata.
 */
class Search
{
    private:
        struct stemmer *stemmer;
        Classifiers classifiers;
        std::vector<std::fstream*> postingListStreams;
        std::vector<std::fstream*> dictStreams;
        std::unordered_map<std::string, int> invertedIndex; // `DocID - Offset`(Posting List)
        std::map<int, int> l1MetadataMap;
        std::vector<std::filebuf *> metadataFileBuffers;
        
    public:
        /**
         * Performs a search based on user inputs in a loop.
         * 
         * This function takes user input query, processes it, retrieves relevant documents,
         * and displays the search results.
         * It prompts the user to enter 'q' or 'exit' to quit the search.
         */
        void search();

        /**
         * Performs a search based on the given search query.
         * 
         * @param searchQuery The search query to be used for searching.
         * @return A vector of pairs, where each pair contains the document ID and its corresponding Title.
         */
        std::vector<std::pair<std::string, std::string>> search(std::string searchQuery);


        /**
         * Retrieves the posting list for a single term from the inverted index.
         * 
         * @param word The term to retrieve the posting list for.
         * @return The posting list as a vector of pairs, where each pair contains a document ID and its frequency.
         * @throws std::out_of_range if the start character of the term is invalid.
         * @throws std::exception if an exception occurs while converting the file content to a posting list.
         */
        std::vector<std::pair<std::string,int>> getPostingListForSingleTerm(std::string word);
      
        /**
         * Retrieves the posting list for the given search terms and updates the document TF-IDF map.
         *
         * @param searchTerms The unordered map containing the search terms and their frequencies.
         * @param docTfidfMap The unordered map containing the document IDs and their corresponding TF-IDF values.
         */
        void getPostingList(std::unordered_map<std::string, int> &searchTerms, std::unordered_map<std::string, double> &docTfidfMap);

        /**
         * Processes the search query by tokenising words, stemming them, and adding them to the search terms map.
         * 
         * @param query The search query string.
         * @param searchTerms The map to store the search terms and their frequencies.
         */
        void processSearchQuery(std::string query, std::unordered_map<std::string, int> &searchTerms);

        /**
         * Calculates the top K documents based on the TF-IDF values in the given document TF-IDF map.
         * The top K documents are stored in the provided vector of pairs, where each pair contains the document ID and its title.
         *
         * @param docTfidfMap The document TF-IDF map containing the TF-IDF values for each document.
         * @param topKDocIds The vector of pairs to store the top K documents, where each pair contains the document ID and its title.
         */
        void calculateTopKDocs(std::unordered_map<std::string, double> &docTfidfMap, std::vector<std::pair<std::string, std::string>> &topKDocIds);

        /**
         * Loads the inverted index from the given file.
         * 
         * This function initializes the search engine by loading the inverted index `DocID-Offset` into Inverted Index map from the Dict files.
         * 
         * @param file The FileIO object used to read the inverted index file.
         */ 
        void loadInvertedIndex(FileIO &file);

        /**
         * Retrieves the title of a document based on its ID.
         * 
         * Finds previous smaller id in L1 Metadata and calls L2 Metadata search.
         * 
         * @param docId The ID of the document.
         * @return The title of the document.
         */
        std::string getTitleFromDocId(std::string docId);

        /**
         * Searches for L2 metadata based on the given document ID and L2 offset.
         * 
         * Finds previous smaller id in L2 Metadata and calls L3 Metadata search.
         * 
         * @param docId The document ID to search for.
         * @param offset The offset to start searching from.
         * @return The title of the document.
         * @throws std::exception if there is an error searching the L2 metadata.
         */
        std::string searchL2Metadata(int docId, int offset);

        /**
         * Searches for L3 metadata based on the given document ID and L3 offset.
         * 
         * Finds previous smaller id in L3 Metadata and calls DocID-Title Metadata search.
         * 
         * @param docId The document ID to search for.
         * @param offset The offset to start searching from.
         * @return The title of the document.
         * @throws std::exception if there is an error searching the L2 metadata.
         */
        std::string searchL3Metadata(int docId, int offset);

        /**
         * Searches for DocID - Title metadata based on the given document ID and offset.
         * 
         * Finds the docID equal to the given id.
         * 
         * @param docId The document ID to search for.
         * @param offset The offset to start searching from.
         * @return The title of the document.
         * @throws std::exception if there is an error searching the L2 metadata.
         */
        std::string searchMetadata(int docId, int offset);

        /**
         * Loads the L1 metadata and open Metadata Buffer files.
         *
         * It reads the L1 metadata file line by line,
         * extracts the document ID and offset(L2 Metadata), and stores them in the l1MetadataMap.
         *
         * @param file The FileIO object used to access the metadata files.
         * @throws std::runtime_error if any of the metadata files fail to open.
         */
        void loadL1Metadata(FileIO &file);

        /**
         * Calculates the TF-IDF `tf` Term Frequency (number of times the term appeared in searchQuery) `df` The document frequency (number of documents in the collection that contain the term).
         * @return The TF-IDF score for the term.
         */
        double tfidf(int tf, int df);

        /**
         * @brief Stems a word using the stemmer object.
         * 
         * This function takes a word as input and applies stemming using the stemmer object.
         * The stemmed word is then stored back in the input word array.
         * 
         * @param word The word to be stemmed.
         */
        void stemWord(char word[]);

        void initializeStemmer();
        void freeStemmer();
};


#endif