#pragma once
#include <map>
#include <string>

class HttpHeader
{
public:
    HttpHeader(std::string s):recv_(true),method_(NUL),header_(parseHeader(s)),status_code_(0){}
    HttpHeader():recv_(false),method_(NUL),status_code_(0){}
    HttpHeader(const HttpHeader& other):
        recv_(other.recv_), method_(other.method_),
        path_(other.path_),
        protocol_(other.protocol_),
        status_code_(other.status_code_),
        header_(other.header_)
    {}
    HttpHeader(HttpHeader&& other) noexcept :
        recv_(other.recv_),method_(other.method_),
        path_(std::move(other.path_)),
        protocol_(std::move(other.protocol_)),
        status_code_(other.status_code_),
        header_(std::move(other.header_)) 
    {}
    HttpHeader& operator=(const HttpHeader& other) {
        recv_ = other.recv_;
        method_ = other.method_;
        path_ = other.path_;
        protocol_ = other.protocol_;
        status_code_ = other.status_code_;
        header_ = other.header_;
        return *this;
    }

    HttpHeader& operator=(HttpHeader&& other) noexcept {
        recv_ = other.recv_;
        method_ = other.method_;
        path_ = std::move(other.path_);
        protocol_ = std::move(other.protocol_);
        status_code_ = other.status_code_;
        header_ = std::move(other.header_);
        return *this;
    }
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
