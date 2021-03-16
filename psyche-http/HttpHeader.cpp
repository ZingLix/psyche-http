#include "HttpHeader.h"

using namespace psyche_http;

HttpHeader::HttpHeader() = default;

HttpHeader::HttpHeader(const std::string& s): header_(parse_header(s)) {
}

HttpHeader::HttpHeader(const HttpHeader& other) = default;

HttpHeader::HttpHeader(HttpHeader&& other) noexcept:
    header_(std::move(other.header_)) {
}

HttpHeader& HttpHeader::operator=(const HttpHeader& other) = default;

HttpHeader& HttpHeader::operator=(HttpHeader&& other) noexcept {
    header_ = std::move(other.header_);
    return *this;
}

HttpHeader::~HttpHeader() = default;

void HttpHeader::set(const std::string& key, const std::string& value) {
    if (value.length() == 0) erase(key);
    else header_[key] = value;
}

void HttpHeader::erase(const std::string& key) {
    header_.erase(key);
}

void HttpHeader::set_header(const std::string& msg) {
    header_ = parse_header(msg);
}

HttpHeader::Header HttpHeader::parse_header(const std::string& header) {
    std::map<std::string, std::string> result;
    auto it = header.begin();
    it += parse_first_line(header);
    auto last_it = it;
    std::string cur_key, cur_val;
    bool flag = true;
    for (; it != header.end(); ++it) {
        if (flag) {
            if (*it == ':') {
                cur_key = std::string(last_it, it);
                if (*(it + 1) == ' ') ++it;
                last_it = it + 1;
                flag = !flag;
            }
        }
        else {
            if (*it == '\r') {
                cur_val = std::string(last_it, it);
                if (cur_val == "\r\n") break;
                result[cur_key] = cur_val;
                it += 2;
                last_it = it;
                flag = !flag;
            }
        }
    }
    return result;
}

HttpRequestHeader::HttpRequestHeader(): method_(Method::NUL) {
}

HttpRequestHeader::HttpRequestHeader(const std::string& message): HttpHeader(message), method_(Method::NUL) {
}

HttpRequestHeader::HttpRequestHeader(const HttpRequestHeader& other):
    HttpHeader(other), method_(other.method_),
    path_(other.path_), protocol_(other.protocol_) {
}

HttpRequestHeader::HttpRequestHeader(HttpRequestHeader&& other) noexcept:
    HttpHeader(std::move(other)),
    method_(other.method_),
    path_(std::move(other.path_)),
    protocol_(std::move(other.protocol_)) {
}

HttpRequestHeader& HttpRequestHeader::operator=(const HttpRequestHeader& other) {
    HttpHeader::operator=(other);
    method_ = other.method_;
    path_ = other.path_;
    protocol_ = other.protocol_;
    return *this;
}

HttpRequestHeader& HttpRequestHeader::operator=(HttpRequestHeader&& other) noexcept {
    HttpHeader::operator=(std::move(other));
    method_ = other.method_;
    path_ = std::move(other.path_);
    protocol_ = std::move(other.protocol_);
    return *this;
}

std::string HttpRequestHeader::get_path() const {
    return path_;
}

HttpHeader::Method HttpRequestHeader::get_method() const {
    return method_;
}

std::string HttpRequestHeader::get_protocol() const {
    return protocol_;
}

size_t HttpRequestHeader::parse_first_line(const std::string& msg) {
    auto it = msg.begin();
    auto last_it = it;
    int i = 0;
    for (;; ++it) {
        if (*it == ' ' || *it == '\r') {
            if (i == 0) {
                method_ = string_to_method(std::string(last_it, it));
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

std::string HttpRequestHeader::first_line() {
    return std::string();
}

HttpResponseHeader::HttpResponseHeader(): status_code_(0) {
}

void HttpResponseHeader::set_protocol(const std::string& protocol) {
    protocol_ = protocol;
}

void HttpResponseHeader::set_status_code(int status_code) {
    status_code_ = status_code;
}

std::string HttpResponseHeader::first_line() {
    if (status_code_ == 0) throw std::runtime_error("Status code not set.");
    return protocol_ + " " + status_code_definition(status_code_);
}

const char* HttpResponseHeader::status_code_definition(int status_code) {
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
        case 504:
            return "504 Gateway Timeout";
        case 505:
            return "505 HTTP Version Not Supported";
        default:
            return "";
    }
}

size_t HttpResponseHeader::parse_first_line(const std::string& msg) { return -1; }

HttpHeader::Method HttpHeader::string_to_method(const std::string& s) {
    if (s == "GET") {
        return Method::GET;
    }
    if (s == "HEAD") {
        return Method::HEAD;
    }
    if (s == "PUT") {
        return Method::PUT;
    }
    if (s == "POST") {
        return Method::POST;
    }
    if (s == "TRACE") {
        return Method::TRACE;
    }
    if (s == "OPTIONS") {
        return Method::OPTIONS;
    }
    if (s == "DELETE") {
        return Method::DELETE;
    }
    if (s == "PATCH") {
        return Method::PATCH;
    }
    if (s == "CONNECT") {
        return Method::CONNECT;
    }
    return Method::NUL;
}

std::string HttpHeader::get(const std::basic_string<char>& key) {
    auto it = header_.find(key);
    if (it != header_.end()) return it->second;
    return std::string();
}


std::string HttpHeader::to_string() {
    std::string s(first_line());
    s += "\r\n";
    for (auto it = header_.begin(); it != header_.end(); ++it) {
        s += it->first + ": " + it->second + "\r\n";
    }
    s += "\r\n";
    return s;
}
