#include "psyche-http.h"

int main(int argc, char* argv[]) {
    HttpServer server(8898);
    server.start();
}
