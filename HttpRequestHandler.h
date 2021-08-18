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
#define FAIL -1

#define BASE_PATH "public_html"

#include <iostream>
#include <vector>
#include <map>

class HttpResponse{

        private:
            std::map<std::string, std::string> headers;
            std::vector<char>                  data;
            std::pair<int,std::string>         status;

        public:
            void addHeader(std::string header_name, std::string header_value){this->headers[header_name] = header_value;}

            void setStatus(int code, std::string text){status = std::pair<int, std::string>{code,text};}

            int getDataLength(){return data.size();}

            std::pair<int,std::string> getStatus() {return status;}

            void setDataFromString(std::string str_data){std::copy(str_data.begin(),str_data.end(),std::back_inserter(data));}

            void setDataFromVector(std::vector<char> vec_data){this->data = vec_data;}

            //returns response header ie. first part of the response (everything before body), version, status, and all the headers as string
            std::string getResponseHeader();

            std::vector<char> getData(){return this->data;}

            void print();
 
};

#include <iostream>
#include <map>

class HttpRequest{

        public:
            HttpRequest(){}
            HttpRequest(std::string method, std::string version, std::string path):
            method(method), version(version), path(path){}

            void addHeader(std::string header_name, std::string header_value){this->headers[header_name] = header_value;}

            std::string getPath(){return path;}

        private:
            std::string                             method;
            std::string                             version;
            std::string                             path;
            std::map<std::string, std::string>      headers;
                
};

enum Method {GET,POST,PUT};

class HttpRequestHandler{
    public:
        
        //returns response based on given request
        HttpResponse processRequest(const std::string request);
        
        //parses request, returns false if request is invalid
        int parseRequest(const std::string request);

        //finds resource and sets status code (200 if OK) returns nullptr on error
        int getResource(std::string filename);

        HttpResponse  getMethodHandler();

        //create data to send based on error (creates a string "{error_code} - {error_text}")
        std::string createStringError(int status_code);

        std::string getExt(std::string filename);

    private:
        static const std::map<std::string,std::string>          content_types; //list of extensions mapped to content types      
        static const std::map<int,std::string>                  status_codes; //a list of status codes mapped to status texts
        HttpResponse                                            response;
        HttpRequest                                             request;
        Method                                                  method;
};







    




