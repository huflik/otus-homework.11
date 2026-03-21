#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <boost/asio.hpp>
#include "server.h"
#include "dbmanager.h"

std::shared_ptr<boost::asio::io_context> g_io_context;
std::shared_ptr<Server> g_server;

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        if (g_io_context) {
            g_io_context->stop();
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
        if (result.isError()) {
            std::cerr << "Failed to initialize database: " 
                      << result.toString() << std::endl;
            return 1;
        }
        
        g_io_context = std::make_shared<boost::asio::io_context>();
        g_server = std::make_shared<Server>(*g_io_context, port);
        g_server->Start();
        
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        g_io_context->run();
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}