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

    for (int i = 0; i < 16; ++i) {
        std::cout << filename << ": " << i << " bytes" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

