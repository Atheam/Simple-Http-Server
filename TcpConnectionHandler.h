#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <thread> 
#include "HttpRequestHandler.h"

#define BACKLOG 100


class TcpConnectionHandler{

    public:

        TcpConnectionHandler(const char* addr, int port_number):
            ip_address(addr), port(port_number) { }

        //initializes sockets 
        int init();

        //starts listening 
        int start();

        //function executed by a separate thread, processes a single connection
        void processConnection(int client_socket);

        //sends all the data to a given handler
        int sendAll(int sckt, const void *data, int datalen);

    private:
        const char*     ip_address;    //IP address 
        int             port;          //port number
        int             socket_fd;     //socket fd

};