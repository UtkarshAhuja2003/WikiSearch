#ifndef WebSearch_H
#define WebSearch_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include "Search.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebSearch : public std::enable_shared_from_this<WebSearch>
{
    private:
        Search searchEngine;

        static std::size_t count = 0;
        
        // The socket for the currently connected client.
        tcp::socket socket_;

        // The buffer for performing reads.
        beast::flat_buffer buffer_{8192};

         // The request message.
        http::request<http::dynamic_body> request_;

        // The response message.
        http::response<http::dynamic_body> response_;

        // The timer for putting a deadline on connection processing.
        net::steady_timer deadline_{
            socket_.get_executor(), std::chrono::days(60)};
    public:
        WebSearch(Search searchEngine);
        void createServer();
        std::size_t request_count();
        std::time_t now();
        void http_server(tcp::acceptor& acceptor, tcp::socket& socket);
        void read_request();
        void check_deadline();

        // Determine what needs to be done with the request message.
        void process_request();

            // Construct a response message based on the program state.
        void create_response();

         // Asynchronously transmit the response message.
        void write_response();
};


#endif