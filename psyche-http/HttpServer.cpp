#include "HttpServer.h"

using namespace std::placeholders;
using namespace psyche_http;

HttpServer::HttpServer(std::uint16_t port): server_(port), thread_pool_(16) {
    server_.setReadCallback(std::bind(&HttpServer::handle_request, this, _1, _2));
}

void HttpServer::handle_request(psyche::Connection conn, psyche::Buffer buffer) {
    auto it = unfinishedRequest_.find(conn);
    HttpRequest request_msg;
    HttpResponse response_msg;
    bool close_flag = true;

    if (it == unfinishedRequest_.end()) {
        request_msg.parseHeader(buffer.retrieveUntil("\r\n\r\n"));
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
    thread_pool_.Execute(
        [=,con=std::move(conn),request=std::move(request_msg),response=std::move(response_msg)]() mutable
        {
            if (recv_callback_) recv_callback_(request, response);
            response.set_content_length();
            auto tmp = response.to_string();
            con.send(response.to_string());
            if (close_flag) con.close();
        });

}

void HttpServer::set_recv_callback(RecvCallback cb) {
    recv_callback_ = cb;
}

void HttpServer::start() {
    server_.start();
}

bool HttpServer::recvBody(psyche::Buffer& buffer, HttpRequest& msg) {
    if(msg.get("Content-Length")=="") {
        msg.append_body(buffer.retrieveAll());
    }else {
        auto length = std::stoi(msg.get("Content-Length"));
        if (buffer.available() < length) return false;
        msg.append_body(buffer.retrieve(length));
    }
    return true;
}
