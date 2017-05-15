#include <iostream>
#include "SocketManagerImpl.h"

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
    std::cout << "sending file: " << filename << std::endl;
}

