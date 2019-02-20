#pragma once
#include <map>
#include <string>

class HttpHeader
{
public:
    HttpHeader(std::string s):recv_(true),method_(NUL),header_(parseHeader(s)){}
    HttpHeader():recv_(false),method_(NUL){}
    enum method
    {
        NUL,GET,HEAD,PUT,POST,TRACE,OPTIONS,DELETE
    };

    
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void erase(const std::string& key);
    void setStatusCode(int status_code) {
        status_code_ = status_code;
    }
    std::string getPath() const {
        return header_.find("path")->second;
    }
    std::string to_string();
private:
    using Header = std::map<std::string, std::string>;

    Header parseHeader(const std::string& header);
    bool recv_;
    method method_;
    std::string path_;
    std::string protocol_;
    int status_code_;
    Header header_;
};
