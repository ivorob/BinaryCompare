CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT (loader)

SET (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET (INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
SET (COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../common/)

SET (SOURCES
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/IPCManager.cpp
        ${SRC_DIR}/SocketManagerImpl.cpp)

INCLUDE_DIRECTORIES (${INCLUDE_DIR} ${COMMON_DIR})

IF (WIN32)
    ADD_DEFINITIONS (-D_WINSOCK_DEPRECATED_NO_WARNINGS)
ENDIF ()

ADD_EXECUTABLE (loader ${SOURCES})

IF (WIN32)
    TARGET_LINK_LIBRARIES (loader ws2_32)
ENDIF (WIN32)
