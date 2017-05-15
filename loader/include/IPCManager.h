#ifndef __IPC_MANAGER_H__
#define __IPC_MANAGER_H__

#include "AbstractManagerImpl.h"

namespace IPC {

class Manager {
public:
    Manager() = delete;
    Manager(AbstractManagerImpl *impl);
    ~Manager();

    void sendFile(const std::string& filename);

    AbstractManagerImpl& getImpl();
    void setImpl(AbstractManagerImpl *impl);
private:
    AbstractManagerImpl *impl;
};

}

#endif //__IPC_MANAGER_H__
