#include "HttpServer.h"
#include <fcntl.h>
#include <iostream>
#include <dirent.h>

int main(int argc, char* argv[]) {
    if(argc==1) {
        std::cout << "Please input the website root directory.";
        return 0;
    }
    DIR* d = opendir(argv[1]);
    if(d==nullptr) {
        std::cout << "Directory doesn't exist.";
        return -1;
    }
    closedir(d);
    HttpServer server(8898);
    std::string rootpath(argv[1]);
    if (rootpath.back() == '/') rootpath.pop_back();
    server.setRecvCallback([&](HttpMessage & recvmsg, HttpMessage & sendmsg) {
        std::string path = recvmsg.getPath();
        if (path.back() == '/') path += "index.html";
        path = rootpath + path;
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
