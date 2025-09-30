#include <gmp.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "alt_bn128.hpp"
#include "point_compress.hpp"
#include "sqrt_tonelli.hpp"
#include "zkey_utils.hpp"

void squareRootTest() {
	std::cout<<"Square root test"<<std::endl;
	FieldElementSqrt<AltBn128::Engine> sqrt_calculator(AltBn128::Engine::engine);
	uint8_t raw_value[32] = {
		0x82, 0xea, 0x90, 0xb0, 0xd9, 0x5b, 0x3d, 0x03,
		0x3b, 0x4f, 0xc5, 0x93, 0xd1, 0xb9, 0x72, 0xa3,
		0xb4, 0x96, 0xee, 0x07, 0x17, 0x7e, 0xf5, 0xad,
		0x54, 0x28, 0x0a, 0x26, 0xa6, 0x3a, 0xca, 0x10
	};

	uint8_t raw_res[32] = {
		0xc5, 0x67, 0xc0, 0xd9, 0x09, 0xef, 0x83, 0xb5,
		0x7b, 0x91, 0x78, 0xb7, 0xb1, 0x15, 0x2d, 0x8f,
		0xed, 0x9f, 0x9d, 0x15, 0x9a, 0x11, 0x17, 0x24,
		0xc6, 0xfa, 0xd2, 0xf1, 0x3b, 0xaa, 0x58, 0x03
	};

	std::cout<<"Original value : ";
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << (int)raw_value[i] << " ";
	}
	std::cout << std::dec << std::endl;

	AltBn128::F1Element value;
	AltBn128::F1Element sqrt_res;

	memcpy(reinterpret_cast<uint8_t*>(&value), raw_value, sizeof(raw_value));

	if (!sqrt_calculator.sqrt(sqrt_res, value)) {
        //throw std::runtime_error("Point not on curve - no square root exists");
		std::cout<<"Point not on curve - no square root exists"<<std::endl;
    }

	uint8_t* res_ptr = reinterpret_cast<uint8_t*>(&sqrt_res);
	if (memcmp(res_ptr, raw_res, 32) == 0) {
		std::cout<<"Square root test passed"<<std::endl;
	} else {
		std::cout<<"Sqrt is wrong"<<std::endl;
	}

	std::cout<<"Square root : ";
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << (int)res_ptr[i] << " ";
	}
	std::cout << std::dec << std::endl;

	AltBn128::Engine::engine.f1.neg(sqrt_res, sqrt_res);

	std::cout<<"Square root negative: ";
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << (int)res_ptr[i] << " ";
	}
	std::cout << std::dec << std::endl;
}

void squareRootTest2() {
	std::cout<<"Square root test 2(Montgomery form)"<<std::endl;
	AltBN128SqrtMnt<AltBn128::Engine> sqrt_calculator(AltBn128::Engine::engine);

	uint8_t raw_value[32] = {
		0x82, 0xea, 0x90, 0xb0, 0xd9, 0x5b, 0x3d, 0x03,
		0x3b, 0x4f, 0xc5, 0x93, 0xd1, 0xb9, 0x72, 0xa3,
		0xb4, 0x96, 0xee, 0x07, 0x17, 0x7e, 0xf5, 0xad,
		0x54, 0x28, 0x0a, 0x26, 0xa6, 0x3a, 0xca, 0x10
	};

	uint8_t raw_res[32] = {
		0xc5, 0x67, 0xc0, 0xd9, 0x09, 0xef, 0x83, 0xb5,
		0x7b, 0x91, 0x78, 0xb7, 0xb1, 0x15, 0x2d, 0x8f,
		0xed, 0x9f, 0x9d, 0x15, 0x9a, 0x11, 0x17, 0x24,
		0xc6, 0xfa, 0xd2, 0xf1, 0x3b, 0xaa, 0x58, 0x03
	};

	std::cout<<"Original value : ";
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << (int)raw_value[i] << " ";
	}
	std::cout << std::dec << std::endl;

	AltBn128::F1Element value;
	AltBn128::F1Element sqrt_res;

	memcpy(reinterpret_cast<uint8_t*>(&value), raw_value, sizeof(raw_value));

	if (!sqrt_calculator.sqrt(sqrt_res, value)) {
        //throw std::runtime_error("Point not on curve - no square root exists");
		std::cout<<"Point not on curve - no square root exists"<<std::endl;
    }

	uint8_t* res_ptr = reinterpret_cast<uint8_t*>(&sqrt_res);
	if (memcmp(res_ptr, raw_res, 32) == 0) {
		std::cout<<"Square root test passed"<<std::endl;
	} else {
		std::cout<<"Sqrt is wrong"<<std::endl;
	}

	std::cout<<"Square root : ";
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << (int)res_ptr[i] << " ";
	}
	std::cout << std::dec << std::endl;

	AltBn128::Engine::engine.f1.neg(sqrt_res, sqrt_res);

	std::cout<<"Square root negative: ";
	for (int i = 0; i < 32; i++) {
		std::cout << std::hex << (int)res_ptr[i] << " ";
	}
	std::cout << std::dec << std::endl;
}

void decompressUnitTest() {
	std::cout<<"Decompress point test"<<std::endl;
	uint8_t raw_point[64] = {
    	0x9D, 0x32, 0x11, 0xF0, 0xEB, 0xE7, 0x20, 0x90,
    	0xDF, 0x69, 0x3A, 0xB4, 0x30, 0x39, 0x92, 0xFF,
    	0xF7, 0x47, 0x6D, 0x39, 0x61, 0x6B, 0xE0, 0xBA,
    	0x6E, 0xA8, 0x2F, 0x9B, 0xEF, 0xC0, 0xB0, 0x0B,
    	0x0D, 0xBA, 0x82, 0x7E, 0x14, 0x35, 0xCC, 0xB7,
    	0x73, 0xF1, 0x4E, 0xC0, 0x4D, 0x64, 0x57, 0x49,
    	0xD4, 0x88, 0xF7, 0x97, 0xFA, 0xA0, 0x6E, 0xFB,
    	0xB0, 0xC4, 0x4E, 0xEF, 0x0D, 0x8B, 0x02, 0x13
	};

	uint8_t expected_x[32] = {
    	0x9D, 0x32, 0x11, 0xF0, 0xEB, 0xE7, 0x20, 0x90,
    	0xDF, 0x69, 0x3A, 0xB4, 0x30, 0x39, 0x92, 0xFF,
    	0xF7, 0x47, 0x6D, 0x39, 0x61, 0x6B, 0xE0, 0xBA,
    	0x6E, 0xA8, 0x2F, 0x9B, 0xEF, 0xC0, 0xB0, 0x8B
	};

	AltBn128::G1PointAffine zkeyPoint;
	memcpy(&zkeyPoint, &raw_point, sizeof(raw_point));
	std::cout<<"Original point is : ";
	ZKeyUtils::printField(raw_point, 32);
	ZKeyUtils::printField(raw_point + 32, 32);

	AltBn128::F1Element zk_compressed = compressPoint<AltBn128::Engine>(zkeyPoint);
	if (memcmp(expected_x, reinterpret_cast<uint8_t*>(&zk_compressed), sizeof(zkeyPoint)/2)) {
		std::cout<<"Compressed point is wrong"<<std::endl;
		std::cout<<"Expected   : ";
		ZKeyUtils::printField(expected_x, 32);
	}

	std::cout<< "Compressed: ";
	ZKeyUtils::printField(&zk_compressed, 32);

    AltBn128::G1PointAffine zkeyDecompressed = decompressYCoordinate(zk_compressed, AltBn128::Engine::engine);
	if (memcmp(raw_point, reinterpret_cast<uint8_t*>(&zkeyDecompressed), sizeof(zkeyDecompressed)) != 0) {
		std::cout<<"Decompressed point is wrong"<<std::endl;
		std::cout<<"Expected: ";
		ZKeyUtils::printField(raw_point + 32, 32);
		std::cout<<"Decompressed: ";
		ZKeyUtils::printField(reinterpret_cast<uint8_t*>(&zkeyDecompressed) + 32, 32);
	}

	std::cout<<"Decompress point test finished"<<std::endl;
}

void decompressUnitTest2() {
	std::cout<<"Montgomery decompress point test"<<std::endl;
	uint8_t raw_point[64] = {
    	0x9D, 0x32, 0x11, 0xF0, 0xEB, 0xE7, 0x20, 0x90,
    	0xDF, 0x69, 0x3A, 0xB4, 0x30, 0x39, 0x92, 0xFF,
    	0xF7, 0x47, 0x6D, 0x39, 0x61, 0x6B, 0xE0, 0xBA,
    	0x6E, 0xA8, 0x2F, 0x9B, 0xEF, 0xC0, 0xB0, 0x0B,
    	0x0D, 0xBA, 0x82, 0x7E, 0x14, 0x35, 0xCC, 0xB7,
    	0x73, 0xF1, 0x4E, 0xC0, 0x4D, 0x64, 0x57, 0x49,
    	0xD4, 0x88, 0xF7, 0x97, 0xFA, 0xA0, 0x6E, 0xFB,
    	0xB0, 0xC4, 0x4E, 0xEF, 0x0D, 0x8B, 0x02, 0x13
	};

	uint8_t expected_x[32] = {
    	0x9D, 0x32, 0x11, 0xF0, 0xEB, 0xE7, 0x20, 0x90,
    	0xDF, 0x69, 0x3A, 0xB4, 0x30, 0x39, 0x92, 0xFF,
    	0xF7, 0x47, 0x6D, 0x39, 0x61, 0x6B, 0xE0, 0xBA,
    	0x6E, 0xA8, 0x2F, 0x9B, 0xEF, 0xC0, 0xB0, 0x0B
	};

	AltBn128::G1PointAffine zkeyPoint;
	memcpy(&zkeyPoint, &raw_point, sizeof(raw_point));
	std::cout<<"Original point is : ";
	ZKeyUtils::printField(raw_point, 32);
	ZKeyUtils::printField(raw_point + 32, 32);

	AltBn128::F1Element zk_compressed = compressPointMontgomery<AltBn128::Engine, RawFq::Element>(zkeyPoint, AltBn128::Engine::engine);
	if (memcmp(expected_x, reinterpret_cast<uint8_t*>(&zk_compressed), sizeof(zkeyPoint)/2)) {
		std::cout<<"Compressed point is wrong"<<std::endl;
		std::cout<<"Expected   : ";
		ZKeyUtils::printField(expected_x, 32);
	}

	std::cout<< "Compressed: ";
	ZKeyUtils::printField(&zk_compressed, 32);

    AltBn128::G1PointAffine zkeyDecompressed = decompressYCoordinateMontgomery(zk_compressed, AltBn128::Engine::engine);
	if (memcmp(raw_point, reinterpret_cast<uint8_t*>(&zkeyDecompressed), sizeof(zkeyDecompressed)) != 0) {
		std::cout<<"Decompressed point is wrong"<<std::endl;
		std::cout<<"Expected: ";
		ZKeyUtils::printField(raw_point + 32, 32);
		std::cout<<"Decompressed: ";
		ZKeyUtils::printField(reinterpret_cast<uint8_t*>(&zkeyDecompressed) + 32, 32);
	}

	std::cout<<"Decompress point test finished"<<std::endl;
}

int main() {
    try {
		squareRootTest();
		squareRootTest2();
		decompressUnitTest();
		decompressUnitTest2();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
