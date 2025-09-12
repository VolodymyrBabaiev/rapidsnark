#ifndef ZKEY_UTILS_H
#define ZKEY_UTILS_H

#include <gmp.h>
#include <memory>

#include "binfile_utils.hpp"
#include "binfile_writer.hpp"


namespace ZKeyUtils {

    class Header {


    public:
        u_int32_t n8q;
        mpz_t qPrime;
        u_int32_t n8r;
        mpz_t rPrime;

        u_int32_t nVars;
        u_int32_t nPublic;
        u_int32_t domainSize;
        u_int64_t nCoefs;

        void *vk_alpha1;
        void *vk_beta1;
        void *vk_beta2;
        void *vk_gamma2;
        void *vk_delta1;
        void *vk_delta2;

        Header();
        ~Header();
    };

    std::unique_ptr<Header> loadHeader(BinFileUtils::BinFile *f);
	void savePointsG1(BinFileUtils::BinFileWriter &writer, void *data,  uint32_t nPoints, uint32_t section);
	void savePointsG2(BinFileUtils::BinFileWriter &writer, void *data,  uint32_t nPoints, uint32_t section);
	void savePointsG1ZeroMask(BinFileUtils::BinFileWriter &writer, void *data, uint32_t nPoints, uint32_t section);
	void savePointsG2ZeroMask(BinFileUtils::BinFileWriter &writer, void *data, uint32_t nPoints, uint32_t section);
	void copyHeader(BinFileUtils::BinFileWriter &writer, BinFileUtils::BinFile &binFile);
	void copySection(BinFileUtils::BinFileWriter &writer, void *data,  uint32_t size, uint32_t section);
	void readPointsG1(BinFileUtils::BinFile& binFile, void *data, uint32_t nPoints, uint32_t section);
	void readPointsG2(BinFileUtils::BinFile& binFile, void *data, uint32_t nPoints, uint32_t section);
	void optimizeCoefs(BinFileUtils::BinFileWriter &writer, void *sectionData,  uint32_t sectionSize, uint32_t section);
}

#endif // ZKEY_UTILS_H
