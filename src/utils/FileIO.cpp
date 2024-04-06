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

void FileIO::mergeTemporaryFiles(int tempFileCount)
{
    std::vector<std::string> fileNames = {
        "indexa.txt","indexb.txt","indexc.txt","indexd.txt","indexe.txt",
        "indexf.txt","indexg.txt","indexh.txt","indexi.txt","indexj.txt",
        "indexk.txt","indexl.txt","indexm.txt","indexn.txt","indexo.txt",
        "indexp.txt","indexq.txt","indexr.txt","indexs.txt","indext.txt",
        "indexu.txt","indexv.txt","indexw.txt","indexx.txt","indexy.txt",
        "indexz.txt"
    };
    std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string>> invertedIndexList;
    std::vector<std::fstream> tempFileStreams(tempFileCount);
    std::vector<std::filebuf *> tempFileReader(tempFileCount);
    std::string tempFilePath = indexFolderPath + "/temp/index0.txt";
    std::vector<std::fstream> dictFileStreams(fileNames.size());
    std::vector<std::filebuf *> dictFileWriter(fileNames.size());
    std::string dictFilePath = indexFolderPath + "/index/";

    for(int i = 0; i < tempFileCount; i++)
    {
        tempFileReader[i] = tempFileStreams[i].rdbuf();
        tempFilePath[tempFilePath.length() - 5] = '0' + i;
        tempFileReader[i]->open(tempFilePath, std::ios::in);
    }

    for(int i = 0; i < fileNames.size(); i++)
    {
        dictFileWriter[i] = dictFileStreams[i].rdbuf();
        dictFileWriter[i]->open((dictFilePath + fileNames[i]), std::ios::out | std::ios::app);
    }

    while(!tempFileReader.empty())
    {
        for(int i = 0; i < tempFileReader.size(); i++)
        {
            if(tempFileReader[i]->sgetc() == EOF)
            {
                tempFileReader.erase(tempFileReader.begin() + i);
                continue;
            }
            std::string index;
            char c;
            while((c = tempFileReader[i]->sbumpc()) != '\n')
            {
                index.push_back(c);
            }
            invertedIndexList.push(index);
        }
        if(tempFileReader.empty()) break;
        std::string indexToWrite = getCurrentIndex(invertedIndexList);
        dictFileWriter[indexToWrite[0] - 'a']->sputn(indexToWrite.c_str(), indexToWrite.length());
    }

    while(!invertedIndexList.empty())
    {
        std::string indexToWrite = getCurrentIndex(invertedIndexList);
        dictFileWriter[indexToWrite[0] - 'a']->sputn(indexToWrite.c_str(), indexToWrite.length());
    }
}

std::string FileIO::getCurrentIndex(std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string>> &invertedIndexList)
{
    std::string invertedIndex = invertedIndexList.top();
    invertedIndexList.pop();
    std::string word = invertedIndex.substr(0, invertedIndex.find(':'));
    std::string currentIndex;
    std::string currentWord;
    while(!invertedIndexList.empty())
    {
        currentIndex = invertedIndexList.top();
        currentWord = currentIndex.substr(0, currentIndex.find(':'));
        if(currentWord == word)
        {
            invertedIndex.append(currentIndex.substr(currentIndex.find(':') + 1));
            invertedIndexList.pop();
        }
        else{
            break;
        }
    }
    invertedIndex.append("\n");
    return invertedIndex;
}