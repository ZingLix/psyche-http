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
    using RecvCallback=std::function<void(HttpMessage&, HttpMessage&)>;

    HttpServer(std::uint16_t port):server_(port) {
        server_.setReadCallback(std::bind(&HttpServer::handleRequest, this, _1, _2));
    }

    void handleRequest(psyche::Connection conn,psyche::Buffer buffer) {
        auto it = unfinishedRequest_.find(conn);
        HttpMessage msg,responseMsg;
        bool close_flag = true;
        if(it==unfinishedRequest_.end()) {
            msg = HttpMessage(buffer.retrieveUntil("\r\n\r\n"));
            responseMsg.setHeader("Server", "psyche-http/" + PSYCHE_HTTP_VERSION);
            if (msg.getHeader("Connection") == "keep-alive") {
                responseMsg.setHeader("Connection", "keep-alive");
                close_flag = false;
            }
            if(!recvBody(buffer,msg)) {
                unfinishedRequest_[conn] = msg;
                return;
            }
        } else {
            msg = it->second;
            if(recvBody(buffer,msg)) {
                unfinishedRequest_.erase(it);
            }else {
                return;
            }
        }
        thread_pool_.Execute([=,con=std::move(conn),recv=std::move(msg),send=std::move(responseMsg)]() mutable
        {
            if (recv_callback_) recv_callback_(recv, send);
            send.setContentLength();
            auto tmp = send.to_string();
            con.send(tmp);
            if (close_flag) con.close();
        });

    }

    //constexpr static std::string error(int status_code) {
   //     HttpHeader response;
     //   response.set("Server", "psyche-http/" + PSYCHE_HTTP_VERSION);
   //     return response.to_string(200);
   // }

    void setRecvCallback(RecvCallback cb) {
        recv_callback_ = cb;
    }

    void start() {
        server_.start();
    }

private:
    bool recvBody(psyche::Buffer& buffer, HttpMessage& msg);

    psyche::Server server_;
    psyche::ThreadPool thread_pool_;
    std::map<psyche::Connection, HttpMessage> unfinishedRequest_;
    RecvCallback recv_callback_;
};