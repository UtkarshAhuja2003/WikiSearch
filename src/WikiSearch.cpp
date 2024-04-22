#include <iostream>
#include <chrono>
#include <fstream>
#include "index/ParseIndex.h"
#include "search/Search.h"
#include "search/WebSearch.h"

#define BuildIndex 0
#define SearchWeb 1

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
      [&](beast::error_code ec)
      {
          if(!ec)
            std::make_shared<WebSearch>(std::move(socket))->start();
          http_server(acceptor, socket);
      });
}

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

    if(SearchWeb)
    {
        try
        {
            auto const address = net::ip::make_address("127.0.0.1");
            unsigned short port = static_cast<unsigned short>(8080);

            net::io_context ioc{1};

            tcp::acceptor acceptor{ioc, {address, port}};
            tcp::socket socket{ioc};
            http_server(acceptor, socket);

            ioc.run();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error starting the server: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
    else
    {
        Search searchEngine;
        try
        {
            searchEngine.loadInvertedIndex();
            searchEngine.search();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error loading Search Engine:" << e.what() << '\n';
        }
    }
    
    return 0;
}
