#include <stdexcept>
#include <cstring>
#include <iostream>

#include "zkey_utils.hpp"

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

    h->nCoefs = f->getSectionSize(4) / (12 + h->n8r);

    return h;
}

void saveHeader(BinFileUtils::BinFileWriter &writer, const Header& header) {
    // Section 1: Protocol
    writer.startSection(1);
    writer.writeU32LE(1); // protocol = 1 for groth16
    writer.endSection();

    // Section 2: Header data
    writer.startSection(2);

    // Write n8q and qPrime
    writer.writeU32LE(header.n8q);

    // Export qPrime to bytes and write
    size_t qPrimeSize = (mpz_sizeinbase(header.qPrime, 2) + 7) / 8; // Size in bytes
    if (qPrimeSize != header.n8q) {
        throw std::invalid_argument("qPrime size mismatch with n8q");
    }

    std::vector<uint8_t> qPrimeBytes(header.n8q);
    size_t exportedBytes;
    mpz_export(qPrimeBytes.data(), &exportedBytes, -1, 1, -1, 0, header.qPrime);
    if (exportedBytes != header.n8q) {
        // Pad with zeros if needed
        if (exportedBytes < header.n8q) {
            std::memmove(qPrimeBytes.data() + (header.n8q - exportedBytes),
                        qPrimeBytes.data(), exportedBytes);
            std::memset(qPrimeBytes.data(), 0, header.n8q - exportedBytes);
        }
    }
    writer.write(qPrimeBytes.data(), header.n8q);


    // Write n8r and rPrime
    writer.writeU32LE(header.n8r);

    // Export rPrime to bytes and write
    size_t rPrimeSize = (mpz_sizeinbase(header.rPrime, 2) + 7) / 8; // Size in bytes
    if (rPrimeSize != header.n8r) {
        throw std::invalid_argument("rPrime size mismatch with n8r");
    }

    std::vector<uint8_t> rPrimeBytes(header.n8r);
    mpz_export(rPrimeBytes.data(), &exportedBytes, -1, 1, -1, 0, header.rPrime);
    if (exportedBytes != header.n8r) {
        // Pad with zeros if needed
        if (exportedBytes < header.n8r) {
            std::memmove(rPrimeBytes.data() + (header.n8r - exportedBytes),
                        rPrimeBytes.data(), exportedBytes);
            std::memset(rPrimeBytes.data(), 0, header.n8r - exportedBytes);
        }
    }
    writer.write(rPrimeBytes.data(), header.n8r);

    // Write circuit parameters
    writer.writeU32LE(header.nVars);
    writer.writeU32LE(header.nPublic);
    writer.writeU32LE(header.domainSize);

    // Write verification key components
    writer.write(header.vk_alpha1, header.n8q * 2);  // G1 point: 2 * n8q
    writer.write(header.vk_beta1, header.n8q * 2);   // G1 point: 2 * n8q
    writer.write(header.vk_beta2, header.n8q * 4);   // G2 point: 4 * n8q
    writer.write(header.vk_gamma2, header.n8q * 4);  // G2 point: 4 * n8q
    writer.write(header.vk_delta1, header.n8q * 2);  // G1 point: 2 * n8q
    writer.write(header.vk_delta2, header.n8q * 4);  // G2 point: 4 * n8q

    writer.endSection();
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

} // namespace

