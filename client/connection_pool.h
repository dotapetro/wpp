#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include <iostream>

namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
namespace http = boost::beast::http;
using tcp = ip::tcp;


// Parser is an object that works with HTML
// In multithreading context there will be a lot of workers each working on it's own page.
// Often your parsing stuff is the same, so it need to be further encapsulated to the command pattern
// Command can also be implemented using the composite design pattern div.car -> span.price-tag -> a.price[value]
// I think that spawning all the threads at the same time
// So that you spawn a whole bunch of parsers and giving them a command to work with, assigning the URLs (typically pages like ?page=0, ?page=1, ect...)
// It will be also cool if you use an abstract factory for creating them

class Pool {
public:
    static std::string retrieve_single(const std::string& hostname, const std::string& port, const std::string& target){

        std::cout << "Setting up environment\n";

        boost::asio::io_context ioc;
        ssl::context ssl_context(ssl::context::method::sslv23_client);
        tcp::resolver resolver{ioc};
        ssl::stream<ip::tcp::socket> ssock{ioc, ssl_context};

        std::cout << "Creating socket and performing handshake\n";

        auto const results = resolver.resolve(hostname, port);
        boost::asio::connect(ssock.lowest_layer(), results);
        ssock.handshake(ssl::stream_base::handshake_type::client);


        std::cout << "Creating request\n";

        http::request<http::string_body> req{http::verb::get, target, 11}; // 11 = HTTP 1.1
        req.set(http::field::host, hostname);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        std::cout << "Sending request\n";

        http::write(ssock, req);

        std::cout << "Receiving response\n";

        boost::beast::flat_buffer buff;
        http::response<http::dynamic_body> res;
        http::read(ssock, buff, res);

        ssock.lowest_layer().shutdown(tcp::socket::shutdown_both);
        return  boost::beast::buffers_to_string(res.body().data());
    }
};


