#include <iostream>
#include <thread>
#include <fstream>

#include "SocketManagerImpl.h"
#include "config.h"

IPC::SocketManagerImpl::SocketManagerImpl()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
}

IPC::SocketManagerImpl::~SocketManagerImpl()
{
    WSACleanup();
}

void
IPC::SocketManagerImpl::sendFile(const std::string& filename)
{
    SOCKET sockfd = connectToServer();

    try {
        sendFileName(sockfd, filename);
        sendFileContent(sockfd, filename);
    } catch (const std::exception&) {
        closesocket(sockfd);
        throw;
    }

    closesocket(sockfd);
}

SOCKET
IPC::SocketManagerImpl::connectToServer() const
{
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        throw std::runtime_error("Cannot create socket");
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(serverPort);
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        closesocket(sockfd);
        throw std::runtime_error("Cannot connect to server");
    }

    return sockfd;
}


void
IPC::SocketManagerImpl::sendFileName(SOCKET sockfd, const std::string& filename) const
{
    uint32_t size = static_cast<uint32_t>(filename.size() + 1);
    if (send(sockfd, (const char *)&size, sizeof(size), 0) != sizeof(size)) {
        throw std::runtime_error("Cannot send filename size");
    }

    if (send(sockfd, filename.c_str(), size, 0) != size) {
        throw std::runtime_error("Cannot send filename");
    }
}

void
IPC::SocketManagerImpl::sendFileContent(SOCKET sockfd, const std::string& filename) const
{
    std::ifstream in(filename.c_str(), std::ifstream::binary);
    if (in) {
        in.seekg(0, in.end);
        uint32_t size = static_cast<uint32_t>(in.tellg());
        in.seekg(0, in.beg);

        if (send(sockfd, (const char *)&size, sizeof(size), 0) != sizeof(size)) {
            throw std::runtime_error("Cannot send file size");
        }

        char buffer[1024];
        while (in.read(buffer, sizeof(buffer))) {
            if (send(sockfd, buffer, sizeof(buffer), 0) != sizeof(buffer)) {
                throw std::runtime_error("Cannot send file data");
            }
        }

        if (!in) {
            uint32_t tailSize = static_cast<uint32_t>(in.gcount());
            in.read(buffer, tailSize);
            if (send(sockfd, buffer, tailSize, 0) != tailSize) {
                throw std::runtime_error("Cannot send file data");
            }
        }

        in.close();
    }
}
