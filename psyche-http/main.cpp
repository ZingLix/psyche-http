#include "HttpServer.h"
#include <fcntl.h>

int main(int argc, char* argv[]) {
    HttpServer server(8898);
    server.setRecvCallback([](HttpMessage & recvmsg, HttpMessage & sendmsg) {
        std::string path = recvmsg.getPath();
        if (path.back() == '/') path += "index.html";
        path = "/home/vs/blog" + path;
        int fd = open(path.c_str(), O_RDONLY);
        if(fd==-1) {
            sendmsg.setStatusCode(404);
            return;
        }
        char buf[65536];
        size_t n;
        while((n=read(fd,buf,sizeof buf))!=0) {
            sendmsg.appendBody(std::string(buf,n));
        }
        sendmsg.setStatusCode(200);
    });
    server.start();
}
