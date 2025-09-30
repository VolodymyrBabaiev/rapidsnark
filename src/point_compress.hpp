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

template<typename Engine>
class AltBN128SqrtMnt {
private:
	Engine& engine;
    typename Engine::F1Element exponent;     // (p+1)/4
	typename Engine::F1Element exponent_2;   // 2

    /**
     * Field exponentiation: base^exp mod p using square-and-multiply
     * Works with field elements in normal (non-Montgomery) form
     */
    void fieldPow(typename Engine::F1Element& result, const typename Engine::F1Element& base, const typename Engine::F1Element& exp) {
        // Initialize result to 1
        typename Engine::F1Element one;
        engine.f1.fromUI(one, 1);
        engine.f1.copy(result, one);

        // Convert exponent to bytes for bit-by-bit processing
        uint8_t exp_bytes[32];
        memcpy(exp_bytes, &exp, 32);

        typename Engine::F1Element current_base;
        engine.f1.copy(current_base, base);

        // Square-and-multiply algorithm
        for (int i = 0; i < 256; i++) {  // 256 bits = 32 bytes * 8
            int byte_idx = i / 8;
            int bit_idx = i % 8;

            if (exp_bytes[byte_idx] & (1 << bit_idx)) {
                engine.f1.mul(result, result, current_base);
            }

            // Square for next iteration (except last)
            if (i < 255) {
                engine.f1.square(current_base, current_base);
            }
        }
    }

    /**
     * Field exponentiation for Montgomery form: base^exp mod p
     * Input and output are in Montgomery form
     * Uses square-and-multiply algorithm optimized for Montgomery arithmetic
     */
    void fieldPowMontgomery(typename Engine::F1Element& result, const typename Engine::F1Element& base, const typename Engine::F1Element& exp) {
        // Initialize result to Montgomery form of 1
        typename Engine::F1Element one;
        engine.f1.fromUI(one, 1);
        engine.f1.toMontgomery(result, one);

        // Convert exponent to bytes for bit-by-bit processing
        uint8_t exp_bytes[32];
        typename Engine::F1Element exp_normal;
        engine.f1.fromMontgomery(exp_normal, exp);
        memcpy(exp_bytes, &exp_normal, 32);

        // Copy base (already in Montgomery form)
        typename Engine::F1Element current_base;
        engine.f1.copy(current_base, base);

        // Square-and-multiply algorithm using Montgomery multiplication
        for (int i = 0; i < 256; i++) {  // 256 bits = 32 bytes * 8
            int byte_idx = i / 8;
            int bit_idx = i % 8;

            if (exp_bytes[byte_idx] & (1 << bit_idx)) {
                // Montgomery multiplication: result = result * current_base (in Montgomery form)
                engine.f1.mul(result, result, current_base);
            }

            // Square for next iteration (except last)
            if (i < 255) {
                // Montgomery squaring: current_base = current_base²
                engine.f1.square(current_base, current_base);
            }
        }
    }

public:
    AltBN128SqrtMnt(Engine& eng) : engine(eng)  {
        // Exponent (p+1)/4
		engine.f1.fromString(exponent, "5472060717959818805561601436314318772174077789324455915672259473661306552146");
		engine.f1.fromString(exponent_2, "2");
    }

    ~AltBN128SqrtMnt() {}

    /**
     * Compute square root of n modulo p
     * Returns true if n is a quadratic residue, false otherwise
     */
    bool sqrt(typename Engine::F1Element& result, const typename Engine::F1Element& input) {
        // Check if n is zero
    	if (engine.f1.isZero(input)) {
            engine.f1.copy(result, engine.f1.zero());
            return true;
        }

        // Compute n^((p+1)/4) mod p
		//fieldPow(result, input, exponent);
	    fieldPowMontgomery(result, input, exponent);

        // Verify that result² ≡ n (mod p)
		typename Engine::F1Element temp;
		//fieldPow(temp, result, exponent_2);
		fieldPowMontgomery(temp, result, exponent_2);

		bool is_valid = memcmp(&temp, &input, 32);

        return is_valid;
    }

    /**
     * Check if n is a quadratic residue (has a square root)
     */
    bool hasSquareRoot(const typename Engine::F1Element& input) {
        if (engine.f1.isZero(input)) return true;

        // Compute input^((p-1)/2) mod p for Legendre symbol
        typename Engine::F1Element legendre_exp;
        typename Engine::F1Element normal_input;

        // (p-1)/2 = 10944121435879639402780800718157159386087038894662227957836129736830653276291
        engine.f1.fromString(legendre_exp, "10944121435879639402780800718157159386087038894662227957836129736830653276291");

        engine.f1.fromMontgomery(normal_input, input);

        typename Engine::F1Element result;
        fieldPowMontgomery(result, normal_input, legendre_exp);

        // If result is 1, then input is a quadratic residue
        typename Engine::F1Element one;
        engine.f1.fromUI(one, 1);

        return engine.f1.eq(result, one);
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

template <typename Engine>
typename Engine::F1Element compressPoint(typename Engine::G1PointAffine point) {
    typename Engine::F1Element x = point.x;
    uint8_t* p = reinterpret_cast<uint8_t*>(&x);
    p[31] = (point.y.v[0] & 1) ? 0x80 | p[31] : p[31]; // Set sign bit
    return x;
}

template <typename Engine, typename BaseElement>
typename Engine::F1Element compressPointMontgomery(typename Engine::G1PointAffine point, Engine& engine) {
    typename Engine::F1Element x = point.x;

    BaseElement normal_y;
    engine.f1.fromMontgomery(normal_y, point.y);

    uint8_t* p = reinterpret_cast<uint8_t*>(&x);
    p[31] = (normal_y.v[0] & 1) ? 0x80 | p[31] : p[31]; // Set sign bit
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
 * Get the parity bit of a field element (LSB of the coordinate)
*/
template<typename Engine, typename BaseElement>
uint8_t getParityBitMontgomery(BaseElement element, Engine& engine) {
    BaseElement normal;
    engine.f1.fromMontgomery(normal, element);
	const uint8_t* element_ptr = reinterpret_cast<const uint8_t*>(&normal);

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
	//std::cout <<"unpackedX : ";
	//printField(&unpackedX, 32);

	engine.f1.square(x_cubed, unpackedX);
	//std::cout <<"x_squared : ";
	//printField(&x_cubed, 32);
	engine.f1.mul(x_cubed, x_cubed, unpackedX);
	//std::cout <<"x_cubed : ";
	//printField(&x_cubed, 32);

    // Add curve parameter b (3 for Alt-BN128)
    BaseElement three;
	engine.f1.fromString(three, "3");
	engine.f1.add(y_squared, x_cubed, three);
	//std::cout <<"Plus 3 (y squad): ";
	//printField(&y_squared, 32);

	BaseElement y_sqad_normal;
	engine.f1.fromMontgomery(y_sqad_normal, y_squared);
	//std::cout <<"y squad(normal): ";
	//printField(&y_sqad_normal, 32);

    if (!sqrt_calculator.sqrt(y, y_sqad_normal)) {
        throw std::runtime_error("Point not on curve - no square root exists");
    }

	//std::cout <<" y : ";
	//printField(&y, 32);

	engine.f1.neg(y_neg, y);
	//std::cout <<" y_neg : ";
	//printField(&y_neg, 32);

	BaseElement mont_y;
	engine.f1.toMontgomery(mont_y, y);
	//std::cout <<" y montgomery  : ";
	//printField(&mont_y, 32);

	// Check parity and negate if necessary
    if (getParityBit(mont_y) != (parity & 1)) {
		engine.f1.neg(y, y);
		engine.f1.toMontgomery(mont_y, y);
    }

    // Copy result
	typename Engine::G1PointAffine res;
	res.x = unpackedX;
	res.y = mont_y;

	engine.f1.toMontgomery(mont_y, y_neg);
	//std::cout <<" y montgomery  neg: ";
	//printField(&mont_y, 32);

	return res;
}

/**
 * Decompress y-coordinate from x-coordinate using curve equation
 * For Alt-BN128: y² = x³ + 3
 */
template <typename Engine, typename BaseElement>
typename Engine::G1PointAffine decompressYCoordinateMontgomery(BaseElement x, Engine& engine) {
    BaseElement x_cubed, y_squared, y, y_neg;
	EngineFieldSqrt<Engine> sqrt_calculator(engine);

    BaseElement unpackedX = x;
    uint8_t* p = reinterpret_cast<uint8_t*>(&unpackedX);
    uint8_t parity = p[31] & 0x80 ? 0x01 : 0x00; // Save sign bit
    p[31] &= 0x7F; // Set sign bit

    // Compute x³
	//std::cout <<"unpackedX : ";
	//printField(&unpackedX, 32);

	engine.f1.square(x_cubed, unpackedX);
	//std::cout <<"x_squared : ";
	//printField(&x_cubed, 32);
	engine.f1.mul(x_cubed, x_cubed, unpackedX);
	//std::cout <<"x_cubed : ";
	//printField(&x_cubed, 32);

    // Add curve parameter b (3 for Alt-BN128)
    BaseElement three;
	engine.f1.fromString(three, "3");
	engine.f1.add(y_squared, x_cubed, three);
	//std::cout <<"Plus 3 (y squad): ";
	//printField(&y_squared, 32);

	BaseElement y_sqad_normal;
	engine.f1.fromMontgomery(y_sqad_normal, y_squared);
	//std::cout <<"y squad(normal): ";
	//printField(&y_sqad_normal, 32);

    if (!sqrt_calculator.sqrt(y, y_sqad_normal)) {
        throw std::runtime_error("Point not on curve - no square root exists");
    }

	//std::cout <<" y : ";
	//printField(&y, 32);

	engine.f1.neg(y_neg, y);
	//std::cout <<" y_neg : ";
	//printField(&y_neg, 32);

	BaseElement mont_y;
	//std::cout <<" y montgomery  : ";
	//printField(&mont_y, 32);

	// Check parity and negate if necessary
    if (getParityBit(y) != (parity & 1)) {
		engine.f1.neg(y, y);
    }

	engine.f1.toMontgomery(mont_y, y);

    // Copy result
	typename Engine::G1PointAffine res;
	res.x = unpackedX;
	res.y = mont_y;

	//engine.f1.toMontgomery(mont_y, y_neg);
	//std::cout <<" y montgomery  neg: ";
	//printField(&mont_y, 32);

	return res;
}


#endif //POINT_COMPRESS_HPP