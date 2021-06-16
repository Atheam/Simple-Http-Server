#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>


#define NOT_FOUND 404
#define FORBIDDEN 403
#define BAD_REQUEST 400
#define INT_SERV_ERR 500
#define OK 200

#define BASE_PATH "public_html"

class HttpRequestHandler{
    public:

        //returns response based on given request
        std::string processRequest(const std::string request);
        
        //parses request, returnrs false if request is invalid
        int parseRequest(const std::string request);

        //creates ready to send http response
        std::string createResponseHeader(int status_code, std::string status_text, 
                                    std::map<std::string,std::string> headers);

        //finds resource and sets status code (200 if OK) returns nullptr on error
        int getResource(std::string filename, int *status_code);

        //create data to send based on error (creates a string "{error_code} - {error_text}")
        void createErrorData(int status_code);


        char * getData() const{
            return this->data;
        }

        size_t getdataLen() const{
            return this->data_len;
        }


    private:

        static const std::map<std::string,std::string>          content_types; //list of extensions mapped to content types      
        static const std::map<int,std::string>                  status_codes; //a list of status codes mapped to status texts
        std::string                                             filename; // filename specified in request
        std::string                                             method; // method specified in request
        std::map<std::string,std::string>                       request_headers; // headers specified in request
        char *                                                  data; // data that have been requested
        size_t                                                  data_len; //size of data that have been requested
};
    




