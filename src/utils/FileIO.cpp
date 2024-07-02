#include "FileIO.h"

FileIO::FileIO(std::string resourcesFolderPath)
{
    this->resourcesFolderPath = resourcesFolderPath;
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

        return {{FileBuffer, {MetadataFileBuffer, l1MetadataFileBuffer, l2MetadataFileBuffer, l3MetadataFileBuffer}}, resourcesFolderPath};
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
        std::string filePath = resourcesFolderPath + "/temp/index" + std::to_string(tempFileNumber) + ".txt";
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
        std::string filePath = resourcesFolderPath + "/meta/l1Metadata" + ".txt";
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
        std::cerr << "Error in writing L1 Metadata: " << e.what() << std::endl;
        throw;
    }
}

std::string FileIO::writeL2Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle)
{
    try
    {
        std::string filePath = resourcesFolderPath + "/meta/l2Metadata" + ".txt";
        std::ofstream l2MetadataFile(filePath, std::ios::out | std::ios::app);
        if (!l2MetadataFile.is_open()) {
            throw std::runtime_error("Unable to open L2 metadata file.");
        }
        l2MetadataFile.seekp(0, std::ios::end);
        int offset = l2MetadataFile.tellp();
        std::string dataToWrite = "";

        auto it = docIdTitle.begin();
        for (int i = 0; i < L2_METADATA_LIMIT; i++)
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
        std::cerr << "Error in writing L2 Metadata: " << e.what() << std::endl;
        throw;
    }
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> FileIO::writeL3Metadata(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it)
{
    try
    {
        std::string filePath = resourcesFolderPath + "/meta/l3Metadata" + ".txt";
        std::ofstream l3MetadataFile(filePath, std::ios::out | std::ios::app);
        if (!l3MetadataFile.is_open()) {
            throw std::runtime_error("Unable to open L3 metadata file.");
        }
        l3MetadataFile.seekp(0, std::ios::end);
        int offset = l3MetadataFile.tellp();
        std::string dataToWrite = "";

        for (int i = 0; i < L3_METADATA_LIMIT; i++)
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
        std::cerr << "Error in writing L3 Metadata: " << e.what() << std::endl;
        throw;
    }
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>::iterator> FileIO::writeDocIdTitle(std::vector<std::pair<std::string, std::string>> &docIdTitle, auto it)
{
    try
    {
        std::string filePath = resourcesFolderPath + "/meta/id_title_map" + ".txt";
        std::ofstream metadataFile(filePath, std::ios::out | std::ios::app);
        if (!metadataFile.is_open()) {
            throw std::runtime_error("Unable to open DocId Title metadata file.");
        }
        metadataFile.seekp(0, std::ios::end);
        int offset = metadataFile.tellp();
        std::string dataToWrite = "";

        for (int i = 0; i < METADATA_LIMIT; i++)
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
        std::string tempFilePath = resourcesFolderPath + "/temp/index";

        initialiseDictFiles(std::ios::out | std::ios::app);
        initialisePostingLists(std::ios::out | std::ios::app);

        // Open all the tempFiles.
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

        // Add the first invertedIndex from all files
        for (int i = 0; i < tempFileCount; i++)
        {
            std::string index;
            std::getline(tempFileStreams[i], index);
            if (!index.empty()) {
                invertedIndexList.push({index, i});
            }
        }

        std::string word, postingList, invertedIndex;
        int index, offset;
        std::vector<int> fileIndexes; // files indexes from which inverted indexes are removed
        while (!invertedIndexList.empty())
        {
            std::pair<std::pair<std::string, std::string>, std::vector<int>> dict = getPostingList(invertedIndexList);
            word = dict.first.first;
            postingList = dict.first.second;
            index = word[0] - 'a';

            // Take the offset of posting list and add to inverted index
            postingListsStreams[index]->seekp(0, std::ios::end);
            offset = postingListsStreams[index]->tellp();

            // Inverted Index - word:offset
            invertedIndex = word + ":" + std::to_string(offset) + "\n";

            *dictStreams[index] << invertedIndex;
            *postingListsStreams[index] << postingList;

            // Get the next inverted index from all the files where index was removed
            std::vector<int> fileIndexes = dict.second;
            for(int i : fileIndexes)
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
        std::vector<int> fileIndexes;// files indexes from which inverted indexes are removed
        fileIndexes.push_back(invertedIndex.second);

        // Add all posting list from the queue containing same word
        while (!invertedIndexList.empty())
        {
            std::pair<std::string, int> currentIndex = invertedIndexList.top();
            std::string currentWord = currentIndex.first.substr(0, currentIndex.first.find(':'));
            if (currentWord == word)
            {
                postingList.append(currentIndex.first.substr(currentIndex.first.find(':') + 1));
                fileIndexes.push_back(currentIndex.second);
                invertedIndexList.pop();
            }
            else
            {
                break;
            }
        }
        postingList.append("\n");
        return {{word, postingList}, fileIndexes};
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
    postingListsStreams.resize(FILES_COUNT);
    std::string postingListsPath = resourcesFolderPath + "/posting_lists/";
    for(int i = 0; i < FILES_COUNT; i++)
    {
        postingListsStreams[i] = new std::fstream((postingListsPath + postingLists[i]), openMode);
        if(!postingListsStreams[i]->is_open())
        {
            if(!dictStreams[i]->is_open())
            {
                std::cerr << "Error: Opening file " << postingLists[i];
            }
        }
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
    std::string dictFilesPath = resourcesFolderPath + "/dict/";
    dictStreams.resize(FILES_COUNT);
    for(int i = 0; i < FILES_COUNT; i++)
    {
        dictStreams[i] = new std::fstream((dictFilesPath + dictFiles[i]), openMode);
        if(!dictStreams[i]->is_open())
        {
            std::cerr << "Error: Opening file " << dictFiles[i];
        }
    }
}

std::vector<std::fstream*> FileIO::getDictStreams()
{
    return dictStreams;
}

std::vector<std::fstream*> FileIO::getPostingListStreams()
{
    return postingListsStreams;
}

std::filebuf * FileIO::getMetadataBuffer()
{
    return MetadataFileBuffer;
}
