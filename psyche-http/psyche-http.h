#pragma once
#include <psyche/Server.h>
#include <cstdint>
#include <string>
#include "HttpHeader.h"
using namespace std::placeholders;

class HttpServer
{
public:
    HttpServer(std::uint16_t port):server_(port) {
        server_.setReadCallback(std::bind(&HttpServer::handleRequest, this, _1, _2));
    }

    void handleRequest(psyche::Connection conn,psyche::Buffer buffer) {
        HttpHeader recvHeader(buffer.retrieveUntil("\r\n\r\n"));
        HttpHeader responseHeader;
        responseHeader.set("Server", "psyche-http/0.0.1");
        responseHeader.set("Content-Type", "text/html");
        responseHeader.set("Connection", "keep-alive");
        std::string response("<html>123</html>");
        responseHeader.set("Content-Length", std::to_string(response.length()));
        conn.send(responseHeader.to_string(200));
        conn.send(response);
        //conn.close();
    }

    void start() {
        server_.start();
    }

private:
    std::map<std::string, std::string> parseHeader(const std::string& header);

    psyche::Server server_;
};