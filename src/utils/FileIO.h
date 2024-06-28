#ifndef FileIO_H
#define FileIO_H

#include <fstream>
#include <queue>
#include <iostream>
#include <map>
#include <vector>
#define FilesCount 26
#define L2MetadataLimit 1000
#define L3MetadataLimit 50
#define MetadataLimit 10

class FileIO
{
    private:
        std::fstream stream;
        std::filebuf *FileBuffer = NULL;
        std::fstream metadataStream;
        std::filebuf *MetadataFileBuffer = NULL;
        std::fstream l1MetadataStream,l2MetadataStream,l3MetadataStream;
        std::filebuf *l1MetadataFileBuffer = NULL,*l2MetadataFileBuffer = NULL,*l3MetadataFileBuffer = NULL;
        std::string indexFolderPath;
        std::vector<std::fstream> postingListsStreams;
        std::vector<std::filebuf *> postingListsBuffer;
        std::vector<std::fstream> dictStreams;
        std::vector<std::filebuf *> dictBuffer;
    public:
        FileIO(std::string indexFolderPath);
        std::pair<std::pair<std::filebuf *, std::vector<std::filebuf *>>, std::string> initialise();
        void initialisePostingLists(std::_Ios_Openmode openMode);
        void initialiseDictFiles(std::_Ios_Openmode openMode);
        void close();
        void openFile(int tempFileNumber);
        void writeDataToTemporaryFile(std::string &data);
        void dumpTemporaryFileToDisk();
        void writeL1Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle);
        std::string writeL2Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle);
        std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> writeL3Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it);
        std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> writeDocIdTitle(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it);
        void dumpMetadataToDisk();
        void mergeTemporaryFiles(int tempFileCount);
        std::pair<std::pair<std::string, std::string>, std::vector<int>> getPostingList(std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, std::greater<std::pair<std::string, int>>> &invertedIndexList);
        std::vector<std::filebuf *> getPostingListBuffer();
        std::vector<std::filebuf *> getDictBuffer();
        std::filebuf * getMetadataBuffer();
};

#endif
