#include "HttpServer.h"

bool HttpServer::recvBody(psyche::Buffer& buffer, HttpMessage& msg) {
    if(msg.getHeader("Content-Length")=="") {
        msg.appendBody(buffer.retrieveAll());
    }else {
        auto length = std::stoi(msg.getHeader("Content-Length"));
        if (buffer.available() < length) return false;
        msg.appendBody(buffer.retrieve(length));
    }
    return true;
}
