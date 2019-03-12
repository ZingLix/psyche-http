#include "HttpServer.h"
#include <fcntl.h>
#include <iostream>
#include <dirent.h>

int main(int argc, char* argv[]) {
    if(argc==1) {
        std::cout << "Please input the website root directory.";
        //return 0;
    }
  //  DIR* d = opendir(argv[1]);
   // if(d==nullptr) {
   //     std::cout << "Directory doesn't exist.";
   //     return -1;
 //   }
 //   closedir(d);
    HttpServer server(8898);
//    std::string rootpath(argv[1]);
    std::map<std::string, std::string> buffer;
    std::string rootpath("/home/vs/blog");
    if (rootpath.back() == '/') rootpath.pop_back();
    server.setRecvCallback([&](HttpRequest & recvmsg, HttpResponse & sendmsg) {
        std::string path = recvmsg.getPath();
        if (path.back() == '/') path += "index.html";
        path = rootpath + path;
        std::string response;
        if (auto it = buffer.find(path);it != buffer.end()) {
            response = it->second;
        }else {
            int fd = open(path.c_str(), O_RDONLY);
            if (fd == -1) {
                sendmsg.setStatusCode(404);
                return;
            }
            char buf[65536];
            size_t n;
            while ((n = read(fd, buf, sizeof buf)) != 0) {
                response += std::string(buf, n);
            }
            close(fd);
        }
        sendmsg.appendBody(response);
        sendmsg.setStatusCode(200);
        
    });
    server.start();
}
