#ifndef __IPC_SOCKET_MANAGER_IMPL_H__
#define __IPC_SOCKET_MANAGER_IMPL_H__

#include <winsock2.h>
#include <windows.h>

#include "AbstractManagerImpl.h"

namespace IPC {

class SocketManagerImpl : public AbstractManagerImpl {
public:
    SocketManagerImpl();
    ~SocketManagerImpl();

    void sendFile(const std::string& filename) override;
private:
    SOCKET connectToServer() const;

    void sendFileName(SOCKET sockfd, const std::string& filename) const;
    void sendFileContent(SOCKET sockfd, const std::string& filename) const;
};

}

#endif //__IPC_SOCKET_MANAGER_IMPL_H__
