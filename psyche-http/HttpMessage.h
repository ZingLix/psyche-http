#pragma once
#include "HttpHeader.h"
#include <string>

class HttpMessage
{
public:
    HttpMessage(){}
    HttpMessage(std::string header):header_(header){}

    std::string getHeader(const std::string& key) {
        return header_.get(key);
    }

    void setHeader(const std::string& key,const std::string& val) {
        header_.set(key, val);
    }

    void setContentLength() {
        header_.set("Content-Length",std::to_string(body_.length()));
    }

    void appendBody(const std::string& str) {
        body_ += str;
    }

    size_t bodyLength() const {
        return body_.length();
    }

    std::string to_string() {
        return std::string(header_.to_string()+body_);
    }

    void setStatusCode(int status_code) {
        header_.setStatusCode(status_code);
    }

    std::string getPath() {
        return header_.getPath();
    }

private:
    HttpHeader header_;
    std::string body_;
};