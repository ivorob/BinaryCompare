#ifndef __IPC_SOCKET_MANAGER_IMPL_H__
#define __IPC_SOCKET_MANAGER_IMPL_H__

#include "AbstractManagerImpl.h"

namespace IPC {

class SocketManagerImpl : public AbstractManagerImpl {
public:
    SocketManagerImpl();

    void connectToServer() override;
    void sendFile(const std::string& filename) override;
};

}

#endif //__IPC_SOCKET_MANAGER_IMPL_H__
