#include <iostream>
#include <chrono>
#include <fstream>
#include "index/ParseIndex.h"
#include "search/Search.h"
#include "search/WebSearch.h"

#define BuildIndex 0
#define SearchWeb 1

int main()
{
    if(BuildIndex)
    {
        try
        {
            auto start = std::chrono::high_resolution_clock::now();
        
            std::string wikiDump = "../res/wiki.xml";
            ParseIndex parseIndex(wikiDump);
            std::cout << "Starting Indexing\n";
            parseIndex.buildIndex();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
            std::cout << "Success: Index built in " << duration << " seconds.\n" << std::endl;

            std::cout << "Starting merging\n";
            FileIO file("../res");
            file.mergeTemporaryFiles(parseIndex.getNumberOfTempFiles() + 1);
            std::cout << "Merged Temporary Indexes\n";
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error building Index : " << e.what() << '\n';
            return 1;
        }
        
    }

    Search searchEngine;
    try
    {
        searchEngine.loadInvertedIndex();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error loading Search Engine:" << e.what() << '\n';
    }

    if(SearchWeb)
    {
        WebSearch webSearch(searchEngine);
        webSearch.createServer();
    }
    else
    {
        searchEngine.search();
    }
    
    return 0;
}
