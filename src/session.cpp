#include <iostream>
#include "session.h"

Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)), waitingForResponse_(false) {}

Session::~Session()
{
    boost::system::error_code ec;
    socket_.close(ec);
}

void Session::Start()
{
    Read();
}

void Session::Read()
{
    auto self = shared_from_this();
    
    socket_.async_read_some(boost::asio::buffer(buffer_data_, sizeof(buffer_data_)),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (ec) {
                if (ec == boost::asio::error::eof) {
                } else if (ec != boost::asio::error::operation_aborted) {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
                return;
            }
            
            if (length > 0) {
                std::string data(buffer_data_, length);
                command_buffer_ += data;
                
                size_t pos;
                while ((pos = command_buffer_.find('\n')) != std::string::npos) {
                    std::string command = command_buffer_.substr(0, pos);
                    command_buffer_.erase(0, pos + 1);
                                      
                    if (!command.empty()) {
                        HandleCommand(command);
                    }
                }
            }
            
            if (!waitingForResponse_) {
                Read();
            }
        });
}

void Session::Write(const std::string& response)
{
    auto self = shared_from_this();
    auto response_copy = std::make_shared<std::string>(response);
    
    boost::asio::async_write(socket_, boost::asio::buffer(*response_copy),
        [this, self, response_copy](boost::system::error_code ec, std::size_t) {
            waitingForResponse_ = false;
            if (!ec) {     
                Read();
            } else if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "Write error: " << ec.message() << std::endl;
            }
        });
}

void Session::HandleCommand(const std::string& cmd)
{
    ParseResult parseResult = parser_.Parse(cmd);

    if (parseResult.IsError()) {
        Write(parseResult.result.toString() + "\n");
        return;
    }
    
    waitingForResponse_ = true;
    
    ExecutionResult execResult = DbManager::GetInstance().Execute(parseResult.command, parseResult.tokens);
    
    if (execResult.result.IsError()) {
        Write(execResult.result.toString() + "\n");
    } else if (execResult.result.IsLoad()) {
        Write(execResult.data + "< OK\n");
    } else {
        Write("< OK\n");
    }
}