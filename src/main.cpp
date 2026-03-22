#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <boost/asio.hpp>
#include "server.h"
#include "dbmanager.h"

std::shared_ptr<boost::asio::io_context> io_context;
std::shared_ptr<Server> server;

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        if (io_context) {
            io_context->stop();
        }
    }
}

int main(int argc, char* argv[])
{
    try {
        if (argc != 2) {
            std::cerr << "Usage: join_server <port>" << std::endl;
            return 1;
        }
        
        unsigned short port = static_cast<unsigned short>(std::stoi(argv[1]));
        
        signal(SIGPIPE, SIG_IGN);
        
        DbManager& dbManager = DbManager::GetInstance();
        Result result = dbManager.Initialize();
        if (result.IsError()) {
            std::cerr << "Failed to initialize database: " 
                      << result.toString() << std::endl;
            return 1;
        }
        
        io_context = std::make_shared<boost::asio::io_context>();
        server = std::make_shared<Server>(*io_context, port);
        server->Start();
        
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        io_context->run();
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}