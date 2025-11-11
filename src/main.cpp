#include <iostream>
#include <cstring>
#include <cstdlib>

#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>

#include "WebSocketApp.hpp"
#include "SubProcess/SubProcess.hpp"

using namespace tabxx;
using std::string;

struct execConfig {
    enum {
        WS, SUBPROCESS
    } mode = WS;
    string addr = "localhost";
    string port = "8888";
    string flow = "./flow";
};

int parseArgs(int argc, char** args, execConfig& config);

int main(int argc, char** args) {
    execConfig config;
    int ret = parseArgs(argc, args, config);
    if (ret != 0) {
        return ret;
    }

    switch (config.mode) {
    case execConfig::WS: try {
        WebSocketApp app(config.addr, config.port, config.flow);
        app.run();
        break;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize WebSocketApp: " << e.what() << std::endl;
        return 1;
    }
    case execConfig::SUBPROCESS: {
        tabxx::SubProcess subProcess(config.flow);
        subProcess.run();
        break;
    }
    }
}

const char * hint = 
"Usage: WebCTP [-m <mode>] [-a <address>] [-p <port>] [-f <flow>] [-h|-v]\n"
"Options:\n"
"  -h, --help       Display this help message and exit\n"
"  -v, --version    Display the version information and exit\n"
"  -a, --addr       Specify the address to listen on (default: localhost)\n"
"  -p, --port       Specify the port to listen on (default: 8888)\n"
"  -m, --mode       Specify the mode to run in (default: ws)\n"
"                   Supported modes: ws, subprocess"
"  -f, --flow       Specify the flow directory (default: ./flow)";


int parseArgs(int argc, char** args, execConfig& config) {
    for (int i = 1; i < argc; i++) {
        string arg = args[i];
        
        if (arg == "-h" || arg == "--help") {
            std::cout << hint << std::endl;
            return 0;
        }
        else if (arg == "-v" || arg == "--version") {
            std::cout << "WebCTP version: 0.0.1" << std::endl;
            std::cout << "CTP API version: " << CThostFtdcMdApi::GetApiVersion() << std::endl;
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
        else if (arg == "-m" || arg == "--mode") {
            if (i + 1 < argc) {
                string modeStr = args[++i];
                if (modeStr == "ws") {
                    config.mode = execConfig::WS;
                } else if (modeStr == "subprocess") {
                    config.mode = execConfig::SUBPROCESS;
                } else {
                    std::cerr << "Error: Invalid mode '" << modeStr << "'. Valid modes are: ws, cli, subprocess" << std::endl;
                    std::cerr << hint << std::endl;
                    return 1;
                }
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