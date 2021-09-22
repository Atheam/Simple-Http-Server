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
                {"html","text/html;charset=utf-8'"},
                {"", "text/plain"},
                {"txt", "text/plain"}
                };

HttpResponse HttpRequestHandler::processRequest(std::string request){

    if(this->parseRequest(request) == FAIL){
        this->response.setStatus(400,status_codes.at(400));
        this->response.setDataFromString(createStringError(400));
        this->response.addHeader("Content-length",std::to_string(this->response.getDataLength()));
        this->response.addHeader("Content-Type","text/plain");
        return this->response;
   }
   
    switch(this->method){
        case Method::GET: return getMethodHandler();
        case Method::POST: return postMethodHandler();
        case Method::PUT: return putMethodHandler();
    }
    
}

HttpResponse HttpRequestHandler::getMethodHandler(){
     if(getResource(this->request.getPath()) == FAIL){
        this->response.setDataFromString(createStringError(this->response.getStatus().first));
        this->response.addHeader("Content-length",std::to_string(this->response.getDataLength()));
        this->response.addHeader("Content-Type","text/plain");
        return this->response;
    }
    
    std::string ext = getExt(this->request.getPath());
    this->response.addHeader("Content-length",std::to_string(this->response.getDataLength()));
    this->response.addHeader("Content-Type",content_types.at(ext));
    return this->response;
}

HttpResponse HttpRequestHandler::postMethodHandler(){
    if(createResource(this->request.getPath(), this->request.getData()) == FAIL){
        this->response.setDataFromString(createStringError(this->response.getStatus().first));
        this->response.addHeader("Content-length",std::to_string(this->response.getDataLength()));
        this->response.addHeader("Content-Type","text/plain");
    }
    return this->response;
}

HttpResponse HttpRequestHandler::putMethodHandler(){
    if(updateResource(this->request.getPath(), this->request.getData()) == FAIL){
        this->response.setDataFromString(createStringError(this->response.getStatus().first));
        this->response.addHeader("Content-length",std::to_string(this->response.getDataLength()));
        this->response.addHeader("Content-Type","text/plain");
        
    }
    return this->response;
}


int HttpRequestHandler::parseRequest(const std::string request){
    
    if(request.empty()){
        return FAIL;
    }

    std::stringstream ss(request);
    std::string buff;
    std::getline(ss,buff);

    std::stringstream line(buff); 
    std::string method_buff;
    std::string version;
    std::string path;

    if(!buff.empty()){
        line >> method_buff  >> path >> version;

        if(path.empty() || version != "HTTP/1.1") return FAIL;

        if(method_buff == "GET") this->method = Method::GET;
        else if(method_buff == "POST") this->method = Method::POST;
        else if(method_buff == "PUT") this->method = Method::PUT;
        else return FAIL;
        
    }
    else{
        return FAIL;
    }

    this->request = HttpRequest(method_buff,version,path);

    std::string name;
    std::string value;
    while(std::getline(ss,buff) && buff != "\r"){
        int pos = buff.find(":");
        name = buff.substr(0,pos);
        value = buff.substr(pos+1);
        if(name.empty() || value.empty()) return FAIL;
        this->request.addHeader(name,value);
    }

    std::vector<char> data_buff;
    if(method_buff == "POST" || method_buff == "PUT") {
        char c;
        while(ss >> std::noskipws >> c){
            data_buff.push_back(c);
        }
        this->request.setData(data_buff);
    }
    return 0;
}

int HttpRequestHandler::checkPath(std::string path){
    if(path.find("..") != std::string::npos || (path.length() > 0  && path[0] != '/')){
        return FAIL;
    }
    return 0;
}


int HttpRequestHandler::createResource(std::string path, std::vector<char> data){
    if(checkPath(path) == FAIL){
        this->response.setStatus(FORBIDDEN,status_codes.at(FORBIDDEN));
        return FAIL;
    }

    std::string final_path = BASE_PATH + path;

    createFile(final_path, data);

    if(checkIfFileCorrect(final_path) == FAIL) return FAIL;

    this->response.setStatus(OK,status_codes.at(OK));
    return 0;
}

int HttpRequestHandler::updateResource(std::string path, std::vector<char> data){
    if(checkPath(path) == FAIL){
        this->response.setStatus(FORBIDDEN,status_codes.at(FORBIDDEN));
        return FAIL;
    }

    std::string final_path = BASE_PATH + path;

    if(!std::filesystem::exists(final_path)){
        this->response.setStatus(NOT_FOUND,status_codes.at(NOT_FOUND));
        return FAIL; 
    }

    createFile(final_path, data);

    if(checkIfFileCorrect(final_path) == FAIL) return FAIL;
    

    this->response.setStatus(OK,status_codes.at(OK));
    return 0;
}

void HttpRequestHandler::createFile(std::string path, std::vector<char> data){
    std::ofstream fout(path, std::ios::out | std::ios::binary);
    fout.write((char*)&data[0], data.size() * sizeof(char));
    fout.close();
}

int HttpRequestHandler::checkIfFileCorrect(std::string path){
    std::ifstream file(path,std::ios::in | std::ios::binary);

    if(!file.is_open()){
        this->response.setStatus(INT_SERV_ERR,status_codes.at(INT_SERV_ERR));
        return FAIL;
    }
    return 0;
}

int HttpRequestHandler::getResource(const std::string path){

    if(checkPath(path) == FAIL){
        this->response.setStatus(FORBIDDEN,status_codes.at(FORBIDDEN));
        return FAIL;
    }

    std::string final_path = BASE_PATH + path;
     if(final_path  == BASE_PATH"/"){
        final_path = BASE_PATH"/index.html";
    }
    
    if(!std::filesystem::exists(final_path)){
        this->response.setStatus(NOT_FOUND,status_codes.at(NOT_FOUND));
        return FAIL; 
    }
    
    
    std::ifstream file(final_path,std::ios::in | std::ios::binary);

    if(checkIfFileCorrect(final_path) == FAIL) return FAIL;
    
    this->response.setDataFromVector(std::vector<char>(std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()));
    file.close();

    this->response.setStatus(OK,status_codes.at(OK));
    
    return 0;
}

std::string HttpResponse::getResponseHeader(){
    
    std::stringstream response;

    response << "HTTP/1.1 " << this->status.first << " " << this->status.second << "\r\n";
    for(const auto& kv:headers){
        response << kv.first << ":" << kv.second << "\r\n";
    }
    response << "\r\n";

    return response.str();
}

std::string HttpRequestHandler::createStringError(int status_code){
    std::stringstream ss;
    ss << std::to_string(status_code) << " - " << status_codes.at(status_code);
    return ss.str();
}

std::string HttpRequestHandler::getExt(std::string filename){
    if(filename == "/") return "html";
    std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if(ext == filename) return "";
    return ext;
}

void HttpResponse::print(){
    std::cout << this-> getResponseHeader();
    std::cout << this->data.data();
}