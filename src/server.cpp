#include <iostream>
#include "server.h"
#include "session.h"


Server::Server(boost::asio::io_context& io_context, unsigned short port) : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), io_context_(io_context){}

void Server::Start()
{
    Accept();
}

void Server::Accept()
{
    auto self = shared_from_this();
    
    acceptor_.async_accept(
        [this, self](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket));
                session->Start();
            } else if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }
            Accept();
        });
}