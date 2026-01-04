#include <iostream>
#include <cstring>
#include <cstdlib>

#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>

#include "WebSocketApp.hpp"

using namespace tabxx;
using std::string;

struct Config {
    bool start = true;
    string addr = "localhost";
    string port = "8888";
    string flow = "./flow";
    string log = "";
};

int parseArgs(int argc, char** args, Config& config);

int main(int argc, char** args) {
    Config config;
    int ret = parseArgs(argc, args, config);
    if (ret != 0) {
        return ret; 
    }
    else if (!config.start) {
        return 0;
    }

    try {
        WebSocketApp app(config.addr, config.port, config.flow, config.log);
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize WebSocketApp: " << e.what() << std::endl;
        return 1;
    }
}

const char * hint = 
"Usage: WebCTP [-m <mode>] [-a <address>] [-p <port>] [-f <flow>] [-l <log>] [-h|-v]\n"
"Options:\n"
"  -h, --help       Display this help message and exit\n"
"  -v, --version    Display the version information and exit\n"
"  -a, --addr       Specify the address to listen on (default: localhost)\n"
"  -p, --port       Specify the port to listen on (default: 8888)\n"
"  -f, --flow       Specify the flow directory (default: ./flow)\n"
"  -l, --log        Specify the path log files (default: no file logging)";


int parseArgs(int argc, char** args, Config& config) {
    for (int i = 1; i < argc; i++) {
        string arg = args[i];
        
        if (arg == "-h" || arg == "--help") {
            std::cout << hint << std::endl;
            config.start = false;
            return 0;
        }
        else if (arg == "-v" || arg == "--version") {
            std::cout << "WebCTP version: 0.0.1" << std::endl;
            std::cout << "CTP API version: " << CThostFtdcMdApi::GetApiVersion() << std::endl;
            config.start = false;
            return 0;
        }
        else if (arg == "-a" || arg == "--addr") {
            if (i + 1 < argc) {
                config.addr = args[++i];
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument." << std::endl;
                std::cerr << hint << std::endl;
                return 1;
            }
        }
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                config.port = args[++i];
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument." << std::endl;
                std::cerr << hint << std::endl;
                return 1;
            }
        }
        else if (arg == "-l" || arg == "--log") {
            if (i + 1 < argc) {
                config.log = args[++i];
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument." << std::endl;
                std::cerr << hint << std::endl;
                return 1;
            }
        }
        else {
            std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
            std::cerr << hint << std::endl;
            return 1;
        }
    }
    return 0;
}