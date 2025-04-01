#ifndef MATH_FN_32BIT_H
#define MATH_FN_32BIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>


uint32_t uaddCarry(uint32_t x, uint32_t y, uint32_t* carry);
uint32_t usubBorrow(uint32_t x, uint32_t y, uint32_t* borrow);
void umulExtended(uint32_t x, uint32_t y, uint32_t* msb, uint32_t* lsb);

/**
 * 32-bit arithmetic helper functions
 */
uint32_t mp_add_n_32(uint32_t *r, const uint32_t *a, const uint32_t *b, size_t n);
uint32_t mp_add_1_32(uint32_t *r, const uint32_t *a, size_t n, uint32_t b);
void mp_add_32(uint32_t *r, const uint32_t *a, size_t an, const uint32_t *b, size_t bn);
uint32_t mp_sub_n_32(uint32_t *r, const uint32_t *a, const uint32_t *b, size_t n);
uint32_t mp_mul_1_32(uint32_t *r, const uint32_t *a, size_t n, uint32_t b);
uint32_t mp_addmul_1_32(uint32_t *r, const uint32_t *a, size_t n, uint32_t b);
int mp_cmp_32(const uint32_t *a, const uint32_t *b, size_t n);
void mp_copy_32(uint32_t *dst, const uint32_t *src, size_t n);
int mp_is_zero_32(const uint32_t *a, size_t n);

void convert_64_to_32(uint32_t *r32, const uint64_t *a64, size_t n64);
void convert_32_to_64(uint64_t *r64, const uint32_t *a32, size_t n64);

#ifdef __cplusplus
}
#endif

#endif /* MATH_FN_32BIT_H */
