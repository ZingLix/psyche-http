#pragma once
#include <psyche/psyche.h>
#include <cstdint>
#include <string>
#include "HttpHeader.h"
#include "HttpMessage.h"

namespace psyche_http
{
    const std::string PSYCHE_HTTP_VERSION = "0.0.1";

    void error_response404(HttpRequest& request, HttpResponse& response);
    void error_response405(HttpRequest& request, HttpResponse& response);
    class HttpServer
    {
    public:
        using RecvCallback = std::function<void(HttpRequest&, HttpResponse&)>;

        explicit HttpServer(std::uint16_t port);

        void handle_request(psyche::Connection conn, psyche::Buffer buffer);
        void set_recv_callback(std::string url, HttpHeader::Method method, RecvCallback cb);
        void get(std::string url, RecvCallback cb);
        void post(std::string url, RecvCallback cb);
        void del(std::string url, RecvCallback cb);
        void start();

    private:
        bool recvBody(psyche::Buffer& buffer, HttpRequest& msg);

        class Router
        {
        public:
            Router() :node(std::make_shared<UrlNode>()) {}
            std::pair<RecvCallback, std::map<std::string, std::string>> get_callback(std::string url, HttpHeader::Method method);
            void add_callback(std::string url, HttpHeader::Method method, RecvCallback callback);

            struct UrlNode
            {
                std::string arguments;
                std::shared_ptr<UrlNode> wildcard_next;
                std::map<std::string, std::shared_ptr<UrlNode>> next;
                std::map<HttpHeader::Method, RecvCallback> callback_map;
            };

        private:
            std::shared_ptr<UrlNode> node;
        };

        psyche::Server server_;
        psyche::ThreadPool thread_pool_;
        Router router_;
        std::map<psyche::Connection, HttpRequest> unfinishedRequest_;
    };

}
