#include "TcpConnectionHandler.h"




int TcpConnectionHandler::init(){

     if((this->socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1 ){
         perror("Error creating socket");
         return -1;
     }

    int opt = 1;
    if(setsockopt(this->socket_fd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt)) == -1){
        perror("Error setting socket options");
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(this->port);

    if(inet_aton(this->ip_address,&address.sin_addr) == 0){
        perror("Invalid ip address");
        return -1;
    }

    if(bind(this->socket_fd,(struct sockaddr *) &address,sizeof(address)) == -1){
        perror("Error binding socket");
        return -1;
    }

    return 0;
}

int TcpConnectionHandler::start(){


    if(listen(this->socket_fd,BACKLOG) == -1){
        perror("Error starting listening on socket");
        return -1;
    }

    std::cout << "Server Listening on port " << this->port << "\n";

    std::vector<std::thread> threads;

    while(1){
        int client_socket;
        if((client_socket = accept(this->socket_fd,nullptr,nullptr)) == -1){
            perror("Error accepting connection on socket");
            return -1;
        } 

        threads.push_back(std::thread([this,client_socket]{
            this->processConnection(client_socket);
        }));

    }

    for(auto &th : threads){
        th.join();
    }

    close(this->socket_fd);
    
    return 0;
}


void TcpConnectionHandler::processConnection(int client_socket){
    


    const unsigned int MAX_BUF_LEN = 1024;
    char buff[MAX_BUF_LEN];
    std::string received_data;
    
    int n_data_read;

    do{
        n_data_read = recv(client_socket,buff,MAX_BUF_LEN,0);
        if(n_data_read == -1){
            perror("Error receiving data");
            return;
        }
        received_data.append(buff,n_data_read);
    }
    while(n_data_read == MAX_BUF_LEN);

    HttpRequestHandler http_handler;
    
    std::string http_response_header = http_handler.processRequest(received_data);


    sendAll(client_socket,http_response_header.c_str(),strlen(http_response_header.c_str()));

    sendAll(client_socket,http_handler.getData(),http_handler.getdataLen());
    
    close(client_socket);
    
}

int TcpConnectionHandler::sendAll(int socket, const void *data, int datalen)
{
    const char *ptr = (const char*) data;
    while (datalen > 0) {
        int bytes = send(socket, ptr, datalen, 0);
        if (bytes <= 0) return -1;
        ptr += bytes;
        datalen -= bytes;
    }
    return 0;
}