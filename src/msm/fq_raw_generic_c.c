/**
 * fq_raw_generic.c - Implementation of raw Fq field operations in C
 * Converted from C++ implementation
 */

#include "fq_c.h"
#include "math_fn.h"
#include <string.h>

// Static constants
static uint64_t Fq_rawq_static[] = {0x3c208c16d87cfd47, 0x97816a916871ca8d, 0xb85045b68181585d, 0x30644e72e131a029, 0};
static FqRawElement Fq_rawR2_static = {0xf32cfc5b538afa89, 0xb5e71911d44501fb, 0x47ab1eff0a417ff6, 0x06d89f71cab8351f};
static uint64_t Fq_np_static = 0x87d20782e4866389;
static uint64_t lboMask_static = 0x3fffffffffffffff;

void Fq_rawCopy(FqRawElement pRawResult, const FqRawElement pRawA) {
    pRawResult[0] = pRawA[0];
    pRawResult[1] = pRawA[1];
    pRawResult[2] = pRawA[2];
    pRawResult[3] = pRawA[3];
}

void Fq_rawCopyS2L(FqRawElement pRawResult, int64_t val)
{
    pRawResult[0] = val;
    pRawResult[1] = 0;
    pRawResult[2] = 0;
    pRawResult[3] = 0;

    if (val < 0)
    {
        pRawResult[1] = -1;
        pRawResult[2] = -1;
        pRawResult[3] = -1;

        mp_add_n_64(pRawResult, pRawResult, Fq_rawq, Fq_N64);
    }
}

void Fq_rawAdd(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB)
{
    uint64_t carry = mp_add_n_64(pRawResult, pRawA, pRawB, Fq_N64);

    if(carry || mp_cmp(pRawResult, Fq_rawq_static, Fq_N64) >= 0)
    {
        mp_sub_n_64(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawAddLS(FqRawElement pRawResult, FqRawElement pRawA, uint64_t rawB)
{
    uint64_t carry = mp_add_1_64(pRawResult, pRawA, Fq_N64, rawB);

    if(carry || mp_cmp(pRawResult, Fq_rawq_static, Fq_N64) >= 0)
    {
        mp_sub_n_64(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawSub(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB)
{
    uint64_t carry = mp_sub_n_64(pRawResult, pRawA, pRawB, Fq_N64);

    if(carry)
    {
        mp_add_n_64(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawSubRegular(FqRawElement pRawResult, FqRawElement pRawA, FqRawElement pRawB)
{
    mp_sub_n_64(pRawResult, pRawA, pRawB, Fq_N64);
}

void Fq_rawSubSL(FqRawElement pRawResult, uint64_t rawA, FqRawElement pRawB)
{
    FqRawElement pRawA = {rawA, 0, 0, 0};

    uint64_t carry = mp_sub_n_64(pRawResult, pRawA, pRawB, Fq_N64);

    if(carry)
    {
        mp_add_n_64(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawSubLS(FqRawElement pRawResult, FqRawElement pRawA, uint64_t rawB)
{
    uint64_t carry = mp_sub_1_64(pRawResult, pRawA, Fq_N64, rawB);

    if(carry)
    {
        mp_add_n_64(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawNeg(FqRawElement pRawResult, const FqRawElement pRawA)
{
    const uint64_t zero[Fq_N64] = {0, 0, 0, 0};

    if (mp_cmp(pRawA, zero, Fq_N64) != 0)
    {
        mp_sub_n_64(pRawResult, Fq_rawq_static, pRawA, Fq_N64);
    }
    else
    {
        mp_copy(pRawResult, zero, Fq_N64);
    }
}

/* Substracts a long element and a short element form 0 */
void Fq_rawNegLS(FqRawElement pRawResult, FqRawElement pRawA, uint64_t rawB)
{
    uint64_t carry1 = mp_sub_1_64(pRawResult, Fq_rawq_static, Fq_N64, rawB);
    uint64_t carry2 = mp_sub_n_64(pRawResult, pRawResult, pRawA, Fq_N64);

    if (carry1 || carry2)
    {
        mp_add_n_64(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

int Fq_rawIsEq(const FqRawElement pRawA, const FqRawElement pRawB)
{
    return mp_cmp(pRawA, pRawB, Fq_N64) == 0;
}

void Fq_rawMMul(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB)
{
    const uint64_t  N = Fq_N64+1;
    const uint64_t  *mq = Fq_rawq_static;

    uint64_t  np0;

    uint64_t  product0[N];
    uint64_t  product1[N];
    uint64_t  product2[N];
    uint64_t  product3[N];

    memset(product0, 0, N*sizeof(uint64_t));
    memset(product1, 0, N*sizeof(uint64_t));
    memset(product2, 0, N*sizeof(uint64_t));
    memset(product3, 0, N*sizeof(uint64_t));

    product0[4] = mp_mul_1_64(product0, pRawB, Fq_N64, pRawA[0]);

    np0 = Fq_np_static * product0[0];
    product1[1] = mp_addmul_1_64(product0, mq, N, np0);

    product1[4] = mp_addmul_1_64(product1, pRawB, Fq_N64, pRawA[1]);
    mp_add_64(product1, product1, N, product0+1, N-1);

    np0 = Fq_np_static * product1[0];
    product2[1] = mp_addmul_1_64(product1, mq, N, np0);

    product2[4] = mp_addmul_1_64(product2, pRawB, Fq_N64, pRawA[2]);
    mp_add_64(product2, product2, N, product1+1, N-1);

    np0 = Fq_np_static * product2[0];
    product3[1] = mp_addmul_1_64(product2, mq, N, np0);

    product3[4] = mp_addmul_1_64(product3, pRawB, Fq_N64, pRawA[3]);
    mp_add_64(product3, product3, N, product2+1, N-1);

    np0 = Fq_np_static * product3[0];
    mp_addmul_1_64(product3, mq, N, np0);

    mp_copy(pRawResult, product3+1, Fq_N64);

    if (mp_cmp(pRawResult, mq, Fq_N64) >= 0)
    {
        mp_sub_n_64(pRawResult, pRawResult, mq, Fq_N64);
    }
}

void Fq_rawMSquare(FqRawElement pRawResult, const FqRawElement pRawA)
{
    Fq_rawMMul(pRawResult, pRawA, pRawA);
}

void Fq_rawMMul1(FqRawElement pRawResult, const FqRawElement pRawA, uint64_t pRawB)
{
    const uint64_t  N = Fq_N64+1;
    const uint64_t  *mq = Fq_rawq_static;

    uint64_t  np0;

    uint64_t  product0[N];
    uint64_t  product1[N];
    uint64_t  product2[N];
    uint64_t  product3[N];

    memset(product0, 0, N*sizeof(uint64_t));
    memset(product1, 0, N*sizeof(uint64_t));
    memset(product2, 0, N*sizeof(uint64_t));
    memset(product3, 0, N*sizeof(uint64_t));

    product0[4] = mp_mul_1_64(product0, pRawA, Fq_N64, pRawB);

    np0 = Fq_np_static * product0[0];
    product1[1] = mp_addmul_1_64(product0, mq, N, np0);
    mp_add_64(product1, product1, N, product0+1, N-1);

    np0 = Fq_np_static * product1[0];
    product2[1] = mp_addmul_1_64(product1, mq, N, np0);
    mp_add_64(product2, product2, N, product1+1, N-1);

    np0 = Fq_np_static * product2[0];
    product3[1] = mp_addmul_1_64(product2, mq, N, np0);
    mp_add_64(product3, product3, N, product2+1, N-1);

    np0 = Fq_np_static * product3[0];
    mp_addmul_1_64(product3, mq, N, np0);

    mp_copy(pRawResult, product3+1, Fq_N64);

    if (mp_cmp(pRawResult, mq, Fq_N64) >= 0)
    {
        mp_sub_n_64(pRawResult, pRawResult, mq, Fq_N64);
    }
}

void Fq_rawToMontgomery(FqRawElement pRawResult, const FqRawElement pRawA)
{
    Fq_rawMMul(pRawResult, pRawA, Fq_rawR2_static);
}

int Fq_rawIsZero(const FqRawElement pRawB) {
    for (int i = 0; i < Fq_N64; i++) {
        if (pRawB[i] != 0) {
            return 0;
        }
    }
    return 1;
}
