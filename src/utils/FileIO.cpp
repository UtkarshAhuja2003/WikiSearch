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

void FileIO::writeDataToTemporaryFile(std::string &data)
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

void FileIO::openFile(int tempFileNumber)
{
    try
    {
        std::string filePath = indexFolderPath + "/temp/index" + std::to_string(tempFileNumber) + ".txt";
        FileBuffer->open(filePath, std::ios::out | std::ios::app);
        if(!FileBuffer->is_open())
        {
            std::cout << "Unable to open TempFile";
        }
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

void FileIO::writeL1Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/l1Metadata" + ".txt";
        std::ofstream l1MetadataFile(filePath, std::ios::out | std::ios::app);
        if (!l1MetadataFile.is_open()) {
            throw std::runtime_error("Unable to open L1 metadata file.");
        }
        std::string data = docIdTitle.begin()->first + ":" + writeL2Metadata(docIdTitle) + "\n";
        l1MetadataFile << data;
        l1MetadataFile.close();
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write l1 metadata\n";
        throw std::runtime_error(e.what());
    }
}

std::string FileIO::writeL2Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/l2Metadata" + ".txt";
        std::ofstream l2MetadataFile(filePath, std::ios::out | std::ios::app);
        if (!l2MetadataFile.is_open()) {
            throw std::runtime_error("Unable to open L2 metadata file.");
        }
        l2MetadataFile.seekp(0, std::ios::end);
        int offset = l2MetadataFile.tellp();
        std::string dataToWrite = "";

        auto it = docIdTitle.begin();
        for (int i = 0; i < L2MetadataLimit; i++)
        {
            if(it == docIdTitle.end()) break;
            std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> offsetIteratorPair = writeL3Metadata(docIdTitle, it);
            dataToWrite.append(it->first);
            dataToWrite.append(":");
            dataToWrite.append(offsetIteratorPair.first);
            dataToWrite.append("\n");
            it = offsetIteratorPair.second;
        }
        l2MetadataFile << dataToWrite;
        dataToWrite.clear();
        l2MetadataFile.close();
        return std::to_string(offset);
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write l2 metadata\n";
        throw std::runtime_error(e.what());
    }
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> FileIO::writeL3Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/l3Metadata" + ".txt";
        std::ofstream l3MetadataFile(filePath, std::ios::out | std::ios::app);
        if (!l3MetadataFile.is_open()) {
            throw std::runtime_error("Unable to open L3 metadata file.");
        }
        l3MetadataFile.seekp(0, std::ios::end);
        int offset = l3MetadataFile.tellp();
        std::string dataToWrite = "";

        for (int i = 0; i < L3MetadataLimit; i++)
        {
            if(it == docIdTitle.end()) break;
            std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> offsetIteratorPair = writeDocIdTitle(docIdTitle, it);
            dataToWrite.append(it->first);
            dataToWrite.append(":");
            dataToWrite.append(offsetIteratorPair.first);
            dataToWrite.append("\n");
            it = offsetIteratorPair.second;
        }
        l3MetadataFile << dataToWrite;
        dataToWrite.clear();
        l3MetadataFile.close();
        return {std::to_string(offset), it};
    }
    catch(const std::exception& e)
    {
        std::cout << "Unable to write l3 metadata\n";
        throw std::runtime_error(e.what());
    }
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> FileIO::writeDocIdTitle(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it)
{
    try
    {
        std::string filePath = indexFolderPath + "/meta/id_title_map" + ".txt";
        std::ofstream metadataFile(filePath, std::ios::out | std::ios::app);
        if (!metadataFile.is_open()) {
            throw std::runtime_error("Unable to open DocId Title metadata file.");
        }
        metadataFile.seekp(0, std::ios::end);
        int offset = metadataFile.tellp();
        std::string dataToWrite = "";

        for (int i = 0; i < MetadataLimit; i++)
        {
            if(it == docIdTitle.end()) break;
            dataToWrite.append(it->first);
            dataToWrite.append(":");
            dataToWrite.append(it->second);
            dataToWrite.append("\n");
            it++;
        }
        metadataFile << dataToWrite;
        dataToWrite.clear();
        metadataFile.close();
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
    try
    {
        std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, std::greater<std::pair<std::string, int>>> invertedIndexList;
        std::vector<std::ifstream> tempFileStreams(tempFileCount);
        std::string tempFilePath = "C:/Users/hp/res/temp/index";

        initialiseDictFiles(std::ios::out | std::ios::app);
        initialisePostingLists(std::ios::out | std::ios::app);

        for (int i = 0; i < tempFileCount; i++)
        {
            std::string path = tempFilePath + std::to_string(i) + ".txt";
            tempFileStreams[i].open(path);
            if (!tempFileStreams[i].is_open())
            {
                std::cerr << "Unable to open temp file: " << path << "\n";
                return;
            }
        }

        for (int i = 0; i < tempFileCount; i++)
        {
            std::string index;
            std::getline(tempFileStreams[i], index);
            if (!index.empty()) {
                invertedIndexList.push({index, i});
            }
        }

        while (!invertedIndexList.empty())
        {
            std::pair<std::pair<std::string, std::string>, std::vector<int>> dict = getPostingList(invertedIndexList);
            std::string word = dict.first.first;
            std::string postingList = dict.first.second;
            std::string invertedIndex = word + ":" + std::to_string(postingListsStreams[word[0] - 'a'].tellp()) + "\n";
            dictBuffer[word[0] - 'a']->sputn(invertedIndex.c_str(), invertedIndex.length());
            postingListsBuffer[word[0] - 'a']->sputn(postingList.c_str(), postingList.length());

            std::vector<int> files = dict.second;
            for(int i : files)
            {
                if (tempFileStreams[i].peek() != EOF)
                {
                    std::string index;
                    std::getline(tempFileStreams[i], index);
                    if (!index.empty())
                    {
                        invertedIndexList.push({index, i});
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception in mergeTemporaryFiles: " << e.what() << '\n';
    } 
}

std::pair<std::pair<std::string, std::string>, std::vector<int>> FileIO::getPostingList(std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, std::greater<std::pair<std::string, int>>> &invertedIndexList)
{
    try
    {
        std::pair<std::string, int> invertedIndex = invertedIndexList.top();
        invertedIndexList.pop();
        std::string word = invertedIndex.first.substr(0, invertedIndex.first.find(':'));
        std::string postingList = invertedIndex.first.substr(invertedIndex.first.find(':') + 1);
        std::vector<int> files;
        files.push_back(invertedIndex.second);

        while (!invertedIndexList.empty())
        {
            std::pair<std::string, int> currentIndex = invertedIndexList.top();
            std::string currentWord = currentIndex.first.substr(0, currentIndex.first.find(':'));
            if (currentWord == word)
            {
                postingList.append(currentIndex.first.substr(currentIndex.first.find(':') + 1));
                files.push_back(currentIndex.second);
                invertedIndexList.pop();
            }
            else
            {
                break;
            }
        }
        postingList.append("\n");
        return {{word, postingList}, files};
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error getting posting list: " << e.what() << '\n';
        throw std::runtime_error("Error getting posting list");
    }
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
