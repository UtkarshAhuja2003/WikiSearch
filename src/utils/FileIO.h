#ifndef FileIO_H
#define FileIO_H

#include <fstream>
#include <queue>
#include <iostream>
#include <map>
#include <vector>
constexpr int FILES_COUNT = 26; // Number of dictionary and postings files (A-Z)
constexpr int L2_METADATA_LIMIT = 1000; // Maximum number of ID-offset pairs in Level 2 metadata for one ID from Level 1 metadata
constexpr int L3_METADATA_LIMIT = 50; // Maximum number of ID-offset pairs in Level 3 metadata for one ID from Level 2 metadata
constexpr int METADATA_LIMIT = 10; // Maximum number of ID-Title pairs in metadata for one ID from Level 3 metadata

/**
 * @class FileIO
 * The FileIO class provides functionality for file input/output operations.
 * 
 * The FileIO class is responsible for managing file streams, metadata, posting lists, and dictionary files.
 * It provides methods for initializing, writing data, dumping to disk, merging temporary files, and retrieving posting lists.
 * 
 * @note Resources folder path is provided during construction.
 */
class FileIO
{
    private:
        std::fstream stream;
        std::filebuf *FileBuffer = NULL;
        std::fstream metadataStream;
        std::filebuf *MetadataFileBuffer = NULL;
        std::fstream l1MetadataStream,l2MetadataStream,l3MetadataStream;
        std::filebuf *l1MetadataFileBuffer = NULL,*l2MetadataFileBuffer = NULL,*l3MetadataFileBuffer = NULL;
        std::string resourcesFolderPath; // Path to the directory containing resources, posting lists, metadata 
        std::vector<std::fstream*> postingListsStreams;
        std::vector<std::fstream*> dictStreams;
    public:
        /**
         * @brief Constructs a FileIO object with the specified resources folder path.
         * 
         * @param resourcesFolderPath The path to the resources folder.
         */
        FileIO(std::string resourcesFolderPath);

        
        /**
         * @brief Initializes the file buffers
         *
         * @return A pair containing the file buffers and the resources folder path.
         *         The first element of the pair is a pair containing the main file buffer and a vector of metadata file buffers (Metadata, L1, L2, L3).
         *         The second element of the pair is the resources folder path.
         *
         * @throws std::runtime_error if an exception occurs during initialization.
         */
        std::pair<std::pair<std::filebuf *, std::vector<std::filebuf *>>, std::string> initialise();

        void close();
        void dumpMetadataToDisk();
        void writeDataToTemporaryFile(std::string &data);
        void dumpTemporaryFileToDisk();

        /**
         * Opens a tempfile with the specified file number.
         *
         * @param tempFileNumber The file number to open.
         */
        void openFile(int tempFileNumber);

        /**
         * Writes the Level1 metadata to a file.
         *
         * The Level1 metadata includes the `DocumentID:Level2 Offset`
         *
         * @param docIdTitle A reference to a vector of pairs containing document IDs and titles.
         * @throws std::runtime_error if the Level 1 metadata file cannot be opened.
         * @throws Any exception thrown by the `writeL2Metadata` function.
         */
        void writeL1Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle);

        /**
         * Writes the Level2 metadata to a file.
         * 
         * The Level2 metadata includes the `DocumentID:Level3 Offset`
         * 
         * @param docIdTitle A reference to a vector of document ID and title pairs.
         * @return offset where the Level2 metadata is written.
         * @throws std::runtime_error if the Level2 metadata file cannot be opened.
         * @throws Any other exception that occurs during the writing process.
        */
        std::string writeL2Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle);

        /**
         * Writes L3 metadata to a file.
         * 
         * The Level3 metadata includes the `DocumentID:DocID-Title Offset`
         * 
         * @param docIdTitle A reference to a vector of document ID-title pairs.
         * @param it An iterator pointing to the current position in the vector.
         * @return A pair containing the offset where the Level3 metadata is written and the updated iterator.
         * @throws std::runtime_error if the L3 metadata file cannot be opened.
         * @throws Any other exception that occurs during the writing process.
         */
        std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> writeL3Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it);


        /**
         * Writes the document ID and title pairs to a metadata file.
         *
         * @param docIdTitle A reference to a vector of pairs containing document ID and title.
         * @param it An iterator pointing to the current position in the vector.
         * @return A pair containing the offset where the DocId-Title metadata is written and the updated iterator.
         * @throws std::runtime_error if the metadata file cannot be opened.
         */
        std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> writeDocIdTitle(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it);

        /**
         * Initializes the posting list files by opening the corresponding files.
         * 
         * @param openMode The open mode for the file streams.
         */
        void initialisePostingLists(std::_Ios_Openmode openMode);

        /**
         * Initializes the dictionary files by opening the corresponding files.
         * 
         * @param openMode The open mode for the file streams.
         */
        void initialiseDictFiles(std::_Ios_Openmode openMode);

        /**
         * Merges the sorted temporary files created during the index parsing using SPMI.
         *
         * This function takes the number of temporary files as input and merges them into dictionary and posting list files.
         * The Dict files contain the Word-Offset pairs.
         * The posting list files contains the IDs for a word in (ID-Frequency) format
         *
         * @param tempFileCount The number of temporary files to be merged.
         * @note The temp file's data must be sorted by word lexicographically.
         */
        void mergeTemporaryFiles(int tempFileCount);

        /**
         * @brief Retrieves the posting list for the top word (lexicographically smallest) from the inverted index queue.
         *
         * The function iterates through the remaining
        * entries in the priority queue and appends their posting lists to the original one if they
        * have the same word.
         *
         * @param invertedIndexList The priority queue (Min Heap) of inverted index entries.
         * @return A pair containing the word and the posting list, with the document IDs removed from queue.
         * @throws std::runtime_error if there is an error getting the posting list.
         */
        std::pair<std::pair<std::string, std::string>, std::vector<int>> getPostingList(std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, std::greater<std::pair<std::string, int>>> &invertedIndexList);

        std::vector<std::fstream*> getPostingListStreams();
        std::vector<std::fstream*> getDictStreams();
        std::filebuf * getMetadataBuffer();
};

#endif
