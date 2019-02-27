#include "HttpServer.h"

bool HttpServer::recvBody(psyche::Buffer& buffer, HttpRequest& msg) {
    if(msg.get("Content-Length")=="") {
        msg.appendBody(buffer.retrieveAll());
    }else {
        auto length = std::stoi(msg.get("Content-Length"));
        if (buffer.available() < length) return false;
        msg.appendBody(buffer.retrieve(length));
    }
    return true;
}
