#include <stdexcept>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "zkey_utils.hpp"
#include "alt_bn128.hpp"
#include "groth16.hpp"
#include "zkop_point_storage.hpp"


namespace ZKeyUtils {


Header::Header() {
    mpz_init(qPrime);
    mpz_init(rPrime);
}

Header::~Header() {
    mpz_clear(qPrime);
    mpz_clear(rPrime);
}


std::unique_ptr<Header> loadHeader(BinFileUtils::BinFile *f) {

    std::unique_ptr<Header> h(new Header());

    f->startReadSection(1);
    uint32_t protocol = f->readU32LE();
    if (protocol != 1) {
        throw std::invalid_argument( "zkey file is not groth16" );
    }
    f->endReadSection();

    f->startReadSection(2);

    h->n8q = f->readU32LE();
    mpz_import(h->qPrime, h->n8q, -1, 1, -1, 0, f->read(h->n8q));

    h->n8r = f->readU32LE();
    mpz_import(h->rPrime, h->n8r , -1, 1, -1, 0, f->read(h->n8r));

    h->nVars = f->readU32LE();
    h->nPublic = f->readU32LE();
    h->domainSize = f->readU32LE();

    h->vk_alpha1 = f->read(h->n8q*2);
    h->vk_beta1 = f->read(h->n8q*2);
    h->vk_beta2 = f->read(h->n8q*4);
    h->vk_gamma2 = f->read(h->n8q*4);
    h->vk_delta1 = f->read(h->n8q*2);
    h->vk_delta2 = f->read(h->n8q*4);
    f->endReadSection();

    f->startReadSection(4);
    h->nCoefs = f->readU32LE();
    f->endReadSection(false);

    return h;
}

bool isZero(uint8_t *data) {
    for (int i = 0; i < 32; i++) {
        if (data[i] != 0)
            return false;
    }
    return true;
}

void beToLeCopy32(uint8_t *src, uint8_t *dst) {
    for (int i = 0; i < 32; i++) {
        dst[i] = src[31 - i];
    }
}

void savePointsG1(BinFileUtils::BinFileWriter &writer, void *data, uint32_t nPoints, uint32_t section) {
    writer.startSection(section);

    uint8_t zero_mask = 0x80;
    uint8_t buf[32];
    int zeroX = 0;
    int zeroY = 0;

    for (int i = 0; i < nPoints; i++) {
        uint8_t* p1 = static_cast<uint8_t*>(data) + i * 64;
        uint8_t* p2 = static_cast<uint8_t*>(data) + i * 64 + 32;

        if (!isZero(p1)) {
            beToLeCopy32(p1, buf);
            writer.write(buf, 32);
        } else {
            zeroX++;
            writer.writeU8(zero_mask);
        }

        if (!isZero(p2)) {
            beToLeCopy32(p2, buf);
            writer.write(buf, 32);
        } else {
            zeroY++;
            writer.writeU8(zero_mask);
        }
    }

    std::cout<<"X zero num : "<< zeroX << " (" << (static_cast<double>(zeroX) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"Y zero num : "<< zeroY << " (" << (static_cast<double>(zeroY) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"Points num : "<< nPoints << std::endl;

    writer.endSection();
}

void printPoint(void* point, size_t size) {
    uint8_t* p = reinterpret_cast<uint8_t*>(point);
    std::cout << "[ ";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned)p[i] << " ";
    }

    std::cout << std::dec << "]" << std::endl;
}

void savePointsG1ZeroMask(BinFileUtils::BinFileWriter &writer, void *data, uint32_t nPoints, uint32_t section) {
    writer.startSection(section);

    MemoryEfficientZeroTracker zeroTracker(nPoints);
    int zeroNum = 0;

    for (int i = 0; i < nPoints; i++) {
        uint8_t* p1 = static_cast<uint8_t*>(data) + i * 64;
        uint8_t* p2 = static_cast<uint8_t*>(data) + i * 64 + 32;

        if (isZero(p1) && isZero(p2)) {
            zeroNum++;
            zeroTracker.setBit(i, 1);
        }
    }

    zeroTracker.buildCache();

    writer.writeU32LE(nPoints);
    std::vector<uint8_t> zeroMask = zeroTracker.getBitArray();
    for (int i = 0; i < zeroMask.size(); i++) {
        writer.writeU8(zeroMask[i]);
    }

    for (int i = 0; i < nPoints; i++) {
        if (!zeroTracker.isZeroAt(i)) {
            uint8_t* p1 = static_cast<uint8_t*>(data) + i * 64;
            uint8_t* p2 = static_cast<uint8_t*>(data) + i * 64 + 32;

            writer.write(p1, 32);
            writer.write(p2, 32);
        }
    }

    std::cout<<"Zero point num : "<< zeroNum << " (" << (static_cast<double>(zeroNum) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"Points num : "<< nPoints << std::endl;

    writer.endSection();
}

void savePointsG2(BinFileUtils::BinFileWriter &writer, void *data,  uint32_t nPoints, uint32_t section) {
    writer.startSection(section);

    uint8_t zero_mask = 0x80;
    uint8_t buf[32];
    int zeron[4] = {0, 0, 0, 0};

    for (int i = 0; i < nPoints * 4; i++) {
        uint8_t* p = static_cast<uint8_t*>(data) + i * 32;

        if (!isZero(p)) {
            beToLeCopy32(p, buf);
            writer.write(buf, 32);
        } else {
            zeron[i % 4]++;
            writer.writeU8(zero_mask);
        }
    }

    std::cout<<"a.x zero num : "<< zeron[0] << " (" << (static_cast<double>(zeron[0]) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"a.y zero num : "<< zeron[1] << " (" << (static_cast<double>(zeron[1]) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"b.x zero num : "<< zeron[2] << " (" << (static_cast<double>(zeron[2]) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"b.y zero num : "<< zeron[3] << " (" << (static_cast<double>(zeron[3]) / nPoints * 100) << "%)"<< std::endl;

    std::cout<<"Points num : "<< nPoints << std::endl;

    writer.endSection();
}

void savePointsG2ZeroMask(BinFileUtils::BinFileWriter &writer, void *data, uint32_t nPoints, uint32_t section) {
    writer.startSection(section);

    MemoryEfficientZeroTracker zeroTracker(nPoints);
    int zeroNum = 0;

    for (int i = 0; i < nPoints; i++) {
        uint8_t* p1 = static_cast<uint8_t*>(data) + i * 32 * 4;
        uint8_t* p2 = p1 + 32;
        uint8_t* p3 = p1 + 64;
        uint8_t* p4 = p1 + 96;

        if (isZero(p1) && isZero(p2) && isZero(p3) && isZero(p4)) {
            zeroNum++;
            zeroTracker.setBit(i, 1);
        }
    }

    zeroTracker.buildCache();

    writer.writeU32LE(nPoints);
    std::vector<uint8_t> zeroMask = zeroTracker.getBitArray();
    for (int i = 0; i < zeroMask.size(); i++) {
        writer.writeU8(zeroMask[i]);
    }

    for (int i = 0; i < nPoints; i++) {
        if (!zeroTracker.isZeroAt(i)) {
            uint8_t* p = static_cast<uint8_t*>(data) + i * 128;

            writer.write(p, 128);
        }
    }

    std::cout<<"Zero point num : "<< zeroNum << " (" << (static_cast<double>(zeroNum) / nPoints * 100) << "%)"<< std::endl;
    std::cout<<"Points num : "<< nPoints << std::endl;

    writer.endSection();
}

void copySection(BinFileUtils::BinFileWriter &writer, void *data,  uint32_t size, uint32_t section) {
    writer.startSection(section);
    writer.write(data, size);
    writer.endSection();
}

void copyHeader(BinFileUtils::BinFileWriter &writer, BinFileUtils::BinFile &binFile) {
    copySection(writer, binFile.getSectionData(1), binFile.getSectionSize(1), 1);
    copySection(writer, binFile.getSectionData(2), binFile.getSectionSize(2), 2);
}

void saveCoefs(BinFileUtils::BinFileWriter &writer, void *data,  uint32_t size, uint32_t section) {
    writer.startSection(section);
    writer.write(data, size);
    writer.endSection();
}

void setZero32(uint8_t *data) {
    for (int i = 0; i < 32; i++) {
        data[i] = 0;
    }
}

void readPointsG1(BinFileUtils::BinFile& binFile, void *data, uint32_t nPoints, uint32_t section) {
    // Get direct access to section data
    uint8_t* sectionData = static_cast<uint8_t*>(binFile.getSectionData(section));
    uint64_t sectionSize = binFile.getSectionSize(section);

    // Cast the output data pointer to uint32_t for easier access
    uint8_t* pointData = static_cast<uint8_t*>(data);

    uint8_t zero_mask = 0x80;
    uint64_t pos = 0;  // Position in section data

    for (int i = 0; i < nPoints; i++) {
        // Read first component (x)
        if (pos >= sectionSize) {
            throw std::runtime_error("Unexpected end of section data while reading G1 points");
        }

        if (sectionData[pos] == zero_mask) {
            // This is a zero value
            setZero32(pointData);
            pointData += 32;
            pos++;
        } else {
            // This is part of a uint32_t, need to read 4 bytes total
            if (pos + 32 > sectionSize) {
                throw std::runtime_error("Unexpected end of section data while reading G1 point component");
            }

            beToLeCopy32(sectionData + pos, pointData);
            pointData += 32;
            pos += 32;
        }

        // Read second component (y)
        if (pos >= sectionSize) {
            throw std::runtime_error("Unexpected end of section data while reading G1 points");
        }

        if (sectionData[pos] == zero_mask) {
            // This is a zero value
            setZero32(pointData);
            pointData += 32;
            pos++;
        } else {
            // This is part of a uint32_t, need to read 4 bytes total
            if (pos + 32 > sectionSize) {
                throw std::runtime_error("Unexpected end of section data while reading G1 point component");
            }

            beToLeCopy32(sectionData + pos, pointData);
            pointData += 32;
            pos += 32;
        }
    }
}

void readPointsG2(BinFileUtils::BinFile& binFile, void *data, uint32_t nPoints, uint32_t section) {
    // Get direct access to section data
    uint8_t* sectionData = static_cast<uint8_t*>(binFile.getSectionData(section));
    uint64_t sectionSize = binFile.getSectionSize(section);

    // Cast the output data pointer to uint32_t for easier access
    uint8_t* pointData = static_cast<uint8_t*>(data);

    uint8_t zero_mask = 0x80;
    uint64_t pos = 0;  // Position in section data

    // G2 points have 4 components: x.a, x.b, y.a, y.b
    for (int i = 0; i < nPoints*4; i++) {
        if (pos >= sectionSize) {
            throw std::runtime_error("Unexpected end of section data while reading G2 points");
        }

        if (sectionData[pos] == zero_mask) {
            // This is a zero value
            setZero32(pointData);
            pointData += 32;
            pos++;
        } else {
            // This is part of a uint32_t, need to read 4 bytes total
            if (pos + 32 > sectionSize) {
                throw std::runtime_error("Unexpected end of section data while reading G2 point component");
            }

            beToLeCopy32(sectionData + pos, pointData);
            pointData += 32;
            pos += 32;
        }
    }
}

struct Coef {
    uint32_t m;
    uint32_t c;
    uint32_t s;
    uint8_t data[32];
};

struct coef_opt {
    uint32_t c;
    uint32_t s;
};

// Using std::array for the key - no dynamic allocation needed
using Coef_t = std::array<uint8_t, 32>;

// Alternative hash function using FNV-1a algorithm (faster)
struct CoefHashFNV {
    std::size_t operator()(const Coef_t& key) const noexcept {
        const std::size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
        const std::size_t FNV_PRIME = 1099511628211ULL;

        std::size_t hash = FNV_OFFSET_BASIS;
        for (uint8_t byte : key) {
            hash ^= byte;
            hash *= FNV_PRIME;
        }
        return hash;
    }
};

void optimizeCoefs(BinFileUtils::BinFileWriter &writer, void *sectionData,  uint32_t sectionSize, uint32_t section) {
    // Cast the output data pointer to uint32_t for easier access
    uint32_t coefNum = *static_cast<uint32_t*>(sectionData);
    uint8_t* data = static_cast<uint8_t*>(sectionData);

    uint64_t pos = 4;  // Position in section data

    std::unordered_map<Coef_t, std::vector<coef_opt>, CoefHashFNV> optimizedCoefs;
    std::vector<Coef_t> inclusionOrder;
    Coef_t coef_key;

    for (int i = 0; i < coefNum; i++) {
        if (pos >= sectionSize) {
            throw std::runtime_error("Unexpected end of section data while reading coefs");
        }

        if (pos + 44 > sectionSize) {
            throw std::runtime_error("Unexpected end of section data while reading coef component");
        }

        Coef* pcoef = reinterpret_cast<Coef*>(data + pos);
        uint32_t packed_c = pcoef->m ? pcoef->c | 0x80000000 : pcoef->c;
        memcpy(coef_key.data(), pcoef->data, 32);
        if (optimizedCoefs.find(coef_key) == optimizedCoefs.end())
            inclusionOrder.push_back(coef_key);
        optimizedCoefs[coef_key].push_back({packed_c, pcoef->s});

        pos += 44;
    }

    writer.startSection(section);
    // Overall coefficient number - 4 bytes
    writer.writeU32LE(coefNum);

    // Unique coef value number - 4 bytes
    writer.writeU32LE(optimizedCoefs.size());
    uint32_t coef_num = 0;

    // Store unique coef values
    for (auto& key : inclusionOrder) {
        writer.write(key.data(), 32);
        coef_num += optimizedCoefs[key].size();
        writer.writeU32LE(coef_num);
    }

    // Store coefs parameters
    for (auto& key : inclusionOrder) {
        std::vector<coef_opt>& opt_vec = optimizedCoefs[key];
        for (auto& opt : opt_vec) {
            writer.writeU32LE(opt.c);
            writer.writeU32LE(opt.s);
        }
    }

    writer.endSection();
}

} // namespace

