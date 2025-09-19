#include <gmp.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "alt_bn128.hpp"
#include "point_compress.hpp"

class PointCompressionExample {
private:
    AltBN128Sqrt sqrt_calc;

public:
    void demonstrateCompression() {
        std::cout << "=== Point Compression/Decompression Example ===" << std::endl;

        // Your point data
        uint8_t x_bytes[32] = {
            0x9d, 0x32, 0x11, 0xf0, 0xeb, 0xe7, 0x20, 0x90,
            0xdf, 0x69, 0x3a, 0xb4, 0x30, 0x39, 0x92, 0xff,
            0xf7, 0x47, 0x6d, 0x39, 0x61, 0x6b, 0xe0, 0xba,
            0x6e, 0xa8, 0x2f, 0x9b, 0xef, 0xc0, 0xb0, 0x0b
        };

        uint8_t y_bytes[32] = {
            0x0d, 0xba, 0x82, 0x7e, 0x14, 0x35, 0xcc, 0xb7,
            0x73, 0xf1, 0x4e, 0xc0, 0x4d, 0x64, 0x57, 0x49,
            0xd4, 0x88, 0xf7, 0x97, 0xfa, 0xa0, 0x6e, 0xfb,
            0xb0, 0xc4, 0x4e, 0xef, 0x0d, 0x8b, 0x02, 0x13
        };

        std::cout << "\n--- ORIGINAL POINT ---" << std::endl;
        std::cout << "x: ";
        printHexBytes(x_bytes, 32);
        std::cout << "y: ";
        printHexBytes(y_bytes, 32);

        // Step 1: Show compression
        showCompression(x_bytes, y_bytes);

        // Step 2: Show decompression step-by-step
        showDecompressionSteps(x_bytes, y_bytes);

        // Step 3: Verify the point is on the curve
        verifyPointOnCurve(x_bytes, y_bytes);
    }

private:
    void showCompression(const uint8_t* x_bytes, const uint8_t* y_bytes) {
        std::cout << "\n--- COMPRESSION PROCESS ---" << std::endl;

        // Extract parity bit from y-coordinate
        uint8_t parity = y_bytes[0] & 1;

        std::cout << "1. Extract parity from y-coordinate:" << std::endl;
        std::cout << "   y[0] = 0x" << std::hex << std::setfill('0') << std::setw(2)
                  << (unsigned)y_bytes[0] << std::dec << std::endl;
        std::cout << "   y[0] & 1 = " << (unsigned)parity << std::endl;
        std::cout << "   Parity bit = " << (parity ? "1 (odd)" : "0 (even)") << std::endl;

        std::cout << "\n2. Compressed point format:" << std::endl;
        std::cout << "   x-coordinate (32 bytes): ";
        printHexBytes(x_bytes, 32);
        std::cout << "   parity bit (1 byte): 0x" << std::hex << std::setfill('0')
                  << std::setw(2) << (unsigned)parity << std::dec << std::endl;

        std::cout << "\n3. Complete compressed point (33 bytes total):" << std::endl;
        std::cout << "   ";
        printHexBytes(x_bytes, 32);
        std::cout << " " << std::hex << std::setfill('0') << std::setw(2)
                  << (unsigned)parity << std::dec << std::endl;

        // Calculate compression ratio
        double ratio = 33.0 / 64.0;
        double savings = (1.0 - ratio) * 100.0;
        std::cout << "\n4. Compression statistics:" << std::endl;
        std::cout << "   Original size: 64 bytes (32 + 32)" << std::endl;
        std::cout << "   Compressed size: 33 bytes (32 + 1)" << std::endl;
        std::cout << "   Compression ratio: " << std::fixed << std::setprecision(3) << ratio << std::endl;
        std::cout << "   Space savings: " << std::fixed << std::setprecision(1) << savings << "%" << std::endl;
    }

    void showDecompressionSteps(const uint8_t* x_bytes, const uint8_t* y_bytes) {
        std::cout << "\n--- DECOMPRESSION PROCESS ---" << std::endl;

        mpz_t x, y_squared, x_cubed, three, computed_y, p;
        mpz_init(x);
        mpz_init(y_squared);
        mpz_init(x_cubed);
        mpz_init(three);
        mpz_init(computed_y);
        mpz_init(p);

        // Set Alt-BN128 prime
        mpz_set_str(p, "21888242871839275222246405745257275088696311157297823662689037894645226208583", 10);
        mpz_set_ui(three, 3);

        // Convert x-coordinate from bytes to GMP integer (little-endian)
        mpz_import(x, 32, -1, 1, -1, 0, x_bytes);

        std::cout << "1. Input data:" << std::endl;
        std::cout << "   x = ";
        mpz_out_str(stdout, 10, x);
        std::cout << std::endl;

        uint8_t stored_parity = y_bytes[0] & 1;
        std::cout << "   stored_parity = " << (unsigned)stored_parity << std::endl;

        std::cout << "\n2. Compute x³:" << std::endl;
        mpz_powm_ui(x_cubed, x, 3, p);
        std::cout << "   x³ = ";
        mpz_out_str(stdout, 10, x_cubed);
        std::cout << std::endl;

        std::cout << "\n3. Compute x³ + 3 (curve equation y² = x³ + 3):" << std::endl;
        mpz_add(y_squared, x_cubed, three);
        mpz_mod(y_squared, y_squared, p);
        std::cout << "   x³ + 3 = ";
        mpz_out_str(stdout, 10, y_squared);
        std::cout << std::endl;

        std::cout << "\n4. Check if x³ + 3 is a quadratic residue:" << std::endl;
        bool has_sqrt = sqrt_calc.hasSquareRoot(y_squared);
        std::cout << "   hasSquareRoot(x³ + 3) = " << (has_sqrt ? "true" : "false") << std::endl;

        if (has_sqrt) {
            std::cout << "\n5. Compute square root: y = sqrt(x³ + 3):" << std::endl;
            bool sqrt_success = sqrt_calc.sqrt(computed_y, y_squared);

            if (sqrt_success) {
                std::cout << "   computed_y = ";
                mpz_out_str(stdout, 10, computed_y);
                std::cout << std::endl;

                // Check parity of computed y
                uint8_t computed_parity = mpz_odd_p(computed_y) ? 1 : 0;
                std::cout << "\n6. Check parity of computed y:" << std::endl;
                std::cout << "   computed_parity = " << (unsigned)computed_parity << std::endl;
                std::cout << "   stored_parity = " << (unsigned)stored_parity << std::endl;

                if (computed_parity != stored_parity) {
                    std::cout << "   Parities don't match - negating y" << std::endl;
                    mpz_sub(computed_y, p, computed_y);  // computed_y = p - computed_y
                    std::cout << "   After negation: y = ";
                    mpz_out_str(stdout, 10, computed_y);
                    std::cout << std::endl;
                } else {
                    std::cout << "   Parities match - keeping computed y" << std::endl;
                }

                std::cout << "\n7. Verification - compare with original y:" << std::endl;

                // Convert original y to GMP for comparison
                mpz_t original_y;
                mpz_init(original_y);
                mpz_import(original_y, 32, -1, 1, -1, 0, y_bytes);

                std::cout << "   original_y = ";
                mpz_out_str(stdout, 10, original_y);
                std::cout << std::endl;
                std::cout << "   computed_y = ";
                mpz_out_str(stdout, 10, computed_y);
                std::cout << std::endl;

                if (mpz_cmp(computed_y, original_y) == 0) {
                    std::cout << "   ✅ SUCCESS: Decompressed y matches original!" << std::endl;
                } else {
                    std::cout << "   ❌ MISMATCH: Decompressed y does not match original" << std::endl;
                }

                std::cout << "\n8. Final verification - check y² = x³ + 3:" << std::endl;
                mpz_t verification;
                mpz_init(verification);
                mpz_powm_ui(verification, computed_y, 2, p);

                if (mpz_cmp(verification, y_squared) == 0) {
                    std::cout << "   ✅ Point equation verified: y² ≡ x³ + 3 (mod p)" << std::endl;
                } else {
                    std::cout << "   ❌ Point equation failed" << std::endl;
                }

                mpz_clear(verification);
                mpz_clear(original_y);
            } else {
                std::cout << "   ❌ Failed to compute square root" << std::endl;
            }
        } else {
            std::cout << "   ❌ x³ + 3 is not a quadratic residue - invalid point!" << std::endl;
        }

        mpz_clear(x);
        mpz_clear(y_squared);
        mpz_clear(x_cubed);
        mpz_clear(three);
        mpz_clear(computed_y);
        mpz_clear(p);
    }

    void verifyPointOnCurve(const uint8_t* x_bytes, const uint8_t* y_bytes) {
        std::cout << "\n--- CURVE VERIFICATION ---" << std::endl;

        mpz_t x, y, x_cubed, y_squared, three, p, lhs, rhs;
        mpz_init(x);
        mpz_init(y);
        mpz_init(x_cubed);
        mpz_init(y_squared);
        mpz_init(three);
        mpz_init(p);
        mpz_init(lhs);
        mpz_init(rhs);

        // Set values
        mpz_set_str(p, "21888242871839275222246405745257275088696311157297823662689037894645226208583", 10);
        mpz_set_ui(three, 3);
        mpz_import(x, 32, -1, 1, -1, 0, x_bytes);
        mpz_import(y, 32, -1, 1, -1, 0, y_bytes);

        // Compute LHS: y²
        mpz_powm_ui(lhs, y, 2, p);

        // Compute RHS: x³ + 3
        mpz_powm_ui(x_cubed, x, 3, p);
        mpz_add(rhs, x_cubed, three);
        mpz_mod(rhs, rhs, p);

        std::cout << "Verifying: y² ≡ x³ + 3 (mod p)" << std::endl;
        std::cout << "LHS (y²) = ";
        mpz_out_str(stdout, 10, lhs);
        std::cout << std::endl;
        std::cout << "RHS (x³+3) = ";
        mpz_out_str(stdout, 10, rhs);
        std::cout << std::endl;

        if (mpz_cmp(lhs, rhs) == 0) {
            std::cout << "✅ VERIFIED: Point is on Alt-BN128 curve" << std::endl;
        } else {
            std::cout << "❌ ERROR: Point is NOT on Alt-BN128 curve" << std::endl;
        }

        mpz_clear(x);
        mpz_clear(y);
        mpz_clear(x_cubed);
        mpz_clear(y_squared);
        mpz_clear(three);
        mpz_clear(p);
        mpz_clear(lhs);
        mpz_clear(rhs);
    }

    void printHexBytes(const uint8_t* bytes, int count) {
        for (int i = 0; i < count; i++) {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << (unsigned)bytes[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl << "   ";
        }
        std::cout << std::dec << std::endl;
    }
};

// Utility function to show the compressed representation
void showCompressedRepresentation() {
    std::cout << "\n=== COMPRESSED POINT REPRESENTATION ===" << std::endl;

    uint8_t x_bytes[32] = {
        0x9d, 0x32, 0x11, 0xf0, 0xeb, 0xe7, 0x20, 0x90,
        0xdf, 0x69, 0x3a, 0xb4, 0x30, 0x39, 0x92, 0xff,
        0xf7, 0x47, 0x6d, 0x39, 0x61, 0x6b, 0xe0, 0xba,
        0x6e, 0xa8, 0x2f, 0x9b, 0xef, 0xc0, 0xb0, 0x0b
    };

    uint8_t y_bytes[32] = {
        0x0d, 0xba, 0x82, 0x7e, 0x14, 0x35, 0xcc, 0xb7,
        0x73, 0xf1, 0x4e, 0xc0, 0x4d, 0x64, 0x57, 0x49,
        0xd4, 0x88, 0xf7, 0x97, 0xfa, 0xa0, 0x6e, 0xfb,
        0xb0, 0xc4, 0x4e, 0xef, 0x0d, 0x8b, 0x02, 0x13
    };

    // Extract parity
    uint8_t parity = y_bytes[0] & 1;

    std::cout << "Your compressed point is:" << std::endl;
    std::cout << "[";
    for (int i = 0; i < 32; i++) {
        std::cout << " " << std::hex << std::setfill('0') << std::setw(2)
                  << (unsigned)x_bytes[i];
    }
    std::cout << " " << std::hex << std::setfill('0') << std::setw(2)
              << (unsigned)parity;
    std::cout << " ]" << std::dec << std::endl;

    std::cout << "\nBreakdown:" << std::endl;
    std::cout << "- First 32 bytes: x-coordinate" << std::endl;
    std::cout << "- Last byte (0x" << std::hex << (unsigned)parity << std::dec
              << "): parity bit (" << (parity ? "odd" : "even") << ")" << std::endl;
}

void squareRootTest() {
	std::cout<<"Square root test"<<std::endl;
	EngineFieldSqrt<AltBn128::Engine> sqrt_calculator(AltBn128::Engine::engine);

	uint8_t raw_res[32] = {
    	0x9D, 0x32, 0x11, 0xF0, 0xEB, 0xE7, 0x20, 0x90,
    	0xDF, 0x69, 0x3A, 0xB4, 0x30, 0x39, 0x92, 0xFF,
    	0xF7, 0x47, 0x6D, 0x39, 0x61, 0x6B, 0xE0, 0xBA,
    	0x6E, 0xA8, 0x2F, 0x9B, 0xEF, 0xC0, 0xB0, 0x0B};

	uint8_t raw_value[32] = {
    	0xDA, 0x78, 0x1E, 0x38, 0x69, 0xAD, 0x11, 0x2F,
    	0xDC, 0x93, 0x14, 0x51, 0x3D, 0x30, 0x67, 0x24,
    	0x37, 0x2F, 0x24, 0xDE, 0x60, 0xB2, 0xA0, 0x5C,
    	0x88, 0x81, 0x1C, 0x73, 0x34, 0xDB, 0x8B, 0x1A
	};


	//uint8_t raw_value[32] = { 0x09 };
	//uint8_t raw_res[32] = { 0x03 };

	//uint8_t raw_value[32] = { 0x00 };
	//uint8_t raw_res[32] = { 0x00 };

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
	std::cout<<"Square root test"<<std::endl;
	EngineFieldSqrt<AltBn128::Engine> sqrt_calculator(AltBn128::Engine::engine);
	uint8_t data[] = {0x44, 0xFD, 0x7C, 0xD8, 0x16, 0x8C, 0x20, 0x3C, 0x8D, 0xCA, 0x71, 0x68, 0x91, 0x6A, 0x81, 0x97,
    				  0x5D, 0x58, 0x81, 0x81, 0xB6, 0x45, 0x50, 0xB8, 0x29, 0xA0, 0x31, 0xE1, 0x72, 0x4E, 0x64, 0x30};
	uint8_t raw_value[32] = { 0x09 };
	uint8_t raw_res[32] = { 0x03 };

	//uint8_t raw_value[32] = { 0x00 };
	//uint8_t raw_res[32] = { 0x00 };

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
	//memcpy(expected_x, &raw_point, sizeof(raw_point)/2);

	AltBn128::F1Element zk_compressed = compressPoint<AltBn128::Engine>(zkeyPoint);
	if (memcmp(expected_x, reinterpret_cast<uint8_t*>(&zk_compressed), sizeof(zkeyPoint)/2)) {
		std::cout<<"Compressed point is wrong"<<std::endl;
		std::cout<<"Expected   : ";
		printField(expected_x, 32);
		std::cout<< "Compressed: ";
		printField(&zk_compressed, 32);
	}

    AltBn128::G1PointAffine zkeyDecompressed = decompressYCoordinate(zk_compressed, AltBn128::Engine::engine);
	if (memcmp(raw_point, reinterpret_cast<uint8_t*>(&zkeyDecompressed), sizeof(zkeyDecompressed)) != 0) {
		std::cout<<"Decompressed point is wrong"<<std::endl;
		std::cout<<"Expected: ";
		printField(raw_point + 32, 32);
		std::cout<<"Decompressed: ";
		printField(reinterpret_cast<uint8_t*>(&zkeyDecompressed) + 32, 32);
	}

	std::cout<<"Decompress point test finished"<<std::endl;
}

int main() {
    try {
        showCompressedRepresentation();

        PointCompressionExample example;
        example.demonstrateCompression();

		squareRootTest();
		decompressUnitTest();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/*
Expected output format:

=== COMPRESSED POINT REPRESENTATION ===
Your compressed point is:
[ 9d 32 11 f0 eb e7 20 90 df 69 3a b4 30 39 92 ff f7 47 6d 39 61 6b e0 ba 6e a8 2f 9b ef c0 b0 0b 01 ]

Breakdown:
- First 32 bytes: x-coordinate
- Last byte (0x01): parity bit (odd)

--- COMPRESSION PROCESS ---
1. Extract parity from y-coordinate:
   y[0] = 0x0d
   y[0] & 1 = 1
   Parity bit = 1 (odd)

2. Complete compressed point (33 bytes total):
   9d 32 11 f0 eb e7 20 90 df 69 3a b4 30 39 92 ff f7 47 6d 39 61 6b e0 ba 6e a8 2f 9b ef c0 b0 0b 01

--- DECOMPRESSION PROCESS ---
1. Input data:
   x = [large number]
   stored_parity = 1

2. Compute x³:
   x³ = [computed value]

3. Compute x³ + 3:
   x³ + 3 = [computed value]

4. Check if x³ + 3 is a quadratic residue:
   hasSquareRoot(x³ + 3) = true

5. Compute square root: y = sqrt(x³ + 3):
   computed_y = [computed value]

6. Check parity of computed y:
   computed_parity = 0 or 1
   stored_parity = 1
   [Negate if needed]

7. Verification - compare with original y:
   ✅ SUCCESS: Decompressed y matches original!
*/