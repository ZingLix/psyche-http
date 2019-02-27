#pragma once
#include <map>
#include <string>

class HttpHeader
{
public:
    HttpHeader(){}
    HttpHeader(const std::string& s):header_(parseHeader(s)){}
    HttpHeader(const HttpHeader& other):
        header_(other.header_)
    {}
    HttpHeader(HttpHeader&& other) noexcept :
        header_(std::move(other.header_)) 
    {}
    HttpHeader& operator=(const HttpHeader& other) {
        header_ = other.header_;
        return *this;
    }

    HttpHeader& operator=(HttpHeader&& other) noexcept {
        header_ = std::move(other.header_);
        return *this;
    }

    virtual ~HttpHeader(){}

    enum method
    {
        NUL,GET,HEAD,PUT,POST,TRACE,OPTIONS,DELETE,PATCH,CONNECT
    };
    
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void erase(const std::string& key);

    void setHeader(const std::string& msg) {
        header_ = parseHeader(msg);
    }

    std::string to_string();
protected:
    using Header = std::map<std::string, std::string>;

    virtual size_t parseFirstLine(const std::string& msg) = 0;
    Header parseHeader(const std::string& header);
    virtual std::string firstLine() = 0;
    static method stringToMethod(const std::string& s) {
        if (s == "GET") {
            return GET;
        }
        else if (s == "HEAD") {
            return HEAD;
        }
        else if (s == "PUT") {
            return PUT;
        }
        else if (s == "POST") {
            return POST;
        }
        else if (s == "TRACE") {
            return TRACE;
        }
        else if (s == "OPTIONS") {
            return OPTIONS;
        }
        else if (s == "DELETE") {
            return DELETE;
        }
        else if (s == "PATCH") {
            return PATCH;
        }
        else if (s == "CONNECT") {
            return CONNECT;
        }
        return NUL;
    }

    Header header_;
};

class HttpRequestHeader:public HttpHeader
{
public:
    HttpRequestHeader():method_(NUL) {}
    // ReSharper disable once CppPossiblyUninitializedMember
    HttpRequestHeader(const std::string& message):HttpHeader(message){}
    HttpRequestHeader(const HttpRequestHeader& other) :
        HttpHeader(other),method_(other.method_),
        path_(other.path_),protocol_(other.protocol_)
    {}
    HttpRequestHeader(HttpRequestHeader&& other) noexcept :
        HttpHeader(std::move(other)),
        method_(std::move(other.method_)),
        path_(std::move(other.path_)),
        protocol_(std::move(other.protocol_))
    {}
    HttpRequestHeader& operator=(const HttpRequestHeader& other) {
        HttpHeader::operator=(other);
        method_ = other.method_;
        path_ = other.path_;
        protocol_ = other.protocol_;
        return *this;
    }
    HttpRequestHeader& operator=(HttpRequestHeader&& other) noexcept {
        HttpHeader::operator=(std::move(other));
        method_ = other.method_;
        path_ = std::move(other.path_);
        protocol_ = std::move(other.protocol_);
        return *this;
    }

    std::string getPath() {
        return path_;
    }

    method getMethod() {
        return method_;
    }

    std::string getProtocol() {
        return protocol_;
    }

protected:
    size_t parseFirstLine(const std::string& msg) override {
        auto it = msg.begin();
        auto last_it = it;
        int i = 0;
        for (;; ++it) {
            if (*it == ' ' || *it == '\r') {
                if (i == 0) {
                    method_ = stringToMethod(std::string(last_it, it));
                }
                else if (i == 1) {
                    path_ = std::string(last_it, it);
                }
                else {
                    protocol_ = std::string(last_it, it);
                    last_it = it + 2;
                    break;
                }
                ++i;
                last_it = it + 1;
            }
        }
        return last_it - msg.begin();
    }
    std::string firstLine() override {
        return std::string();
    }
    method method_;
    std::string path_;
    std::string protocol_;
};

class HttpResponseHeader:public HttpHeader
{
public:
    HttpResponseHeader():status_code_(0){}

    void setProtocol(const std::string& protocol) {
        protocol_ = protocol;
    }

    void setStatusCode(int status_code) {
        status_code_ = status_code;
    }

protected:
    std::string firstLine() override {
        if (status_code_ == 0) throw std::runtime_error("Status code not set.");
        return protocol_ + " " + status_code_definition(status_code_) ;
    }

    static const char* status_code_definition(int status_code) {
        switch (status_code) {
        case 100:
            return "100 Continue";
        case 101:
            return "101 Switching Protocols";
        case 200:
            return "200 OK";
        case 301:
            return "301 Moved Permanently";
        case 302:
            return "302 Found";
        case 304:
            return "304 Not Modified";
        case 400:
            return "400 Bad Request";
        case 401:
            return "401 Unauthorized";
        case 403:
            return "403 Forbidden";
        case 404:
            return "404 Not Found";
        case 500:
            return "500 Internal Server Error";
        case 502:
            return "502 Bad Gateway";
        case 503:
            return "503 Service Unavailable";
        case 504 :
            return "504 Gateway Timeout";
        case 505:
            return "505 HTTP Version Not Supported";
        default:
            return "";
        }
    }

    size_t parseFirstLine(const std::string& msg) override { return -1; }

    std::string protocol_;
    int status_code_;
};