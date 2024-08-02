#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int create_listener(char* service) {
    struct addrinfo* address = nullptr;
    struct addrinfo hint = {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM
    };
    int gai_err = getaddrinfo(nullptr, service, &hint, &address);
    if (gai_err) {
        std::cerr << "gai err: " << gai_err << '\n';
        return -1;
    }
    int sock = -1;
    for (struct addrinfo* ai = address; ai; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype, 0);
        if (sock < 0) {
            continue;
        }
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0 || listen(sock, SOMAXCONN) < 0) {
            close(sock);
            sock = -1;
            continue;
        }
        break;
    }
    if (sock < 0) {
        std::cerr << "Failed to connect to the server\n";
    }
    freeaddrinfo(address);
    return sock;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " SERVICE\n";
        return 1;
    }
    int sock = create_listener(argv[1]);
    if (sock < 0) {
        return 1;
    }
    int connection = accept(sock, nullptr, nullptr);

    const char* message = "OK";
    write(connection, message, strlen(message));
    close(sock);
    return 0;
}