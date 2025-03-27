#ifdef __cplusplus
extern "C" {
#endif

#include<stdint.h>

typedef uint32_t* BaseField;


uint32_t uaddCarry(uint32_t x, uint32_t y, uint32_t* carry);
uint32_t usubBorrow(uint32_t x, uint32_t y, uint32_t* borrow);
void umulExtended(uint32_t x, uint32_t y, uint32_t* msb, uint32_t* lsb);


#ifdef __cplusplus
}
#endif