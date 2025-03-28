//
// Created by volodymyr on 3/6/25.
//
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

/**
 * Helper function: Add two multi-precision numbers with carry
 * a and b must have the same size, result is stored in r
 * Returns the final carry
 */
uint64_t mp_add_n(uint64_t *r, const uint64_t *a, const uint64_t *b, size_t n) {
    uint64_t carry = 0;
    for (size_t i = 0; i < n; i++) {
        uint64_t sum = a[i] + b[i] + carry;
        carry = (sum < a[i]) || ((sum == a[i]) && (b[i] > 0));
        r[i] = sum;
    }
    return carry;
}

/**
 * Helper function: Add two multi-precision numbers with carry using 32-bit operations
 * a and b must have the same size, result is stored in r
 * Returns the final carry
 */
//uint32_t mp_add_n(uint32_t *r, const uint32_t *a, const uint32_t *b, size_t n) {
//    uint32_t carry = 0;
//    for (size_t i = 0; i < n; i++) {
//        uint32_t sum = a[i] + b[i] + carry;
//        carry = (sum < a[i]) || ((sum == a[i]) && (b[i] > 0));
//        r[i] = sum;
//    }
//    return carry;
//}

/**
 * Helper function: Add a single limb to a multi-precision number with carry
 * Result is stored in r
 * Returns the final carry
 */
uint64_t mp_add_1(uint64_t *r, const uint64_t *a, size_t n, uint64_t b) {
    uint64_t carry = b;
    for (size_t i = 0; i < n && carry; i++) {
        uint64_t sum = a[i] + carry;
        carry = sum < a[i];
        r[i] = sum;
    }
    if (carry == 0 && r != a) {
        // Just copy the remaining limbs
        memcpy(r, a + (r - a), (n - (r - a)) * sizeof(uint64_t));
    }
    return carry;
}

/**
 * Helper function: Add a single limb to a multi-precision number with carry
 * Result is stored in r
 * Returns the final carry
 */
//uint32_t mp_add_1(uint32_t *r, const uint32_t *a, size_t n, uint32_t b) {
//    uint32_t carry = b;
//    for (size_t i = 0; i < n && carry; i++) {
//        uint32_t sum = a[i] + carry;
//        carry = sum < a[i];
//        r[i] = sum;
//    }
//    if (carry == 0 && r != a) {
//        // Just copy the remaining limbs
//        memcpy(r, a + (r - a), (n - (r - a)) * sizeof(uint32_t));
//    }
//    return carry;
//}

/**
 * Helper function: Add one multi-precision number to another
 * r = a + b
 * a and b can have different sizes
 */
void mp_add(uint64_t *r, const uint64_t *a, size_t an, const uint64_t *b, size_t bn) {
    size_t common = (an < bn) ? an : bn;

    uint64_t carry = mp_add_n(r, a, b, common);

    if (an > common) {
        carry = mp_add_1(r + common, a + common, an - common, carry);
    } else if (bn > common) {
        carry = mp_add_1(r + common, b + common, bn - common, carry);
    }

    if (carry && an >= bn) {
        r[an] = carry;
    }
}

/**
 * Helper function: Add one multi-precision number to another
 * r = a + b
 * a and b can have different sizes
 */
//void mp_add(uint32_t *r, const uint32_t *a, size_t an, const uint32_t *b, size_t bn) {
//    size_t common = (an < bn) ? an : bn;
//
//    uint32_t carry = mp_add_n(r, a, b, common);
//
//    if (an > common) {
//        carry = mp_add_1(r + common, a + common, an - common, carry);
//    } else if (bn > common) {
//        carry = mp_add_1(r + common, b + common, bn - common, carry);
//    }
//
//    if (carry && an >= bn) {
//        r[an] = carry;
//    }
//}

/**
 * Helper function: Subtract two multi-precision numbers with borrow
 * a and b must have the same size, result is stored in r
 * Returns the final borrow (1 if a < b, 0 otherwise)
 */
uint64_t mp_sub_n(uint64_t *r, const uint64_t *a, const uint64_t *b, size_t n) {
    uint64_t borrow = 0;
    for (size_t i = 0; i < n; i++) {
        uint64_t tmp = a[i] - b[i] - borrow;
        borrow = (tmp > a[i]) || ((tmp == a[i]) && (borrow > 0));
        r[i] = tmp;
    }
    return borrow;
}

/**
 * Helper function: Multiply a multi-precision number by a single limb
 * Result is stored in r
 * Returns the carry
 */
uint64_t mp_mul_1(uint64_t *r, const uint64_t *a, size_t n, uint64_t b) {
    uint64_t carry = 0;
    for (size_t i = 0; i < n; i++) {
        // Full multiplication of two 64-bit integers
        // Requires using 128-bit arithmetic, which we'll simulate
        uint64_t low;
        uint64_t high;

        // Compute the low 64 bits of the product
        //low = a[i] * b;

        // Compute the high 64 bits of the product
        // Split into 32-bit chunks to avoid overflow
        uint64_t a_lo = a[i] & 0xFFFFFFFFULL;
        uint64_t a_hi = a[i] >> 32;
        uint64_t b_lo = b & 0xFFFFFFFFULL;
        uint64_t b_hi = b >> 32;

        low = a_lo * b_lo;

        // Cross products
        uint64_t cross1 = a_lo * b_hi;
        uint64_t cross2 = a_hi * b_lo;

        // High product
        uint64_t high_prod = a_hi * b_hi;

        // Add cross products to the high word
        high = high_prod + (cross1 >> 32) + (cross2 >> 32);

        // Handle carry from lower 32 bits of cross products
//        uint64_t cross_low = ((cross1 & 0xFFFFFFFFULL) + (cross2 & 0xFFFFFFFFULL)) << 32;
//        uint64_t sum = low + cross_low;
//        if (sum < low) {
//            high++;
//        }
//        low = sum;

        uint64_t sum = low + ((cross1 & 0xFFFFFFFFULL) << 32);
        if (sum < low) {
            high++;
        }
        low = sum;

		sum = low + ((cross2 & 0xFFFFFFFFULL) << 32);
        if (sum < low) {
            high++;
        }
        low = sum;

        // Add the carry from the previous iteration
        sum = low + carry;
        if (sum < low) {
            high++;
        }
        r[i] = sum;
        carry = high;
    }
    return carry;
}

/**
 * Helper function: Multiply a multi-precision number by a single limb and add to another
 * r += a * b
 * Returns the final carry
 */
uint64_t mp_addmul_1(uint64_t *r, const uint64_t *a, size_t n, uint64_t b) {
    uint64_t carry = 0;
    for (size_t i = 0; i < n; i++) {
        // Full multiplication of two 64-bit integers
        uint64_t low;
        uint64_t high;

        // Compute the high 64 bits of the product
        uint64_t a_lo = a[i] & 0xFFFFFFFFULL;
        uint64_t a_hi = a[i] >> 32;
        uint64_t b_lo = b & 0xFFFFFFFFULL;
        uint64_t b_hi = b >> 32;

		// Compute the low 64 bits of the product
        low = a_lo * b_lo;

        // Cross products
        uint64_t cross1 = a_lo * b_hi;
        uint64_t cross2 = a_hi * b_lo;

        // High product
        uint64_t high_prod = a_hi * b_hi;

        // Add cross products to the high word
        high = high_prod + (cross1 >> 32) + (cross2 >> 32);

        // Handle carry from lower 32 bits of cross products
        uint64_t sum = low + ((cross1 & 0xFFFFFFFFULL) << 32);
        if (sum < low) {
            high++;
        }
        low = sum;

        sum = low + ((cross2 & 0xFFFFFFFFULL) << 32);
        if (sum < low) {
            high++;
        }
        low = sum;

        // Add the product to r[i] with carry from the previous iteration
//        sum = r[i] + low + carry;
//        if (sum < r[i] || (sum == r[i] && (low > 0 || carry > 0))) {
//            high++;
//        }
        sum = low + carry;
        if (sum < low) {
          high++;
        }
        low = sum;

        sum = r[i] + low;
        if (sum < r[i]) {
          high++;
        }

        r[i] = sum;
        carry = high;
    }
    return carry;
}

/**
 * Helper function: Compare two multi-precision numbers
 * Returns < 0 if a < b, 0 if a == b, > 0 if a > b
 */
int mp_cmp(const uint64_t *a, const uint64_t *b, size_t n) {
    for (int i = n - 1; i >= 0; i--) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return -1;
    }
    return 0;
}

/**
 * Helper function: Copy a multi-precision number
 */
void mp_copy(uint64_t *dst, const uint64_t *src, size_t n) {
    memcpy(dst, src, n * sizeof(uint64_t));
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

#ifdef __cplusplus
}
#endif
