#pragma once
#include <psyche/psyche.h>
#include <cstdint>
#include <string>
#include "HttpHeader.h"
#include "HttpMessage.h"

namespace psyche_http
{
const std::string PSYCHE_HTTP_VERSION = "0.0.1";

class HttpServer
{
public:
    using RecvCallback = std::function<void(HttpRequest&, HttpResponse&)>;

    explicit HttpServer(std::uint16_t port);

    void handle_request(psyche::Connection conn, psyche::Buffer buffer);
    void set_recv_callback(RecvCallback cb);
    void start();

private:
    bool recvBody(psyche::Buffer& buffer, HttpRequest& msg);

    psyche::Server server_;
    psyche::ThreadPool thread_pool_;
    std::map<psyche::Connection, HttpRequest> unfinishedRequest_;
    RecvCallback recv_callback_;
};
}
