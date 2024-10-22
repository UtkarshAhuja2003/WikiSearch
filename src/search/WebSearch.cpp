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
                self->process_request();
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
            response_.set(http::field::access_control_allow_origin, "*");
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
    std::string requestTarget = request_.target();
    if(requestTarget.starts_with("/search"))
    {
        std::string word = requestTarget.substr(requestTarget.find("=") + 1);
        std::vector<std::pair<std::string, std::string>> topKDocs = searchEngine.search(word);
        std::stringstream jsonStream;
        response_.set( http::field::content_type, "application/json" );
        response_.prepare_payload();
        if (topKDocs.empty()) {
            jsonStream << "[]";
        } else {
            jsonStream << "{ \"results\": [";
            bool isFirst = true;
            for (const auto& doc : topKDocs) {
                if (!isFirst) {
                    jsonStream << ",";
                }
                jsonStream << "{ \"docid\": \"" << doc.first << "\", \"title\": \"" << doc.second << "\" }";
                isFirst = false;
            }
            jsonStream << "] }";
        }
        beast::ostream(response_.body())
            << jsonStream.str();
    }
    else if(requestTarget == "/count")
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
    else if(requestTarget == "/time")
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
        beast::ostream(response_.body()) << "Route not found\r\n";
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
