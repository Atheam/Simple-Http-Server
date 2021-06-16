#include "HttpRequestHandler.h"


const std::map<int,std::string> HttpRequestHandler::status_codes = {
                {400,"Bad request"},
                {200,"OK"},
                {404,"Not found"},
                {403,"Forbidden"},
                {500,"Internal Server Error"}
                };

const std::map<std::string,std::string> HttpRequestHandler::content_types = {
                {"jpg","image/jpg"},
                {"png","image/png"},
                {"html","text/html;charset=utf-8'"}
                };


std::string HttpRequestHandler::processRequest(std::string request){

    int status_code;
    std::string status_text;
    std::map<std::string,std::string> response_headers;

    if(this->parseRequest(request) == -1){
        status_code = BAD_REQUEST;
        status_text = status_codes.at(status_code);
        createErrorData(status_code);
        response_headers["Content-length"] = this->data_len;
        response_headers["Content-Type"] = "text/html";
        return createResponseHeader(status_code,status_text,response_headers);
    }

    
    
    if(getResource(filename,&status_code) == -1){
        status_text = status_codes.at(status_code);
        createErrorData(status_code);
        response_headers["Content-length"] = this->data_len;
        response_headers["Content-Type"] = "text/plain";
        return createResponseHeader(status_code,status_text,response_headers);
    }

    std::string ext;
    if(filename != "/") ext = this->filename.substr(this->filename.find_last_of(".") + 1); 
    else ext = "html";

    response_headers["Content-Type"] = content_types.at(ext);
    response_headers["Content-length"] = this->data_len;
    
    
    return createResponseHeader(status_code,status_codes.at(status_code),response_headers);


}

void HttpRequestHandler::createErrorData(int status_code){
    std::stringstream ss;
    ss << std::to_string(status_code) << " - " << status_codes.at(status_code);
    std::string string_data = ss.str();
    this->data = new char[sizeof(string_data)];
    std::copy(string_data.begin(),string_data.end(),this->data);
    this->data_len = string_data.size();
}

int HttpRequestHandler::parseRequest(const std::string request){
    
    if(request.empty()){
        return -1;
    }

    std::stringstream ss(request);
    std::string buff;
    std::getline(ss,buff);

    if(!buff.empty()){
        std::stringstream line(buff); 
        std::string method;
        std::string version;
        std::string path;
        line >> method >> path >> version;
        if(method != "GET" || path.empty() || version != "HTTP/1.1") return 0;
        this->method = method;
        this->filename = path;
    }
    else{
        return -1;
    }

    std::string name;
    std::string value;
    while(std::getline(ss,buff)){
        int pos = buff.find(":");
        name = buff.substr(0,pos);
        value = buff.substr(pos+1);
        if(name.empty() || value.empty()) return -1;
        this->request_headers[name] = value;
    }


    return 0;
}


int HttpRequestHandler::getResource(const std::string path,int *status_code){
    

    std::string data;

    if(path.find("..") != std::string::npos || (path.length() > 0  && path[0] != '/')){
        *status_code = FORBIDDEN;
        return -1;
    }
    
    std::string final_path = BASE_PATH + path;
     if(final_path  == BASE_PATH"/"){
        final_path = BASE_PATH"/index.html";
    }
    
    if(!std::filesystem::exists(final_path)){
        *status_code = NOT_FOUND;
        return -1; 
    }
    

    std::ifstream file(final_path,std::ios::in | std::ios::binary);

    if(!file.is_open()){
        *status_code = INT_SERV_ERR;
        return -1;
    }

    file.seekg(0, std::ios::end);

    this->data_len = file.tellg();
    std::vector<char> v_data;

    v_data.resize(data_len);
    file.seekg(0, std::ios::beg);
    file.read(&v_data[0], data_len);

    this->data = new char[this->data_len];

    std::copy(v_data.begin(),v_data.end(),this->data);
    file.close();

    *status_code = OK;
    
    return 0;
}


std::string HttpRequestHandler::createResponseHeader(int status_code, std::string status_text, 
                         std::map<std::string,std::string> headers){

    std::stringstream response;

    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
    for(const auto& kv:headers){
        response << kv.first << ":" << kv.second << "\r\n";
    }
    response << "\r\n";

    return response.str();


}


