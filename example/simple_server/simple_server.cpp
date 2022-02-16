#include "psyche-http/HttpServer.h"

using namespace psyche_http;

int main(int argc, char* argv[]) {
    HttpServer server(8898);
    server.get("/test", [&](HttpRequest& recvmsg, HttpResponse& sendmsg)
        {
            sendmsg.append_body("GET /test !");
            sendmsg.set_status_code(200);
        });
    server.post("/test1", [&](HttpRequest& recvmsg, HttpResponse& sendmsg)
        {
            sendmsg.append_body("POST /test1 !");
            sendmsg.set_status_code(200);
        });
    server.post("/test2/:tt", [&](HttpRequest& recvmsg, HttpResponse& sendmsg)
        {
            sendmsg.append_body("POST /test/" + recvmsg.get_arguments_value("tt"));
            sendmsg.set_status_code(200);
        });
    server.start();
}
