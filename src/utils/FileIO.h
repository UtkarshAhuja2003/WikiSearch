#ifndef FileIO_H
#define FileIO_H

#include <fstream>
#include <queue>
#include <iostream>
#include <map>
#define FilesCount 26

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
        void writeDataToTemporaryFile(std::string &data, int tempFileNumber);
        void dumpTemporaryFileToDisk();
        void writeL1Metadata(std::map<int, std::string> &docIdTitleMap);
        std::string writeL2Metadata(std::map<int, std::string> &docIdTitleMap);
        std::pair<std::string, std::map<int, std::string>::iterator> writeL3Metadata(std::map<int, std::string> &docIdTitleMap, auto it);
        std::pair<std::string, std::map<int, std::string>::iterator> writeDocIdTitle(std::map<int, std::string> &docIdTitleMap, auto it);
        void dumpMetadataToDisk();
        void mergeTemporaryFiles(int tempFileCount);
        std::pair<std::string, std::string> getPostingList(std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string>> &invertedIndexList);
        std::vector<std::filebuf *> getPostingListBuffer();
        std::vector<std::filebuf *> getDictBuffer();
        std::filebuf * getMetadataBuffer();
};

#endif
