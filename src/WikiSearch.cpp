#include <iostream>
#include <chrono>
#include <fstream>
#include "index/ParseIndex.h"

int main()
{
    try
    {
        auto start = std::chrono::high_resolution_clock::now();

        std::string wikiDump = "../../../wiki/data.xml";
        ParseIndex parseIndex(wikiDump);
        parseIndex.buildIndex();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "Success: Index built in " << duration << " seconds." << std::endl;

        std::fstream data("../res/temp.txt");
        if(!data.is_open()){
            throw std::runtime_error("Temp not open");
        }
        std::map<std::string, std::set<std::string>> ind = parseIndex.getWikiIndexes();
        for(auto m:ind)
        {
            data << m.first << ":";
            for(auto s:m.second)
            {
                data << " "<< s;
            }
            data << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error building Index : " << e.what() << '\n';
        return 1;
    }
    
    return 0;
}
