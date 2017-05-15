#include "IPCManager.h"

IPC::Manager::Manager(AbstractManagerImpl *impl)
    : impl(impl)
{
}

IPC::Manager::~Manager()
{
    delete this->impl;
}

IPC::AbstractManagerImpl&
IPC::Manager::getImpl()
{
    return *this->impl;
}

void
IPC::Manager::setImpl(AbstractManagerImpl *impl)
{
    delete this->impl;
    this->impl = impl;
}

void
IPC::Manager::sendFile(const std::string& filename)
{
    getImpl().sendFile(filename);
}
