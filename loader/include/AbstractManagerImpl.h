#ifndef __ABSTRACT_IPC_MANAGER_IMPL_H__
#define __ABSTRACT_IPC_MANAGER_IMPL_H__

#include <string>

namespace IPC {

class AbstractManagerImpl {
public:
    virtual ~AbstractManagerImpl() = default;

    virtual void connectToServer() = 0;
    virtual void sendFile(const std::string& filename) = 0;
};

}

#endif //__ABSTRACT_IPC_MANAGER_IMPL_H__
