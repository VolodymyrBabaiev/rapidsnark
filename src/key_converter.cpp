#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include "fileloader.hpp"
#include "binfile_utils.hpp"
#include "fileloader.hpp"
#include "binfile_writer.hpp"
#include "zkey_utils.hpp"
#include <iomanip>
#include <sstream>


class MemorySize {
private:
    u_int64_t bytes;

public:
    explicit MemorySize(u_int64_t b) : bytes(b) {}

    // Accessor for raw bytes if needed
    u_int64_t getBytes() const { return bytes; }

    // Convert to human readable string
    std::string toHumanReadable() const {
        static const char* suffixes[] = {"bytes", "KBytes", "MBytes", "GBytes", "TBytes", "PBytes"};
        int i = 0;
        double count = static_cast<double>(bytes);

        while (count >= 1024 && i < 5) {
            count /= 1024;
            ++i;
        }

        std::ostringstream out;
        out << std::fixed << std::setprecision((count < 10 && i > 0) ? 1 : 0)
            << count << " " << suffixes[i];
        return out.str();
    }

    // Friend function to allow std::cout << MemorySize
    friend std::ostream& operator<<(std::ostream& os, const MemorySize& fs) {
        return os << fs.toHumanReadable();
    }
};

void printInfo(BinFileUtils::BinFile *f) {
    std::string type = f->getType();
    std::cout << "File type: " << type << std::endl;
    std::cout << "File size: " << MemorySize(f->getDataSize()) << std::endl;
    std::cout << "Section 2 (Header): " << MemorySize(f->getSectionSize(2)) << std::endl;
    if (type == "zkey")
        std::cout << "Section 3 (IC): " << MemorySize(f->getSectionSize(3)) << std::endl;
    std::cout << "Section 4 (Coefs): " << MemorySize(f->getSectionSize(4)) << std::endl;
    std::cout << "Section 5 (PointsA): " << MemorySize(f->getSectionSize(5)) << std::endl;
    std::cout << "Section 6 (PointsB1): " << MemorySize(f->getSectionSize(6)) << std::endl;
    std::cout << "Section 7 (PointsB2(G2)): " << MemorySize(f->getSectionSize(7)) << std::endl;
    std::cout << "Section 8 (PointsC): " << MemorySize(f->getSectionSize(8)) << std::endl;
    std::cout << "Section 9 (PointsH1): " << MemorySize(f->getSectionSize(9)) << std::endl;
    if (type == "zkey")
        std::cout << "Section 10 (Credentials): " << MemorySize(f->getSectionSize(10)) << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Invalid number of parameters" << std::endl;
        std::cerr << "Usage: key_converter <circuit.zkey> <circuit.zkop>" << std::endl;

        return EXIT_FAILURE;
    }

    try {
        const std::string zkeyFilename = argv[1];
        const std::string zkopFilename = argv[2];

        BinFileUtils::BinFile zkey(zkeyFilename, {"zkey"}, 1);
        std::unique_ptr<ZKeyUtils::Header> zkeyHeader = ZKeyUtils::loadHeader(&zkey);

        std::cout << "Original file" << std::endl;
        printInfo(&zkey);
        std::cout << std::endl;

        std::cout << "Domains : " << zkeyHeader->domainSize << std::endl;
        std::cout << "Coef : " << zkeyHeader->nCoefs << std::endl;

        BinFileUtils::BinFileWriter writer("zkop", 1);

        // Save header untouched
        std::cout << "Optimization" << std::endl;
        ZKeyUtils::copyHeader(writer , zkey); // Header
        ZKeyUtils::optimizeCoefs(writer, zkey.getSectionData(4),  zkey.getSectionSize(4), 4); // Coefs
        std::cout << "Points A" << std::endl;
        ZKeyUtils::savePointsG1(writer, zkey.getSectionData(5),  zkeyHeader->nVars, 5); // pointsA
        std::cout << "Points B1" << std::endl;
        ZKeyUtils::savePointsG1(writer, zkey.getSectionData(6),  zkeyHeader->nVars, 6); // PointsB1
        std::cout << "Points B2(G2)" << std::endl;
        ZKeyUtils::savePointsG2(writer, zkey.getSectionData(7),  zkeyHeader->nVars, 7); // PointsB2
        std::cout << "Points C" << std::endl;
        ZKeyUtils::savePointsG1(writer, zkey.getSectionData(8),  zkeyHeader->nVars, 8); // PointsC
        std::cout << "Points H1" << std::endl;
        ZKeyUtils::savePointsG1(writer, zkey.getSectionData(9),  zkeyHeader->domainSize, 9); // PointsH1
        std::cout << std::endl;
        writer.writeToFile(zkopFilename);

        std::unique_ptr<BinFileUtils::BinFile> zkeyOp = BinFileUtils::openExisting(zkopFilename, {"zkop"}, 1);
        std::cout << "Optimized file" << std::endl;
        printInfo(zkeyOp.get());

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;

    }

    exit(EXIT_SUCCESS);
}
