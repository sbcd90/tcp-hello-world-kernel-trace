#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <csignal>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Run program as 'program <port>'\n";
        return -1;
    }

    auto &portNum = argv[1];
    const unsigned int backLog = 8;

    addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int gAddRes = getaddrinfo(nullptr, portNum, &hints, &res);
    if (gAddRes != 0) {
        std::cerr << gai_strerror(gAddRes) << std::endl;
        return -2;
    }

    std::cout << "Detecting addresses" << std::endl;

    unsigned int numOfAddr = 0;
    char ipStr[INET6_ADDRSTRLEN];

    for (p = res; p != nullptr; p = p->ai_next) {
        void *addr;
        std::string ipVer;

        if (p->ai_family == AF_INET) {
            ipVer = "IPv4";
            sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            addr = &(ipv4->sin_addr);
            ++numOfAddr;
        } else {
            ipVer = "IPv6";
            sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6*>(p->ai_addr);
            addr = &(ipv6->sin6_addr);
            ++numOfAddr;
        }

        inet_ntop(p->ai_family, addr, ipStr, sizeof(ipStr));
        std::cout << "(" << numOfAddr << ") " << ipVer << ": " << ipStr << std::endl;
    }

    if (!numOfAddr) {
        std::cerr << "Found no host address to use\n";
        return -3;
    }

    std::cout << "Enter the number of host address to bind with: ";
    unsigned int choice = 0;
    bool madeChoice = false;

    do {
        std::cin >> choice;
        if (choice > (numOfAddr + 1) || choice < 1) {
            madeChoice = false;
            std::cout << "Wrong choice, try again" << std::endl;
        } else {
            madeChoice = true;
        }
    } while (!madeChoice);

    p = res;

    int sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockFD == -1) {
        std::cerr << "Error while creating socket\n";
        freeaddrinfo(res);
        return -4;
    }

    int bindR = bind(sockFD, p->ai_addr, p->ai_addrlen);
    if (bindR == -1) {
        std::cerr << "Error while binding socket\n";

        close(sockFD);
        freeaddrinfo(res);
        return -5;
    }

    int listenR = listen(sockFD, backLog);
    if (listenR == -1) {
        std::cerr << "Error while Listening on socket\n";

        close(sockFD);
        freeaddrinfo(res);
        return -6;
    }

    sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    const std::string response = "Hello World";

    pid_t pid = getpid();
    std::cout << pid << std::endl;
    while (1) {
        int newFD = accept(sockFD, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_size);
        if (newFD == -1) {
            std::cerr << "Error while Accepting on socket\n";
            continue;
        }

        auto bytes_sent = send(newFD, response.data(), response.length(), 0);
        close(newFD);
    }

    close(sockFD);
    freeaddrinfo(res);

    return 0;
}