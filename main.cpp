#include "TcpConnectionHandler.h"

#define CONFIG "config.txt"


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

    server.init();
    server.start();

    return 0;
}

