#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int create_connection(char* node, char* service) {
    struct addrinfo* address = nullptr;
    struct addrinfo hint = {
            .ai_family = AF_UNSPEC,
            .ai_socktype = SOCK_STREAM
    };
    int gai_err = getaddrinfo(node, service, &hint, &address);
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
        if (connect(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
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
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " NODE SERVICE\n";
        return 1;
    }
    const char* expected_message = "OK";
    int sock = create_connection(argv[1], argv[2]);
    if (sock < 0) {
        return 1;
    }

    char buf[32] = {};
    if (read(sock, &buf, sizeof(buf) - 1) < 0) {
        std::cerr << "Failed reading\n";
        return 1;
    }

    if (strcmp(buf, expected_message) == 0) {
        std::cout << "Success: \"" << expected_message << "\" received\n";
    } else {
        std::cout << "Fail: \"" << buf << "\" received instead of \"" << expected_message << "\"\n";
    }
    close(sock);
}