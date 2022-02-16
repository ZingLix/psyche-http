#include "HttpServer.h"
#include "Url.h"
#include <functional>

using namespace std::placeholders;
using namespace psyche_http;


HttpServer::HttpServer(std::uint16_t port) : server_(port), thread_pool_(16) {
    server_.set_read_callback(std::bind(&HttpServer::handle_request, this, _1, _2));
}

void HttpServer::handle_request(psyche::Connection conn, psyche::Buffer buffer) {
    auto it = unfinishedRequest_.find(conn);
    HttpRequest request_msg;
    HttpResponse response_msg;
    bool close_flag = true;

    if (it == unfinishedRequest_.end()) {
        request_msg.parseHeader(buffer.retrieve_until("\r\n\r\n"));
        response_msg.set("Server", "psyche-http/" + PSYCHE_HTTP_VERSION);
        response_msg.set_protocol(request_msg.get_protocol());
        if (request_msg.get("Connection") == "keep-alive") {
            response_msg.set("Connection", "keep-alive");
            close_flag = false;
        }
        if (!recvBody(buffer, request_msg)) {
            unfinishedRequest_[conn] = request_msg;
            return;
        }
    }
    else {
        request_msg = it->second;
        if (recvBody(buffer, request_msg)) {
            unfinishedRequest_.erase(it);
        }
        else {
            return;
        }
    }
    thread_pool_.execute(
        [=, con = std::move(conn), request = std::move(request_msg), response = std::move(response_msg)]() mutable
    {
        auto [callback, arguments] = this->router_.get_callback(request.get_path(), request.get_method());
        request.set_arguments(std::move(arguments));
        callback(request, response);
        response.set_content_length();
        auto tmp = response.to_string();
        con.send(response.to_string());
        if (close_flag) con.close();
    });

}

void HttpServer::set_recv_callback(std::string url, HttpHeader::Method method, RecvCallback cb) {
    this->router_.add_callback(url, method, cb);
}

void HttpServer::get(std::string url, RecvCallback cb) {
    this->set_recv_callback(url, HttpHeader::Method::GET, cb);
}
void HttpServer::post(std::string url, RecvCallback cb) {
    this->set_recv_callback(url, HttpHeader::Method::POST, cb);
}

void HttpServer::del(std::string url, RecvCallback cb) {
    this->set_recv_callback(url, HttpHeader::Method::DELETE, cb);
}


void HttpServer::start() {
    server_.start();
}

bool HttpServer::recvBody(psyche::Buffer& buffer, HttpRequest& msg) {
    if (msg.get("Content-Length") == "") {
        msg.append_body(buffer.retrieve_all());
    }
    else {
        auto length = std::stoi(msg.get("Content-Length"));
        if (buffer.available() < length) return false;
        msg.append_body(buffer.retrieve(length));
    }
    return true;
}

std::pair<HttpServer::RecvCallback, std::map<std::string, std::string>> HttpServer::Router::
get_callback(std::string url, HttpHeader::Method method) {
    Url u(url);
    std::map<std::string, std::string> arguments;
    auto cur_node = this->node;
    for (auto it = u.path_list.begin(); it != u.path_list.end(); ++it) {
        if (cur_node->arguments != "") {
            arguments.insert(std::make_pair(cur_node->arguments, *it));
            cur_node = cur_node->wildcard_next;
        }
        else {
            auto next = cur_node->next.find(*it);
            if (next != cur_node->next.end()) {
                cur_node = cur_node->next[*it];
            }
            else {
                return std::make_pair(error_response404, std::map<std::string, std::string>());
            }
        }
    }
    auto callback = cur_node->callback_map.find(method);
    if (callback == cur_node->callback_map.end()) {
        return std::make_pair(error_response405, std::map<std::string, std::string>());
    }
    return std::make_pair(callback->second, arguments);
}

void HttpServer::Router::add_callback(std::string url, HttpHeader::Method method, RecvCallback callback) {
    Url u(url);
    auto cur_node = this->node;
    for (auto it = u.path_list.begin(); it != u.path_list.end(); ++it) {
        if ((*it)[0] == ':') {
            cur_node->wildcard_next = std::make_shared<UrlNode>();
            cur_node->arguments = (*it).substr(1, it->size());
            cur_node = cur_node->wildcard_next;
        }
        else {
            cur_node->next[*it] = std::make_shared<UrlNode>();
            cur_node = cur_node->next[*it];
        }
    }
    cur_node->callback_map[method] = callback;
}


void psyche_http::error_response404(HttpRequest& request, HttpResponse& response) {
    response.set_status_code(404);
    response.append_body("404 NOT FOUND");
}

void psyche_http::error_response405(HttpRequest& request, HttpResponse& response) {
    response.set_status_code(405);
    response.append_body("405 METHOD NOT ALLOWED");
}