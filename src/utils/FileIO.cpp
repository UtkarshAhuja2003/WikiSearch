#include "FileIO.h"

FileIO::FileIO(std::string indexFolderPath)
{
    this->indexFolderPath = indexFolderPath;
}

void FileIO::initialise()
{
    try
    {
        FileBuffer = stream.rdbuf();
        std::string filePath = indexFolderPath + "/temp/index.txt";
        FileBuffer->open(filePath, std::ios::out | std::ios::app);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
}

void FileIO::close()
{
    try
    {
        FileBuffer->close();
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
}

void FileIO::writeDataToTemporaryFile(std::string &data, char fileChar)
{
    try
    {
        FileBuffer->sputn(data.c_str(), data.length());
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
}

void FileIO::dumpTemporaryFileToDisk()
{
    try
    {
        if(FileBuffer != NULL)
        {
            FileBuffer->close();
            FileBuffer = NULL;
        }
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
}

