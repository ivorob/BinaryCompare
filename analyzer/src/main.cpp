#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <mutex>
#include <string>
#include <stdint.h>
#include <fstream>
#include <unordered_map>
#include <set>

#include "config.h"

#define HIDWORD(x) ((x) >> 32)
#define LODWORD(x) ((x) & 0xffffffff)

namespace {

typedef std::set<size_t> hashes;

std::unordered_map<size_t,std::string> files;
std::unordered_map<uint64_t,hashes> linkage;

std::mutex filesMutex;
std::mutex linkageMutex;

std::unordered_map<uint64_t,hashes>
analyzeFile(SOCKET sockfd, size_t hash, uint32_t fileSize)
{
    std::unordered_map<uint64_t,hashes> linkage;

    uint64_t data = 0;
    uint32_t size = 0;
    uint32_t bytesRead = 0;
    while ((bytesRead = recv(sockfd, (char *)&data, sizeof(data), 0)) == sizeof(data)) {
        size += bytesRead;
        linkage[data].insert(hash);
    }

    if (size != fileSize) {
        throw std::runtime_error("[ERROR]: Cannot read file");
    }

    return linkage;
}

void
printResult(const std::unordered_map<uint64_t,hashes>& linkage)
{
    for (const auto& value : linkage) {
        std::cout << std::hex << LODWORD(value.first) << "  " << HIDWORD(value.first) << " " << value.second.size() << " [";

        int i = 0;
        for (const auto& hash : value.second) {
            if (i++ != 0) {
                std::cout << ", ";
            }

            std::cout << files[hash];
        }

        std::cout << "]" << std::endl;
    }
}

void
merge(const std::unordered_map<uint64_t,hashes>& fileLinkage)
{
    std::lock_guard<std::mutex> lock(linkageMutex);

    for (auto& value : fileLinkage) {
        linkage[value.first].insert(value.second.begin(), value.second.end());
    }
}

void
readFile(SOCKET sockfd)
{
    try {
        uint32_t stringSize = 0;
        if (recv(sockfd, (char *)&stringSize, sizeof(stringSize), 0) != sizeof(stringSize)) {
            throw std::runtime_error("Cannot read filename size");
        }

        size_t hash = 0;
        if (stringSize < 1024) {
            std::unique_ptr<char> buffer(new char[stringSize]);

            if (recv(sockfd, buffer.get(), stringSize, 0) != stringSize) {
                throw std::runtime_error("Cannot read filename");
            }

            std::string filename(buffer.get(), stringSize);
            hash = std::hash<std::string>()(filename);

            std::lock_guard<std::mutex> lock(filesMutex);
            files[hash] = filename;
        }

        uint32_t fileSize = 0;
        if (recv(sockfd, (char *)&fileSize, sizeof(fileSize), 0) != sizeof(fileSize)) {
            throw std::runtime_error("Cannot read file size");
        }

        const auto& fileLinkage = analyzeFile(sockfd, hash, fileSize);
        merge(fileLinkage);
    } catch (const std::exception& e) {
        closesocket(sockfd);
        std::cout << "[ERROR]: " << e.what() << std::endl;
    }
    
    closesocket(sockfd);
}

}

int
main(int argc, char *argv[])
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket < 0) {
        std::cerr << "Cannot create listen socket" << std::endl;
        return -1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(serverPort);
    if (bind(listenSocket, (struct sockaddr *)&address, sizeof(address))) {
        std::cerr << "Cannot bind port" << std::endl;
        closesocket(listenSocket);
        return -1;
    }

    if (listen(listenSocket, SOMAXCONN) < 0) {
        std::cerr << "Cannot listen socket" << std::endl;
        closesocket(listenSocket);
        return -1;
    }

    std::list<std::thread> threads;
    while (1) {
        SOCKET newSocket = accept(listenSocket, nullptr, 0);
        if (newSocket < 0) {
            break;
        }

        uint32_t size = 0;
        if (recv(newSocket, (char *)&size, sizeof(size), MSG_PEEK) != sizeof(size) ||
            size == 0xffffffff)
        {
            break;
        }

        threads.push_back(std::thread(readFile, newSocket));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    printResult(linkage);

    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
