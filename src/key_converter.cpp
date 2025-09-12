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
#include "zkey_point_storage.hpp"
#include "zkop_point_storage.hpp"
#include "alt_bn128.hpp"


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

void printPoint(void* point, size_t size) {
    uint8_t* p = reinterpret_cast<uint8_t*>(point);
    std::cout << "[ ";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned)p[i] << " ";
    }

    std::cout << std::dec << "]" << std::endl;
}


// Helper function to compare two G1 points
bool compareG1Points(const AltBn128::G1PointAffine& p1, const AltBn128::G1PointAffine& p2) {
    // Compare x coordinates
    if (memcmp(&p1.x, &p2.x, sizeof(p1.x)) != 0) {
        return false;
    }
    // Compare y coordinates
    if (memcmp(&p1.y, &p2.y, sizeof(p1.y)) != 0) {
        return false;
    }
    return true;
}

// Helper function to compare two G2 points
bool compareG2Points(const AltBn128::G2PointAffine& p1, const AltBn128::G2PointAffine& p2) {
    // Compare x coordinates
    if (memcmp(&p1.x, &p2.x, sizeof(p1.x)) != 0) {
        return false;
    }
    // Compare y coordinates
    if (memcmp(&p1.y, &p2.y, sizeof(p1.y)) != 0) {
        return false;
    }

    return true;
}

// Helper function to check if a G1 point is zero
bool isZeroPoint(const AltBn128::G1PointAffine& point) {
    // A point is zero if both coordinates are zero
    // Check if all bytes are zero
    const uint8_t* x_ptr = reinterpret_cast<const uint8_t*>(&point.x);
    const uint8_t* y_ptr = reinterpret_cast<const uint8_t*>(&point.y);

    // Check x coordinate
    for (size_t i = 0; i < sizeof(point.x); i++) {
        if (x_ptr[i] != 0) return false;
    }

    // Check y coordinate
    for (size_t i = 0; i < sizeof(point.y); i++) {
        if (y_ptr[i] != 0) return false;
    }

    return true;
}

bool isZeroG2Point(const AltBn128::G2PointAffine& point) {
    // A point is zero if both coordinates are zero
    // Check if all bytes are zero
    const uint8_t* x_ptr = reinterpret_cast<const uint8_t*>(&point.x);
    const uint8_t* y_ptr = reinterpret_cast<const uint8_t*>(&point.y);

    // Check x coordinate
    for (size_t i = 0; i < sizeof(point.x); i++) {
        if (x_ptr[i] != 0) return false;
    }

    // Check y coordinate
    for (size_t i = 0; i < sizeof(point.y); i++) {
        if (y_ptr[i] != 0) return false;
    }

    return true;
}

void testG1PointsComparison(BinFileUtils::BinFile& zkey, BinFileUtils::BinFile& zkop,
                          std::unique_ptr<ZKeyUtils::Header>& zkeyHeader, uint32_t sectionIndex) {
    std::cout << "\n=== Testing section " << sectionIndex << " Points Comparison ===" << std::endl;

    try {
        // Check section sizes first
        std::cout << "ZKey section " << sectionIndex << " size: " << MemorySize(zkey.getSectionSize(sectionIndex)) << std::endl;
        std::cout << "ZKop section " << sectionIndex << " size: " << MemorySize(zkop.getSectionSize(sectionIndex)) << std::endl;

        // Create point storage instances
        ZKeyPointStorage<AltBn128::G1PointAffine> zkeyPointStorage(
            zkey.getSectionData(sectionIndex),
            zkey.getSectionSize(sectionIndex)
        );

        ZKopPointStorage<AltBn128::G1PointAffine> zkopPointStorage(
            zkop.getSectionData(sectionIndex),
            zkop.getSectionSize(sectionIndex)
        );

        uint32_t totalPoints = zkeyHeader->nVars;
        std::cout << "Total points to compare: " << totalPoints << std::endl;
        std::cout << "ZKey storage reports: " << zkeyPointStorage.getPointCount() << " points" << std::endl;
        std::cout << "ZKop storage reports: " << zkopPointStorage.getPointCount() << " points" << std::endl;

        uint32_t matchingPoints = 0;
        uint32_t differentPoints = 0;
        uint32_t zkeyZeroPoints = 0;
        uint32_t zkopZeroPoints = 0;
        uint32_t firstDifferenceIndex = UINT32_MAX;
        uint32_t accessErrors = 0;

        // Verify point counts match
        if (zkeyPointStorage.getPointCount() != zkopPointStorage.getPointCount()) {
            std::cerr << "ERROR: Point counts don't match!" << std::endl;
            std::cerr << "ZKey: " << zkeyPointStorage.getPointCount()
                      << ", ZKop: " << zkopPointStorage.getPointCount() << std::endl;
        }

        // Use minimum count for safety
        uint32_t comparePoints = std::min({totalPoints,
                                         zkeyPointStorage.getPointCount(),
                                         zkopPointStorage.getPointCount()});

        std::cout << "Actually comparing: " << comparePoints << " points" << std::endl;

        // Compare all points
        for (uint32_t i = 0; i < comparePoints; i++) {
            try {
                AltBn128::G1PointAffine zkeyPoint = zkeyPointStorage.get(i);
                AltBn128::G1PointAffine zkopPoint = zkopPointStorage.get(i);

                bool zkeyIsZero = isZeroPoint(zkeyPoint);
                bool zkopIsZero = isZeroPoint(zkopPoint);

                if (zkeyIsZero) zkeyZeroPoints++;
                if (zkopIsZero) zkopZeroPoints++;

                if (compareG1Points(zkeyPoint, zkopPoint)) {
                    matchingPoints++;
                } else {
                    differentPoints++;
                    if (firstDifferenceIndex == UINT32_MAX) {
                        firstDifferenceIndex = i;
                    }

                    // Print details for first few differences
                    if (differentPoints <= 10) {
                        std::cout << "Difference at index " << i << ":" << std::endl;
                        printPoint(&zkeyPoint, sizeof(zkeyPoint));
                        printPoint(&zkopPoint, sizeof(zkopPoint));

                        if (!zkeyIsZero && !zkopIsZero) {
                            // Show first few bytes of each coordinate for non-zero points
                            const uint8_t* zkey_x = reinterpret_cast<const uint8_t*>(&zkeyPoint.x);
                            const uint8_t* zkop_x = reinterpret_cast<const uint8_t*>(&zkopPoint.x);
                            std::cout << "  ZKey x[0-3]: " << std::hex;
                            for (int j = 0; j < 4 && j < sizeof(zkeyPoint.x); j++) {
                                std::cout << std::setfill('0') << std::setw(2) << (unsigned)zkey_x[j] << " ";
                            }
                            std::cout << std::endl;
                            std::cout << "  ZKop x[0-3]: " << std::hex;
                            for (int j = 0; j < 4 && j < sizeof(zkopPoint.x); j++) {
                                std::cout << std::setfill('0') << std::setw(2) << (unsigned)zkop_x[j] << " ";
                            }
                            std::cout << std::dec << std::endl; // Reset to decimal
                        }
                    }
                }

                // Progress indicator for large datasets
                if (comparePoints > 1000 && (i + 1) % (comparePoints / 10) == 0) {
                    std::cout << "Progress: " << ((i + 1) * 100 / comparePoints) << "%" << std::endl;
                }

            } catch (const std::exception& e) {
                accessErrors++;
                std::cerr << "Error accessing point " << i << ": " << e.what() << std::endl;
                if (accessErrors <= 5) { // Don't spam too many errors
                    std::cerr << "  This counts as a difference." << std::endl;
                }
                differentPoints++;
            }
        }

        // Print results
        std::cout << "\n=== Points Comparison Results ===" << std::endl;
        std::cout << "Total points compared: " << comparePoints << std::endl;
        std::cout << "Matching points: " << matchingPoints << std::endl;
        std::cout << "Different points: " << differentPoints << std::endl;
        std::cout << "Access errors: " << accessErrors << std::endl;
        std::cout << "ZKey zero points: " << zkeyZeroPoints << std::endl;
        std::cout << "ZKop zero points: " << zkopZeroPoints << std::endl;

        if (firstDifferenceIndex != UINT32_MAX) {
            std::cout << "First difference at index: " << firstDifferenceIndex << std::endl;
        }

        double matchPercentage = (comparePoints > 0) ? (double(matchingPoints) / comparePoints * 100.0) : 0.0;
        std::cout << "Match percentage: " << std::fixed << std::setprecision(2) << matchPercentage << "%" << std::endl;

        if (matchingPoints == comparePoints && accessErrors == 0) {
            std::cout << "✓ SUCCESS: All A points match between zkey and zkop files!" << std::endl;
        } else {
            std::cout << "✗ WARNING: " << differentPoints << " A points differ between files!" << std::endl;
            if (accessErrors > 0) {
                std::cout << "✗ ERROR: " << accessErrors << " access errors occurred!" << std::endl;
            }
        }

        // Additional statistics
        std::cout << "\n=== Additional Statistics ===" << std::endl;
        std::cout << "Zero point ratio in ZKey: " << std::fixed << std::setprecision(2)
                  << (comparePoints > 0 ? double(zkeyZeroPoints) / comparePoints * 100.0 : 0.0) << "%" << std::endl;
        std::cout << "Zero point ratio in ZKop: " << std::fixed << std::setprecision(2)
                  << (comparePoints > 0 ? double(zkopZeroPoints) / comparePoints * 100.0 : 0.0) << "%" << std::endl;

        // Compression statistics
        if (zkeyZeroPoints > 0) {
            uint64_t originalSize = comparePoints * sizeof(AltBn128::G1PointAffine);
            uint64_t compressedSize = (comparePoints - zkeyZeroPoints) * sizeof(AltBn128::G1PointAffine) +
                                    4 + // point count
                                    ((comparePoints + 7) / 8); // bit mask
            double compressionRatio = double(compressedSize) / originalSize;
            std::cout << "Estimated compression ratio: " << std::fixed << std::setprecision(3)
                      << compressionRatio << " (" << (100.0 * (1.0 - compressionRatio)) << "% savings)" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error during points comparison: " << e.what() << std::endl;
    }
}

void testG2PointsComparison(BinFileUtils::BinFile& zkey, BinFileUtils::BinFile& zkop,
                          std::unique_ptr<ZKeyUtils::Header>& zkeyHeader, uint32_t sectionIndex) {
    std::cout << "\n=== Testing section "<< sectionIndex <<" Points Comparison ===" << std::endl;

    try {
        // Check section sizes first
        std::cout << "ZKey section " << sectionIndex << " size: " << MemorySize(zkey.getSectionSize(sectionIndex)) << std::endl;
        std::cout << "ZKop section " << sectionIndex << " size: " << MemorySize(zkop.getSectionSize(sectionIndex)) << std::endl;

        // Create point storage instances
        ZKeyPointStorage<AltBn128::G2PointAffine> zkeyPointStorage(
            zkey.getSectionData(sectionIndex),
            zkey.getSectionSize(sectionIndex)
        );

        ZKopPointStorage<AltBn128::G2PointAffine> zkopPointStorage(
            zkop.getSectionData(sectionIndex),
            zkop.getSectionSize(sectionIndex)
        );

        uint32_t totalPoints = zkeyHeader->nVars;
        std::cout << "Total points to compare: " << totalPoints << std::endl;
        std::cout << "ZKey storage reports: " << zkeyPointStorage.getPointCount() << " points" << std::endl;
        std::cout << "ZKop storage reports: " << zkopPointStorage.getPointCount() << " points" << std::endl;

        uint32_t matchingPoints = 0;
        uint32_t differentPoints = 0;
        uint32_t zkeyZeroPoints = 0;
        uint32_t zkopZeroPoints = 0;
        uint32_t firstDifferenceIndex = UINT32_MAX;
        uint32_t accessErrors = 0;

        // Verify point counts match
        if (zkeyPointStorage.getPointCount() != zkopPointStorage.getPointCount()) {
            std::cerr << "ERROR: Point counts don't match!" << std::endl;
            std::cerr << "ZKey: " << zkeyPointStorage.getPointCount()
                      << ", ZKop: " << zkopPointStorage.getPointCount() << std::endl;
        }

        // Use minimum count for safety
        uint32_t comparePoints = std::min({totalPoints,
                                         zkeyPointStorage.getPointCount(),
                                         zkopPointStorage.getPointCount()});

        std::cout << "Actually comparing: " << comparePoints << " points" << std::endl;

        // Compare all points
        for (uint32_t i = 0; i < comparePoints; i++) {
            try {
                AltBn128::G2PointAffine zkeyPoint = zkeyPointStorage.get(i);
                AltBn128::G2PointAffine zkopPoint = zkopPointStorage.get(i);

                bool zkeyIsZero = isZeroG2Point(zkeyPoint);
                bool zkopIsZero = isZeroG2Point(zkopPoint);

                if (zkeyIsZero) zkeyZeroPoints++;
                if (zkopIsZero) zkopZeroPoints++;

                if (compareG2Points(zkeyPoint, zkopPoint)) {
                    matchingPoints++;
                } else {
                    differentPoints++;
                    if (firstDifferenceIndex == UINT32_MAX) {
                        firstDifferenceIndex = i;
                    }

                    // Print details for first few differences
                    if (differentPoints <= 10) {
                        std::cout << "Difference at index " << i << ":" << std::endl;
                        printPoint(&zkeyPoint, sizeof(zkeyPoint));
                        printPoint(&zkopPoint, sizeof(zkopPoint));

                        if (!zkeyIsZero && !zkopIsZero) {
                            // Show first few bytes of each coordinate for non-zero points
                            const uint8_t* zkey_x = reinterpret_cast<const uint8_t*>(&zkeyPoint.x);
                            const uint8_t* zkop_x = reinterpret_cast<const uint8_t*>(&zkopPoint.x);
                            std::cout << "  ZKey x[0-3]: " << std::hex;
                            for (int j = 0; j < 4 && j < sizeof(zkeyPoint.x); j++) {
                                std::cout << std::setfill('0') << std::setw(2) << (unsigned)zkey_x[j] << " ";
                            }
                            std::cout << std::endl;
                            std::cout << "  ZKop x[0-3]: " << std::hex;
                            for (int j = 0; j < 4 && j < sizeof(zkopPoint.x); j++) {
                                std::cout << std::setfill('0') << std::setw(2) << (unsigned)zkop_x[j] << " ";
                            }
                            std::cout << std::dec << std::endl; // Reset to decimal
                        }
                    }
                }

                // Progress indicator for large datasets
                if (comparePoints > 1000 && (i + 1) % (comparePoints / 10) == 0) {
                    std::cout << "Progress: " << ((i + 1) * 100 / comparePoints) << "%" << std::endl;
                }

            } catch (const std::exception& e) {
                accessErrors++;
                std::cerr << "Error accessing point " << i << ": " << e.what() << std::endl;
                if (accessErrors <= 5) { // Don't spam too many errors
                    std::cerr << "  This counts as a difference." << std::endl;
                }
                differentPoints++;
            }
        }

        // Print results
        std::cout << "\n=== Points Comparison Results ===" << std::endl;
        std::cout << "Total points compared: " << comparePoints << std::endl;
        std::cout << "Matching points: " << matchingPoints << std::endl;
        std::cout << "Different points: " << differentPoints << std::endl;
        std::cout << "Access errors: " << accessErrors << std::endl;
        std::cout << "ZKey zero points: " << zkeyZeroPoints << std::endl;
        std::cout << "ZKop zero points: " << zkopZeroPoints << std::endl;

        if (firstDifferenceIndex != UINT32_MAX) {
            std::cout << "First difference at index: " << firstDifferenceIndex << std::endl;
        }

        double matchPercentage = (comparePoints > 0) ? (double(matchingPoints) / comparePoints * 100.0) : 0.0;
        std::cout << "Match percentage: " << std::fixed << std::setprecision(2) << matchPercentage << "%" << std::endl;

        if (matchingPoints == comparePoints && accessErrors == 0) {
            std::cout << "✓ SUCCESS: All A points match between zkey and zkop files!" << std::endl;
        } else {
            std::cout << "✗ WARNING: " << differentPoints << " A points differ between files!" << std::endl;
            if (accessErrors > 0) {
                std::cout << "✗ ERROR: " << accessErrors << " access errors occurred!" << std::endl;
            }
        }

        // Additional statistics
        std::cout << "\n=== Additional Statistics ===" << std::endl;
        std::cout << "Zero point ratio in ZKey: " << std::fixed << std::setprecision(2)
                  << (comparePoints > 0 ? double(zkeyZeroPoints) / comparePoints * 100.0 : 0.0) << "%" << std::endl;
        std::cout << "Zero point ratio in ZKop: " << std::fixed << std::setprecision(2)
                  << (comparePoints > 0 ? double(zkopZeroPoints) / comparePoints * 100.0 : 0.0) << "%" << std::endl;

        // Compression statistics
        if (zkeyZeroPoints > 0) {
            uint64_t originalSize = comparePoints * sizeof(AltBn128::G1PointAffine);
            uint64_t compressedSize = (comparePoints - zkeyZeroPoints) * sizeof(AltBn128::G1PointAffine) +
                                    4 + // point count
                                    ((comparePoints + 7) / 8); // bit mask
            double compressionRatio = double(compressedSize) / originalSize;
            std::cout << "Estimated compression ratio: " << std::fixed << std::setprecision(3)
                      << compressionRatio << " (" << (100.0 * (1.0 - compressionRatio)) << "% savings)" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error during A points comparison: " << e.what() << std::endl;
    }
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
        ZKeyUtils::savePointsG1ZeroMask(writer, zkey.getSectionData(5),  zkeyHeader->nVars, 5); // pointsA
        std::cout << "Points B1" << std::endl;
        ZKeyUtils::savePointsG1ZeroMask(writer, zkey.getSectionData(6),  zkeyHeader->nVars, 6); // PointsB1
        std::cout << "Points B2(G2)" << std::endl;
        ZKeyUtils::savePointsG2ZeroMask(writer, zkey.getSectionData(7),  zkeyHeader->nVars, 7); // PointsB2
        std::cout << "Points C" << std::endl;
        ZKeyUtils::savePointsG1ZeroMask(writer, zkey.getSectionData(8),  zkeyHeader->nVars, 8); // PointsC
        std::cout << "Points H1" << std::endl;
        ZKeyUtils::savePointsG1ZeroMask(writer, zkey.getSectionData(9),  zkeyHeader->domainSize, 9); // PointsH1
        std::cout << std::endl;
        writer.writeToFile(zkopFilename);

        std::unique_ptr<BinFileUtils::BinFile> zkeyOp = BinFileUtils::openExisting(zkopFilename, {"zkop"}, 1);
        std::cout << "Optimized file" << std::endl;
        printInfo(zkeyOp.get());

        // Test A points comparison
        //testG1PointsComparison(zkey, *zkeyOp, zkeyHeader, 5);
        // Test B points comparison
        //testG1PointsComparison(zkey, *zkeyOp, zkeyHeader, 6);
        // Test B2 points comparison
        //testG2PointsComparison(zkey, *zkeyOp, zkeyHeader, 7);
        // Test C points comparison
        //testG1PointsComparison(zkey, *zkeyOp, zkeyHeader, 8);
        // Test H1 points comparison
        //testG1PointsComparison(zkey, *zkeyOp, zkeyHeader, 9);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;

    }

    exit(EXIT_SUCCESS);
}
