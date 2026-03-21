#pragma once

#include <memory>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "parser.h"
#include "dbmanager.h"
#include "result.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::ip::tcp::socket socket);
    ~Session();
    
    void Start();

private:
    void Read();
    void Write(const std::string& response);
    void HandleCommand(const std::string& cmd);
    
    boost::asio::ip::tcp::socket socket_;
    char buffer_data_[1024];
    std::string command_buffer_;
    Parser parser_;
    bool waitingForResponse_;
};