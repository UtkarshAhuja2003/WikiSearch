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

void FileIO::writeDataToTemporaryFile(std::string &data, int tempFileNumber)
{
    try
    {
        std::string filePath = indexFolderPath + "/temp/index" + std::to_string(tempFileNumber) + ".txt";
        FileBuffer->open(filePath, std::ios::out | std::ios::app);
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
        FileBuffer->close();
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(std::string("Error in Dumping Temporary file to Disk : ") + e.what());
    }
}

void FileIO::mergeTemporaryFiles()
{
    
}
