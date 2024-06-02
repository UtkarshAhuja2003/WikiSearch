#include "FileIO.h"

FileIO::FileIO(std::string indexFolderPath)
{
    this->indexFolderPath = indexFolderPath;
}

std::pair<std::pair<std::filebuf *, std::vector<std::filebuf *>>, std::string> FileIO::initialise()
{
    try
    {
        FileBuffer = stream.rdbuf();
        MetadataFileBuffer = metadataStream.rdbuf();
        l1MetadataFileBuffer = l1MetadataStream.rdbuf();
        l2MetadataFileBuffer = l2MetadataStream.rdbuf();
        l3MetadataFileBuffer = l3MetadataStream.rdbuf();

        return {{FileBuffer, {MetadataFileBuffer, l1MetadataFileBuffer, l2MetadataFileBuffer, l3MetadataFileBuffer}}, indexFolderPath};
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
        MetadataFileBuffer->close();
        l1MetadataFileBuffer->close();
        l2MetadataFileBuffer->close();
        l3MetadataFileBuffer->close();
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

void FileIO::writeL1Metadata(std::map<int, std::string> &docIdTitleMap)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/l1Metadata" + ".txt";
        l1MetadataFileBuffer->open(filePath, std::ios::out | std::ios::app);
        std::string data = std::to_string(docIdTitleMap.begin()->first) + ":" + writeL2Metadata(docIdTitleMap) + "\n";
        l1MetadataFileBuffer->sputn(data.c_str(), data.length());
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write l1 metadata\n";
        throw std::runtime_error(e.what());
    }
}

std::string FileIO::writeL2Metadata(std::map<int, std::string> &docIdTitleMap)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/l2Metadata" + ".txt";
        l2MetadataFileBuffer->open(filePath, std::ios::out | std::ios::app);
        int offset = l2MetadataStream.tellp();

        auto it = docIdTitleMap.begin();
        for (int i = 0; i < 1000; i++)
        {
            if(it == docIdTitleMap.end())break;
            std::pair<std::string, std::map<int, std::string>::iterator> offsetIteratorPair = writeL3Metadata(docIdTitleMap, it);
            std::string data = std::to_string(it->first) + ":" + offsetIteratorPair.first + "\n";
            l2MetadataFileBuffer->sputn(data.c_str(), data.length());
            it = offsetIteratorPair.second;
        }
        return std::to_string(offset);
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write l2 metadata\n";
        throw std::runtime_error(e.what());
    }
}

std::pair<std::string, std::map<int, std::string>::iterator> FileIO::writeL3Metadata(std::map<int, std::string> &docIdTitleMap, auto it)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/l3Metadata" + ".txt";
        l3MetadataFileBuffer->open(filePath, std::ios::out | std::ios::app);
        int offset = l3MetadataStream.tellp();

        for (int i = 0; i < 50; i++)
        {
            if(it == docIdTitleMap.end())break;
            std::pair<std::string, std::map<int, std::string>::iterator> offsetIteratorPair = writeDocIdTitle(docIdTitleMap, it);
            std::string data = std::to_string(it->first) + ":" + offsetIteratorPair.first + "\n";
            l3MetadataFileBuffer->sputn(data.c_str(), data.length());
            it = offsetIteratorPair.second;
        }
        return {std::to_string(offset), it};
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write l3 metadata\n";
        throw std::runtime_error(e.what());
    }
}

std::pair<std::string, std::map<int, std::string>::iterator> FileIO::writeDocIdTitle(std::map<int, std::string> &docIdTitleMap, auto it)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/id_title_map" + ".txt";
        MetadataFileBuffer->open(filePath, std::ios::out | std::ios::app);
        int offset = metadataStream.tellp();

        for (int i = 0; i < 10; i++)
        {
            if(it == docIdTitleMap.end())break;
            std::string data = std::to_string(it->first) + ":" + it->second + "\n";
            MetadataFileBuffer->sputn(data.c_str(), data.length());
            it++;
        }
        return {std::to_string(offset), it};
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write DocId Title metadata\n";
        throw std::runtime_error(e.what());
    }
}

void FileIO::dumpMetadataToDisk()
{
    try
    {
        MetadataFileBuffer->close();
        l1MetadataFileBuffer->close();
        l2MetadataFileBuffer->close();
        l3MetadataFileBuffer->close();
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(std::string("Error in Dumping Metadata file to Disk : ") + e.what());
    }
}

void FileIO::mergeTemporaryFiles(int tempFileCount)
{
    std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string>> invertedIndexList;
    std::vector<std::fstream> tempFileStreams(tempFileCount);
    std::vector<std::filebuf *> tempFileReader(tempFileCount);
    std::string tempFilePath = indexFolderPath + "/temp/index0.txt";

    initialiseDictFiles(std::ios::out | std::ios::app);
    initialisePostingLists(std::ios::out | std::ios::app);
    
    for(int i = 0; i < tempFileCount; i++)
    {
        tempFileReader[i] = tempFileStreams[i].rdbuf();
        tempFilePath[tempFilePath.length() - 5] = '0' + i;
        tempFileReader[i]->open(tempFilePath, std::ios::in);
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
        std::pair<std::string, std::string> dict = getPostingList(invertedIndexList);
        std::string word = dict.first;
        std::string postingList = dict.second;
        std::string invertedIndex = word + ":" + std::to_string(postingListsStreams[word[0] - 'a'].tellp()) + "\n";
        dictBuffer[word[0] - 'a']->sputn(invertedIndex.c_str(), invertedIndex.length());
        postingListsBuffer[word[0] - 'a']->sputn(postingList.c_str(), postingList.length());
    }

    while(!invertedIndexList.empty())
    {
        std::pair<std::string, std::string> dict = getPostingList(invertedIndexList);
        std::string word = dict.first;
        std::string postingList = dict.second;
        std::string invertedIndex = word + ":" + std::to_string(postingListsStreams[word[0] - 'a'].tellp()) + "\n";
        dictBuffer[word[0] - 'a']->sputn(invertedIndex.c_str(), invertedIndex.length());
        postingListsBuffer[word[0] - 'a']->sputn(postingList.c_str(), postingList.length());
    }
}

std::pair<std::string, std::string> FileIO::getPostingList(std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string>> &invertedIndexList)
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
    return {word, invertedIndex.substr(invertedIndex.find(':') + 1)};
}

void FileIO::initialisePostingLists(std::_Ios_Openmode openMode)
{
    std::vector<std::string> postingLists = {
        "indexa.txt", "indexb.txt", "indexc.txt", "indexd.txt", "indexe.txt",
        "indexf.txt", "indexg.txt", "indexh.txt", "indexi.txt", "indexj.txt",
        "indexk.txt", "indexl.txt", "indexm.txt", "indexn.txt", "indexo.txt",
        "indexp.txt", "indexq.txt", "indexr.txt", "indexs.txt", "indext.txt",
        "indexu.txt", "indexv.txt", "indexw.txt", "indexx.txt", "indexy.txt",
        "indexz.txt"
    };
    postingListsStreams.resize(FilesCount);
    postingListsBuffer.resize(FilesCount);
    std::string postingListsPath = indexFolderPath + "/posting_lists/";
    for(int i = 0; i < FilesCount; i++)
    {
        postingListsBuffer[i] = postingListsStreams[i].rdbuf();
        postingListsBuffer[i]->open((postingListsPath + postingLists[i]), openMode);
    }
}

void FileIO::initialiseDictFiles(std::_Ios_Openmode openMode)
{
    std::vector<std::string> dictFiles = {
        "dindexa.txt","dindexb.txt","dindexc.txt","dindexd.txt","dindexe.txt",
        "dindexf.txt","dindexg.txt","dindexh.txt","dindexi.txt","dindexj.txt",
        "dindexk.txt","dindexl.txt","dindexm.txt","dindexn.txt","dindexo.txt",
        "dindexp.txt","dindexq.txt","dindexr.txt","dindexs.txt","dindext.txt",
        "dindexu.txt","dindexv.txt","dindexw.txt","dindexx.txt","dindexy.txt",
        "dindexz.txt"
    };
    std::string dictFilesPath = indexFolderPath + "/dict/";
    dictStreams.resize(FilesCount);
    dictBuffer.resize(FilesCount);
    for(int i = 0; i < FilesCount; i++)
    {
        dictBuffer[i] = dictStreams[i].rdbuf();
        dictBuffer[i]->open((dictFilesPath + dictFiles[i]), openMode);
    }
}

std::vector<std::filebuf *> FileIO::getDictBuffer()
{
    return dictBuffer;
}

std::vector<std::filebuf *> FileIO::getPostingListBuffer()
{
    return postingListsBuffer;
}

std::filebuf * FileIO::getMetadataBuffer()
{
    return MetadataFileBuffer;
}
