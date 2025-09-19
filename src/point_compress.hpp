#ifndef POINT_COMPRESS_HPP
#define POINT_COMPRESS_HPP

#include <gmp.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "alt_bn128.hpp"


/**
 * Simple square root for Alt-BN128 prime field
 * Since p ≡ 3 (mod 4), we can use the formula: sqrt(n) = n^((p+1)/4) mod p
 */
class AltBN128Sqrt {
private:
    mpz_t p;           // Alt-BN128 prime
    mpz_t exponent;    // (p+1)/4

public:
    AltBN128Sqrt() {
        mpz_init(p);
        mpz_init(exponent);

        // Alt-BN128 prime: 21888242871839275222246405745257275088696311157297823662689037894645226208583
        mpz_set_str(p, "21888242871839275222246405745257275088696311157297823662689037894645226208583", 10);

        // Compute (p+1)/4
        mpz_add_ui(exponent, p, 1);
        mpz_fdiv_q_2exp(exponent, exponent, 2); // Divide by 4
    }

    ~AltBN128Sqrt() {
        mpz_clear(p);
        mpz_clear(exponent);
    }

    /**
     * Compute square root of n modulo p
     * Returns true if n is a quadratic residue, false otherwise
     */
    bool sqrt(mpz_t result, const mpz_t n) {
        // Check if n is zero
        if (mpz_cmp_ui(n, 0) == 0) {
            mpz_set_ui(result, 0);
            return true;
        }

        // Compute n^((p+1)/4) mod p
        mpz_powm(result, n, exponent, p);

        // Verify that result² ≡ n (mod p)
        mpz_t temp;
        mpz_init(temp);
        mpz_powm_ui(temp, result, 2, p); // result²

        bool is_valid = (mpz_cmp(temp, n) == 0);
        mpz_clear(temp);

        return is_valid;
    }

    /**
     * Check if n is a quadratic residue (has a square root)
     */
    bool hasSquareRoot(const mpz_t n) {
        if (mpz_cmp_ui(n, 0) == 0) return true;

        // Legendre symbol: n^((p-1)/2) mod p
        mpz_t temp, exp;
        mpz_init(temp);
        mpz_init(exp);

        mpz_sub_ui(exp, p, 1);
        mpz_fdiv_q_2exp(exp, exp, 1); // (p-1)/2
        mpz_powm(temp, n, exp, p);

        bool hasRoot = (mpz_cmp_ui(temp, 1) == 0);

        mpz_clear(temp);
        mpz_clear(exp);
        return hasRoot;
    }
};

/**
 * Integration with your field elements
 * This assumes your FqElement can be converted to/from byte arrays
 */
template<typename Engine>
class EngineFieldSqrt {
private:
    AltBN128Sqrt sqrt_impl;
    Engine& engine;

public:
    explicit EngineFieldSqrt(Engine& eng) : engine(eng) {}

    bool sqrt(typename Engine::F1Element& result, const typename Engine::F1Element& input) {
        mpz_t n, sqrt_result;
        mpz_init(n);
        mpz_init(sqrt_result);

        // Convert field element to GMP
        fieldToGmp(n, input);

        // Compute square root
        bool success = sqrt_impl.sqrt(sqrt_result, n);

        if (success) {
            // Convert back to field element
            gmpToField(result, sqrt_result);
        }

        mpz_clear(n);
        mpz_clear(sqrt_result);
        return success;
    }

private:
    void fieldToGmp(mpz_t result, const typename Engine::F1Element& element) {
        // Assuming your field element is stored as bytes in little-endian format
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&element);
        size_t size = sizeof(element);

        // Import as little-endian bytes
        mpz_import(result, size, -1, 1, -1, 0, bytes);
    }

    void gmpToField(typename Engine::F1Element& result, const mpz_t value) {
        // Clear the result first
        memset(&result, 0, sizeof(result));

        // Export to bytes in little-endian format
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&result);
        size_t count;
        mpz_export(bytes, &count, -1, 1, -1, 0, value);
    }
};

void printField(void* field, size_t size) {
    uint8_t* p = reinterpret_cast<uint8_t*>(field);
    std::cout << "[ ";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned)p[i] << " ";
    }

    std::cout << std::dec << "]" << std::endl;
}

template <typename Engine>
typename Engine::F1Element compressPoint(typename Engine::G1PointAffine point) {
    typename Engine::F1Element x = point.x;
    uint8_t* p = reinterpret_cast<uint8_t*>(&x);
    p[31] = (point.y.v[0] & 1) ? 0x80 | p[31] : p[31]; // Set sign bit
    return x;
}

/**
 * Get the parity bit of a field element (LSB of the coordinate)
*/
template<typename BaseElement>
uint8_t getParityBit(BaseElement element) {
    const uint8_t* element_ptr = reinterpret_cast<const uint8_t*>(&element);
    return element_ptr[0] & 1;
}

/**
 * Decompress y-coordinate from x-coordinate using curve equation
 * For Alt-BN128: y² = x³ + 3
 */
template <typename Engine, typename BaseElement>
typename Engine::G1PointAffine decompressYCoordinate(BaseElement x, Engine& engine) {
    BaseElement x_cubed, y_squared, y, y_neg;
	EngineFieldSqrt<Engine> sqrt_calculator(engine);

    BaseElement unpackedX = x;
    uint8_t* p = reinterpret_cast<uint8_t*>(&unpackedX);
    uint8_t parity = p[31] & 0x80 ? 0x01 : 0x00; // Save sign bit
    p[31] &= 0x7F; // Set sign bit

    // Compute x³
	std::cout <<"unpackedX : ";
	printField(&unpackedX, 32);
	engine.f1.square(x_cubed, unpackedX);
	std::cout <<"x_squared : ";
	printField(&x_cubed, 32);
	engine.f1.mul(x_cubed, x_cubed, unpackedX);
	std::cout <<"x_cubed : ";
	printField(&x_cubed, 32);

    // Add curve parameter b (3 for Alt-BN128)
    BaseElement three;
	engine.f1.fromString(three, "3");
	engine.f1.add(y_squared, x_cubed, three);
	std::cout <<"Plus 3 : ";
	printField(&y_squared, 32);

    if (!sqrt_calculator.sqrt(y, y_squared)) {
        throw std::runtime_error("Point not on curve - no square root exists");
    }

	std::cout <<" y : ";
	printField(&y, 32);

    // Check parity and negate if necessary
    if (getParityBit(y) != (parity & 1)) {
		engine.f1.neg(y, y);
    }

	engine.f1.neg(y_neg, y);
	std::cout <<" y_neg : ";
	printField(&y_neg, 32);
    // Copy result
	typename Engine::G1PointAffine res;
	res.x = unpackedX;
	res.y = y;

	return res;
}


#endif //POINT_COMPRESS_HPP