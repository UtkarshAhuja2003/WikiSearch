#include <iostream>
#include <chrono>
#include <fstream>
#include "index/ParseIndex.h"
#include "search/Search.h"
#include "search/WebSearch.h"

constexpr bool BUILD_INDEX = false;
constexpr bool SEARCH_WEB = true;
constexpr int FILE_LIMIT = 5000;

Search searchEngine;


/**
 * @brief Starts an HTTP server that accepts incoming connections and handles them asynchronously.
 * 
 * @param acceptor TCP acceptor object to accept incoming connections.
 * @param socket TCP socket object to handle the accepted connections.
 * @note Accepts the connections forever
 */
void http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
      [&](beast::error_code ec)
      {
          if(!ec)
            std::make_shared<WebSearch>(std::move(socket))->start(searchEngine);
          http_server(acceptor, socket);
      });
}

int main()
{
    if(BUILD_INDEX)
    {
        // Increase the file descriptor limit to allow opening temporary index files.
        int setFileLimitResult = _setmaxstdio(FILE_LIMIT);
        if (setFileLimitResult == -1) {
            std::cerr << "Error setting max stdio limit: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        try
        {
            auto start = std::chrono::high_resolution_clock::now();
        
            std::string wikiDump = "../res/wiki_dump.xml";
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

    FileIO file("../res");
    try
    {
        searchEngine.loadInvertedIndex(file);
        searchEngine.loadL1Metadata(file);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error Loading Search Engine:" << e.what() << '\n';
    }

    if(SEARCH_WEB)
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
        try
        {
            searchEngine.search();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error Search Engine:" << e.what() << '\n';
        }
    }

    return 0;
}
