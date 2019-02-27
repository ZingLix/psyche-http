#pragma once
#include <psyche/psyche.h>
#include <cstdint>
#include <string>
#include "HttpHeader.h"
#include "HttpMessage.h"
using namespace std::placeholders;

const std::string PSYCHE_HTTP_VERSION = "0.0.1";

class HttpServer
{
public:
    using RecvCallback=std::function<void(HttpRequest&, HttpResponse&)>;

    HttpServer(std::uint16_t port):server_(port) {
        server_.setReadCallback(std::bind(&HttpServer::handleRequest, this, _1, _2));
    }

    void handleRequest(psyche::Connection conn,psyche::Buffer buffer) {
        auto it = unfinishedRequest_.find(conn);
        HttpRequest request_msg;
        HttpResponse response_msg; 
        bool close_flag = true;
        if(it==unfinishedRequest_.end()) {
            request_msg.parseHeader(buffer.retrieveUntil("\r\n\r\n"));
            response_msg.set("Server", "psyche-http/" + PSYCHE_HTTP_VERSION);
            response_msg.setProtocol(request_msg.getProtocol());
            if (request_msg.get("Connection") == "keep-alive") {
                response_msg.set("Connection", "keep-alive");
                close_flag = false;
            }
            if(!recvBody(buffer, request_msg)) {
                unfinishedRequest_[conn] = request_msg;
                return;
            }
        } else {
            request_msg = it->second;
            if(recvBody(buffer, request_msg)) {
                unfinishedRequest_.erase(it);
            }else {
                return;
            }
        }
        thread_pool_.Execute([=,con=std::move(conn),request=std::move(request_msg),response=std::move(response_msg)]() mutable
        {
            if (recv_callback_) recv_callback_(request, response);
            response.setContentLength();
            auto tmp = response.to_string();
            con.send(response.to_string());
            if (close_flag) con.close();
        });

    }

    void setRecvCallback(RecvCallback cb) {
        recv_callback_ = cb;
    }

    void start() {
        server_.start();
    }

private:
    bool recvBody(psyche::Buffer& buffer, HttpRequest& msg);

    psyche::Server server_;
    psyche::ThreadPool thread_pool_;
    std::map<psyche::Connection, HttpRequest> unfinishedRequest_;
    RecvCallback recv_callback_;
};