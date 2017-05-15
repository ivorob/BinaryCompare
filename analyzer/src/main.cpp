#include <iostream>
#include <string>
#include <stdint.h>
#include <fstream>
#include <unordered_map>
#include <set>

#define HIDWORD(x) ((x) >> 32)
#define LODWORD(x) ((x) & 0xffffffff)

namespace {

typedef std::set<size_t> hashes;

std::unordered_map<size_t,std::string> files;
std::unordered_map<uint64_t,hashes> linkage;

const std::string filename1 = "C:\\projects\\dz\\data\\file1.bin";
const std::string filename2 = "C:\\projects\\dz\\data\\file2.bin";

void
analyzeFile(const std::string& filename, size_t hash)
{
    std::ifstream in(filename, std::ios::binary);
    uint64_t data = 0;
    while (in.good() && !in.eof()) { 
        in.read((char *)&data, sizeof(data));
        linkage[data].insert(hash);
    }

    in.close();
}

void
printResult(const std::unordered_map<uint64_t,hashes>& linkage)
{
    for (const auto& value : linkage) {
        std::cout << std::hex << LODWORD(value.first) << "  " << HIDWORD(value.first) << " " << value.second.size() << " [";

        int i = 0;
        for (const auto& hash : value.second) {
            if (i++ != 0) {
                std::cout << ", ";
            }

            std::cout << files[hash];
        }

        std::cout << "]" << std::endl;
    }
}

}

int
main(int argc, char *argv[])
{
    files[std::hash<std::string>()(filename1)] = filename1;
    files[std::hash<std::string>()(filename2)] = filename2;

    for (const auto& file : files) {
        analyzeFile(file.second, file.first);
    }

    printResult(linkage);

    return 0;
}
