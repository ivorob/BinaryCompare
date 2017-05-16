#include <iostream>
#include <string>
#include <list>
#include <thread>
#include <future>
#include <winsock2.h>
#include <windows.h>
#include "IPCManager.h"
#include "SocketManagerImpl.h"

namespace {

void
usage(const std::string& appName)
{
    std::cerr << "usage:";
    std::cerr << "\t" << appName << " folder1 [folder2 [folder3 [...]]]" << std::endl;
}

bool
isFileExists(const std::string& filename)
{
    return GetFileAttributes(filename.c_str()) != INVALID_FILE_ATTRIBUTES;
}

inline bool
isValidFolder(const std::string& path)
{
    DWORD fileAttributes = GetFileAttributes(path.c_str());
    return fileAttributes != INVALID_FILE_ATTRIBUTES &&
          (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool
isFileAligned(const std::string& filename)
{
    bool aligned = false;

    HANDLE fileHandle = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        aligned = (GetFileSize(fileHandle, nullptr) % 8) == 0;
        CloseHandle(fileHandle);
    }

    return aligned;
}

void
addFilesToAnalyze(std::list<std::string>& files, const std::string& folder)
{
    WIN32_FIND_DATA findData;

    std::string searchMask = folder + "\\*";
    HANDLE hFind = FindFirstFile(searchMask.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
                std::string currentFile = folder + "\\" + findData.cFileName;
                if (isFileExists(currentFile) && isFileAligned(currentFile)) {
                    files.push_back(currentFile);
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }
}

}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        return -1;
    }

    std::list<std::string> folders;
    for (int i = 1; i < argc; ++i) {
        if (!isValidFolder(argv[i])) {
            std::cerr << "[WARNING]: Folder isn't valid: " << argv[i] << std::endl;
        } else {
            folders.push_back(argv[i]);
        }
    }

    if (folders.empty()) {
        std::cerr << "[ERROR]: There are no valid folders!" << std::endl;
        usage(argv[0]);
        return -1;
    }

    std::list<std::string> files;
    for (const auto& folder : folders) {
        addFilesToAnalyze(files, folder);
    }

    if (files.empty()) {
        std::cerr << "[ERROR]: There no valid files to load!" << std::endl;
    }

    IPC::Manager manager(new IPC::SocketManagerImpl);

    std::list<std::future<void>> threads;
    for (const auto& filename : files) {
        std::future<void> f = std::async(std::launch::async, &IPC::Manager::sendFile, &manager, filename);
        threads.push_back(std::move(f));
    }

    for (auto& future : threads) {
        try {
            future.get(); //generate exception if needed
        } catch (const std::exception& e) {
            std::cerr << "[ERROR]: " << e.what() << std::endl;
        }
    }

    return 0;
}
