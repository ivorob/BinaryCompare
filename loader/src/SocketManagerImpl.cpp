#include <iostream>
#include <thread>
#include "SocketManagerImpl.h"
#include "config.h"

IPC::SocketManagerImpl::SocketManagerImpl()
{
}

void
IPC::SocketManagerImpl::connectToServer()
{
}

void
IPC::SocketManagerImpl::sendFile(const std::string& filename)
{
    std::cout << "sending file: " << filename << " to port: " << serverPort << std::endl;

    static int a = 3;
    for (int i = 0; i < 16; ++i) {
        std::cout << filename << ": " << i << " bytes" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if ((i % 3) == 0 && a == 3) {
            a = 4;
            throw std::runtime_error("Cannot send file: " + filename);
        }
    }
}

