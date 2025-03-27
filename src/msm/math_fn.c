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

#ifdef __cplusplus
}
#endif
