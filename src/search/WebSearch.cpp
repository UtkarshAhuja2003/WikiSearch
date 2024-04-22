#include "WebSearch.h"


void WebSearch::read_request()
{
    auto self = shared_from_this();

    http::async_read(
        socket_,
        buffer_,
        request_,
        [self](beast::error_code ec,
            std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);
            if(!ec)
                process_request();
        });
}

void WebSearch::process_request()
{
    response_.version(request_.version());
    response_.keep_alive(false);

    switch(request_.method())
    {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "WikiSearch");
            create_response();
            break;

        default:
            // We return responses indicating an error if
            // we do not recognize the request method.
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body())
            << "Invalid request-method '"
            << std::string(request_.method_string())
            << "'";
            break;
    }

    write_response();
}

void WebSearch::create_response()
{
    if(request_.target() == "/search")
    {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
            << "<html>\n"
            <<  "<head><title>Search Results</title></head>\n"
            <<  "<body>\n"
            <<  "<h1>Search Result</h1>\n"
            <<  request_count()
            <<  "</body>\n"
            <<  "</html>\n";
    }
    else if(request_.target() == "/count")
    {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
            << "<html>\n"
            <<  "<head><title>Request count</title></head>\n"
            <<  "<body>\n"
            <<  "<h1>Request count</h1>\n"
            <<  "<p>There have been "
            <<  request_count()
            <<  " requests so far.</p>\n"
            <<  "</body>\n"
            <<  "</html>\n";
    }
    else if(request_.target() == "/time")
    {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
            <<  "<html>\n"
            <<  "<head><title>Current time</title></head>\n"
            <<  "<body>\n"
            <<  "<h1>Current time</h1>\n"
            <<  "<p>The current time is "
            <<  now()
            <<  " seconds since the epoch.</p>\n"
            <<  "</body>\n"
            <<  "</html>\n";
    }
    else
    {
        response_.result(http::status::not_found);
        response_.set(http::field::content_type, "text/plain");
        beast::ostream(response_.body()) << "File not found\r\n";
    }
}

void WebSearch::write_response()
{
    auto self = shared_from_this();

    response_.content_length(response_.body().size());

    http::async_write(
        socket_,
        response_,
        [self](beast::error_code ec, std::size_t)
        {
            self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            self->deadline_.cancel();
        });
}

// Check whether we have spent enough time on this connection.
void WebSearch::check_deadline()
{
    auto self = shared_from_this();
    deadline_.async_wait(
        [self](beast::error_code ec)
        {
            if(!ec)
            {
                // Close socket to cancel any outstanding operation.
                self->socket_.close(ec);
            }
        });
}

// "Loop" forever accepting new connections.
void WebSearch::http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
      [&](beast::error_code ec)
      {
          if(!ec)
          {
            this->socket_ = socket;
            read_request();
            check_deadline();
          }
          http_server(acceptor, socket);
      });
}

void WebSearch::createServer()
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

WebSearch::WebSearch(Search searchEngine)
{
    this->searchEngine = searchEngine;
}
