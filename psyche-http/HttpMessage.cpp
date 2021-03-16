#include "HttpMessage.h"

using namespace psyche_http;

HttpMessage::HttpMessage() = default;

HttpMessage::HttpMessage(const HttpMessage& other):
    body_(other.body_) {
}

HttpMessage::HttpMessage(HttpMessage&& other) noexcept:
    body_(std::move(other.body_)) {
}

HttpMessage& HttpMessage::operator=(const HttpMessage& other) {
    body_ = other.body_;
    return *this;
}

HttpMessage& HttpMessage::operator=(HttpMessage&& other) noexcept {
    body_ = std::move(other.body_);
    return *this;
}

HttpMessage::~HttpMessage() = default;

void HttpMessage::append_body(const std::string& str) {
    body_ += str;
}

size_t HttpMessage::body_length() const {
    return body_.length();
}

HttpRequest::HttpRequest(): HttpMessage() {
}

HttpRequest::HttpRequest(const std::string& message): header_(message) {

}

std::string HttpRequest::get_path() const {
    return header_.get_path();
}

std::string HttpRequest::get_protocol() const {
    return header_.get_protocol();
}

HttpHeader::Method HttpRequest::get_method() const {
    return header_.get_method();
}

std::string HttpRequest::get(const std::string& key) {
    return header_.get(key);
}

std::string HttpRequest::to_string() {
    return header_.to_string() + body_;
}

void HttpRequest::parseHeader(const std::string& msg) {
    header_.set_header(msg);
}

HttpRequest::~HttpRequest() = default;

HttpResponse::HttpResponse(): HttpMessage() {

}

void HttpResponse::set_status_code(int status_code) {
    header_.set_status_code(status_code);
}

void HttpResponse::set_protocol(std::string protocol) {
    header_.set_protocol(protocol);
}

void HttpResponse::set(const std::string& key, const std::string& value) {
    header_.set(key, value);
}

std::string HttpResponse::to_string() {
    return header_.to_string() + body_;
}

void HttpResponse::set_content_length() {
    set("Content-Length", std::to_string(body_length()));
}

HttpResponse::~HttpResponse() = default;
