#ifndef FileIO_H
#define FileIO_H

#include <fstream>

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
        void writeDataToTemporaryFile(std::string &data, char fileChar);
        void dumpTemporaryFileToDisk();
};

#endif
