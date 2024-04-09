#ifndef FileIO_H
#define FileIO_H

#include <fstream>
#include <queue>
#include <iostream>

class FileIO
{
    private:
        std::fstream stream;
        std::filebuf *FileBuffer = NULL;
        std::string indexFolderPath;
    public:
        FileIO(std::string indexFolderPath);
        void initialise();
        void close();
        void writeDataToTemporaryFile(std::string &data, int tempFileNumber);
        void dumpTemporaryFileToDisk();
        void mergeTemporaryFiles(int tempFileCount);
        std::pair<std::string, std::string> getPostingList(std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string>> &invertedIndexList);
};

#endif
