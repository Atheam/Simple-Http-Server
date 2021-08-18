#include "TcpConnectionHandler.h"

#define CONFIG "config.txt"

#define FAIL -1

int main(){

    std::string ip_addr;
    std::string port;

    std::ifstream infile(CONFIG);
    std::string line;
    while(std::getline(infile,line)){
        if(line[0] != 0){
            std::istringstream iss(line);
            std::string name;
            std::string val;
            iss >> name >> val;
            if(name == "ipaddress") ip_addr = val;
            if(name == "port") port = val;
        }
    }

    TcpConnectionHandler server(ip_addr.c_str(),std::stoi(port));

    if(server.init() == FAIL) server.cleanup();
    if(server.start() == FAIL) server.cleanup();

    return 0;
}

