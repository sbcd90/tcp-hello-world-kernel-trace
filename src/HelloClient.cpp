#include <cstring>
#include <iostream>
#include <string>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Run program as 'program <ipaddress> <port>'\\n";
        return -1;
    }

    auto &ipAddress = argv[1];
    auto &portNum = argv[2];

    addrinfo hints, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int gAddRes = getaddrinfo(ipAddress, portNum, &hints, &p);
    if (gAddRes != 0) {
        std::cerr << gai_strerror(gAddRes) << std::endl;
        return -2;
    }

    if (p == nullptr) {
        std::cerr << "No addresses found\n";
        return -3;
    }

    int sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockFD == -1) {
        std::cerr << "Error while creating socket\n";
        return -4;
    }

    int connectR = connect(sockFD, p->ai_addr, p->ai_addrlen);
    if (connectR == -1) {
        close(sockFD);
        std::cerr << "Error while connecting socket\n";
        return -5;
    }

    std::string reply(15, ' ');

    pid_t pid = getpid();
    std::cout << pid << std::endl;
    auto bytes_recv = recv(sockFD, &reply.front(), reply.size(), 0);
    if (bytes_recv == -1) {
        std::cerr << "Error while receiving bytes\n";
        return -6;
    }

    std::cout << "\n Client received: " << reply << std::endl;
    close(sockFD);
    freeaddrinfo(p);
    return 0;
}