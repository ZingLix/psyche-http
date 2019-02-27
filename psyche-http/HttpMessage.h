#pragma once
#include "HttpHeader.h"
#include <string>
#include <memory>

class HttpMessage
{
public:
    HttpMessage():body_(){}
    HttpMessage(const HttpMessage& other) :
        body_(other.body_)
    {}
    HttpMessage(HttpMessage&& other) noexcept:
        body_(std::move(other.body_))
    {}

    HttpMessage& operator=(const HttpMessage& other) {
        body_ = other.body_;
        return *this;
    }

    HttpMessage& operator=(HttpMessage&& other) noexcept {
        body_ = std::move(other.body_);
        return *this;
    }

    virtual ~HttpMessage(){}

    void appendBody(const std::string& str) {
        body_ += str;
    }

    size_t bodyLength() const {
        return body_.length();
    }

    virtual std::string to_string() = 0;

protected:
    std::string body_;
};

class HttpRequest:public HttpMessage
{
public:
    HttpRequest():HttpMessage(){}

    HttpRequest(const std::string& message):header_(message) {

    }

    std::string getPath() {
        return header_.getPath();
    }

    std::string getProtocol() {
        return header_.getProtocol();
    }

    HttpHeader::method getMethod() {
        return header_.getMethod();
    }

    std::string get(const std::string& key) {
        return header_.get(key);
    }

    std::string to_string() override {
        return header_.to_string() + body_;
    }

    void parseHeader(const std::string& msg) {
        header_.setHeader(msg);
    }

    ~HttpRequest(){}

private:
    HttpRequestHeader header_;
};

class HttpResponse :public HttpMessage
{
public:
    HttpResponse():HttpMessage() {

    }

    void setStatusCode(int status_code) {
        header_.setStatusCode(status_code);
    }

    void setProtocol(std::string protocol){
        header_.setProtocol(protocol);
    }

    void set(const std::string& key, const std::string& value) {
        header_.set(key, value);
    }

    std::string to_string() override {
        return header_.to_string() + body_;
    }

    void setContentLength() {
        set("Content-Length", std::to_string(bodyLength()));
    }

    ~HttpResponse(){}

private:
    HttpResponseHeader header_;
};