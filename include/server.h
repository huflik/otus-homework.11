#pragma once

#include <memory>
#include <boost/asio.hpp>

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(boost::asio::io_context& io_context, unsigned short port);
    ~Server() = default;
    
    void Start();

private:
    void Accept();
    
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_context& io_context_;
};