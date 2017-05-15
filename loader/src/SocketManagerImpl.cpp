#include <iostream>
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
}

