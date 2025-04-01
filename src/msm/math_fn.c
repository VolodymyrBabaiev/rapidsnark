#include "math_fn.h"

#ifdef __cplusplus
extern "C" {
#endif


uint32_t uaddCarry(uint32_t x, uint32_t y, uint32_t* carry) {
    uint64_t sum = (uint64_t)x + (uint64_t)y;  // Use 64-bit to detect overflow
    *carry = (sum >> 32) & 1;  // Extract carry (1 if overflow, 0 otherwise)
    return (uint32_t)sum;  // Return the lower 32 bits
}


uint32_t usubBorrow(uint32_t x, uint32_t y, uint32_t* borrow) {
    uint64_t diff = (uint64_t)x - (uint64_t)y;  // Use 64-bit to detect borrow/underflow
    *borrow = (diff >> 32) & 1;  // Extract borrow (1 if underflow, 0 otherwise)
    return (uint32_t)diff;  // Return the lower 32 bits
}


void umulExtended(uint32_t x, uint32_t y, uint32_t* msb, uint32_t* lsb) {
    uint64_t product = (uint64_t)x * (uint64_t)y;  // Perform 64-bit multiplication
    *lsb = (uint32_t)product;                      // Extract the lower 32 bits
    *msb = (uint32_t)(product >> 32);              // Extract the higher 32 bits
}

///**
// * Helper function: Convert 64-bit array to 32-bit array
// */
//void convert_64_to_32(uint32_t *r32, const uint64_t *a64, size_t n64) {
//    for (size_t i = 0; i < n64; i++) {
//        r32[i*2] = (uint32_t)(a64[i] & 0xFFFFFFFFUL);         // Low 32 bits
//        r32[i*2 + 1] = (uint32_t)((a64[i] >> 32) & 0xFFFFFFFFUL);  // High 32 bits
//    }
//}
//
///**
// * Helper function: Convert 32-bit array to 64-bit array
// */
//void convert_32_to_64(uint64_t *r64, const uint32_t *a32, size_t n64) {
//    for (size_t i = 0; i < n64; i++) {
//        r64[i] = ((uint64_t)a32[i*2 + 1] << 32) | a32[i*2];
//    }
//}

/**
 * Helper function: Add two multi-precision numbers with carry using 32-bit operations
 * a and b must have the same size, result is stored in r
 * Returns the final carry
 */
uint32_t mp_add_n_32(uint32_t *r, const uint32_t *a, const uint32_t *b, size_t n) {
    uint32_t carry = 0;
    for (size_t i = 0; i < n; i++) {
        uint32_t temp_carry1 = 0;
        uint32_t temp_carry2 = 0;
        uint32_t sum = uaddCarry(a[i], b[i], &temp_carry1);
        r[i] = uaddCarry(sum, carry, &temp_carry2);
        carry = uaddCarry(temp_carry1, temp_carry2, &temp_carry1);
    }
    return carry;
}

/**
 * Helper function: Add a single limb to a multi-precision number with carry
 * Result is stored in r
 * Returns the final carry
 */
uint32_t mp_add_1_32(uint32_t *r, const uint32_t *a, size_t n, uint32_t b) {
    uint32_t carry = 0;
    uint32_t sum = uaddCarry(a[0], b, &carry);
    r[0] = sum;

    size_t i = 1;
    for (; i < n && carry; i++) {
        sum = uaddCarry(a[i], carry, &carry);
        r[i] = sum;
    }

    if (carry == 0 && r != a) {
        // Just copy the remaining limbs
        for (; i < n; i++) {
            r[i] = a[i];
        }
    }
    return carry;
}

/**
 * Helper function: Add one multi-precision number to another
 * r = a + b
 * a and b can have different sizes
 */
void mp_add_32(uint32_t *r, const uint32_t *a, size_t an, const uint32_t *b, size_t bn) {
    size_t common = (an < bn) ? an : bn;

    uint32_t carry = mp_add_n_32(r, a, b, common);

    if (an > common) {
        carry = mp_add_1_32(r + common, a + common, an - common, carry);
    } else if (bn > common) {
        carry = mp_add_1_32(r + common, b + common, bn - common, carry);
    }

    if (carry && an >= bn) {
        r[an] = carry;
    }
}

/**
 * Helper function: Subtract two multi-precision numbers with borrow
 * a and b must have the same size, result is stored in r
 * Returns the final borrow (1 if a < b, 0 otherwise)
 */
uint32_t mp_sub_n_32(uint32_t *r, const uint32_t *a, const uint32_t *b, size_t n) {
    uint32_t borrow = 0;
    for (size_t i = 0; i < n; i++) {
        uint32_t diff;
        uint32_t temp_borrow1 = 0;
        uint32_t temp_borrow2 = 0;
        diff = usubBorrow(a[i], b[i], &temp_borrow1);
        r[i] = usubBorrow(diff, borrow, &temp_borrow2);
        borrow = temp_borrow1 + temp_borrow2;
    }
    return borrow;
}

/**
 * Helper function: Multiply a multi-precision number by a single limb
 * Result is stored in r
 * Returns the carry
 */
uint32_t mp_mul_1_32(uint32_t *r, const uint32_t *a, size_t n, uint32_t b) {
    uint32_t carry = 0;
    for (size_t i = 0; i < n; i++) {
        uint32_t high, low;
        umulExtended(a[i], b, &high, &low);

        uint32_t temp_carry = 0;
        uint32_t sum = uaddCarry(low, carry, &temp_carry);
        r[i] = sum;

        carry = uaddCarry(high, temp_carry, &temp_carry);
    }
    return carry;
}

/**
 * Helper function: Multiply a multi-precision number by a single limb and add to another
 * r += a * b
 * Returns the final carry
 */
uint32_t mp_addmul_1_32(uint32_t *r, const uint32_t *a, size_t n, uint32_t b) {
    uint32_t carry = 0;
    for (size_t i = 0; i < n; i++) {
        uint32_t high, low;
        umulExtended(a[i], b, &high, &low);

        // Add the product to r[i] with carry
        uint32_t temp_carry1 = 0;
        uint32_t sum1 = uaddCarry(low, carry, &temp_carry1);

        uint32_t temp_carry2 = 0;
        uint32_t sum2 = uaddCarry(r[i], sum1, &temp_carry2);
        r[i] = sum2;

        // Accumulate carries
        carry = high + temp_carry1 + temp_carry2;
    }
    return carry;
}

/**
 * Helper function: Compare two multi-precision numbers
 * Returns < 0 if a < b, 0 if a == b, > 0 if a > b
 */
int mp_cmp_32(const uint32_t *a, const uint32_t *b, size_t n) {
    for (int i = n - 1; i >= 0; i--) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return -1;
    }
    return 0;
}

/**
 * Helper function: Copy a multi-precision number
 */
void mp_copy_32(uint32_t *dst, const uint32_t *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

/**
 * Helper function: Convert 64-bit array to 32-bit array
 */
void convert_64_to_32(uint32_t *r32, const uint64_t *a64, size_t n64) {
    for (size_t i = 0; i < n64; i++) {
        r32[i*2] = (uint32_t)(a64[i] & 0xFFFFFFFFUL);         // Low 32 bits
        r32[i*2 + 1] = (uint32_t)((a64[i] >> 32) & 0xFFFFFFFFUL);  // High 32 bits
    }
}

/**
 * Helper function: Convert 32-bit array to 64-bit array
 */
void convert_32_to_64(uint64_t *r64, const uint32_t *a32, size_t n64) {
    for (size_t i = 0; i < n64; i++) {
        r64[i] = ((uint64_t)a32[i*2 + 1] << 32) | a32[i*2];
    }
}

/**
 * Helper function: Check if a multi-precision number is zero
 */
int mp_is_zero_32(const uint32_t *a, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != 0) return 0;
    }
    return 1;
}

#ifdef __cplusplus
}
#endif
