#include <iostream>
#include <chrono>
#include <fstream>
#include "index/ParseIndex.h"

int main()
{
    try
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::string wikiDump = "../../../wiki/temp.xml";
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
    
    return 0;
}
