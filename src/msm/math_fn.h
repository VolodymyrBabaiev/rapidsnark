#ifdef __cplusplus
extern "C" {
#endif

#include<stdint.h>
#include<string.h>

typedef uint32_t* BaseField;


uint32_t uaddCarry(uint32_t x, uint32_t y, uint32_t* carry);
uint32_t usubBorrow(uint32_t x, uint32_t y, uint32_t* borrow);
void umulExtended(uint32_t x, uint32_t y, uint32_t* msb, uint32_t* lsb);

uint64_t mp_add_n(uint64_t *r, const uint64_t *a, const uint64_t *b, size_t n);
uint64_t mp_add_1(uint64_t *r, const uint64_t *a, size_t n, uint64_t b);
void mp_add(uint64_t *r, const uint64_t *a, size_t an, const uint64_t *b, size_t bn);
uint64_t mp_sub_n(uint64_t *r, const uint64_t *a, const uint64_t *b, size_t n);
uint64_t mp_mul_1(uint64_t *r, const uint64_t *a, size_t n, uint64_t b);
uint64_t mp_addmul_1(uint64_t *r, const uint64_t *a, size_t n, uint64_t b);
//uint32_t mp_add_n(uint32_t *r, const uint32_t *a, const uint32_t *b, size_t n);
//uint32_t mp_add_1(uint32_t *r, const uint32_t *a, size_t n, uint32_t b);
//void mp_add(uint32_t *r, const uint32_t *a, size_t an, const uint32_t *b, size_t bn);
void convert_64_to_32(uint32_t *r32, const uint64_t *a64, size_t n64);
void convert_32_to_64(uint64_t *r64, const uint32_t *a32, size_t n64);
int mp_cmp(const uint64_t *a, const uint64_t *b, size_t n);
void mp_copy(uint64_t *dst, const uint64_t *src, size_t n);

#ifdef __cplusplus
}
#endif