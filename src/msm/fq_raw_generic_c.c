/**
 * fq_raw_generic.c - Implementation of raw Fq field operations in C
 * Converted from C++ implementation
 */

#include "fq_c.h"
#include "math_fn.h"
#include <gmp.h>
#include <string.h>
#include <stdio.h>

// Static constants
static uint64_t Fq_rawq_static[] = {0x3c208c16d87cfd47, 0x97816a916871ca8d, 0xb85045b68181585d, 0x30644e72e131a029, 0};
static FqRawElement Fq_rawR2_static = {0xf32cfc5b538afa89, 0xb5e71911d44501fb, 0x47ab1eff0a417ff6, 0x06d89f71cab8351f};
static uint64_t Fq_np_static = 0x87d20782e4866389;
static uint64_t lboMask_static = 0x3fffffffffffffff;

void Fq_rawAdd(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB)
{
    uint64_t carry = mpn_add_n(pRawResult, pRawA, pRawB, Fq_N64);

    if(carry || mpn_cmp(pRawResult, Fq_rawq_static, Fq_N64) >= 0)
    {
        mpn_sub_n(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawAddLS(FqRawElement pRawResult, FqRawElement pRawA, uint64_t rawB)
{
    uint64_t carry = mpn_add_1(pRawResult, pRawA, Fq_N64, rawB);

    if(carry || mpn_cmp(pRawResult, Fq_rawq_static, Fq_N64) >= 0)
    {
        mpn_sub_n(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawSub(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB)
{
    uint64_t carry = mpn_sub_n(pRawResult, pRawA, pRawB, Fq_N64);

    if(carry)
    {
        mpn_add_n(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawSubRegular(FqRawElement pRawResult, FqRawElement pRawA, FqRawElement pRawB)
{
    mpn_sub_n(pRawResult, pRawA, pRawB, Fq_N64);
}

void Fq_rawSubSL(FqRawElement pRawResult, uint64_t rawA, FqRawElement pRawB)
{
    FqRawElement pRawA = {rawA, 0, 0, 0};

    uint64_t carry = mpn_sub_n(pRawResult, pRawA, pRawB, Fq_N64);

    if(carry)
    {
        mpn_add_n(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawSubLS(FqRawElement pRawResult, FqRawElement pRawA, uint64_t rawB)
{
    uint64_t carry = mpn_sub_1(pRawResult, pRawA, Fq_N64, rawB);

    if(carry)
    {
        mpn_add_n(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawNeg(FqRawElement pRawResult, const FqRawElement pRawA)
{
    const uint64_t zero[Fq_N64] = {0, 0, 0, 0};

    if (mpn_cmp(pRawA, zero, Fq_N64) != 0)
    {
        mpn_sub_n(pRawResult, Fq_rawq_static, pRawA, Fq_N64);
    }
    else
    {
        mpn_copyi(pRawResult, zero, Fq_N64);
    }
}

/* Substracts a long element and a short element form 0 */
void Fq_rawNegLS(FqRawElement pRawResult, FqRawElement pRawA, uint64_t rawB)
{
    uint64_t carry1 = mpn_sub_1(pRawResult, Fq_rawq_static, Fq_N64, rawB);
    uint64_t carry2 = mpn_sub_n(pRawResult, pRawResult, pRawA, Fq_N64);

    if (carry1 || carry2)
    {
        mpn_add_n(pRawResult, pRawResult, Fq_rawq_static, Fq_N64);
    }
}

void Fq_rawCopy(FqRawElement pRawResult, const FqRawElement pRawA)
{
    pRawResult[0] = pRawA[0];
    pRawResult[1] = pRawA[1];
    pRawResult[2] = pRawA[2];
    pRawResult[3] = pRawA[3];
}

int Fq_rawIsEq(const FqRawElement pRawA, const FqRawElement pRawB)
{
    return mpn_cmp(pRawA, pRawB, Fq_N64) == 0;
}

//void Fq_rawMMul_64(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB)
//{
//    const mp_size_t  N = Fq_N64+1;
//    const uint64_t  *mq = Fq_rawq_static;
//
//    uint64_t  np0;
//
//    uint64_t  product0[N];
//    uint64_t  product1[N];
//    uint64_t  product2[N];
//    uint64_t  product3[N];
//
//    memset(product0, 0, N*sizeof(uint64_t));
//    memset(product1, 0, N*sizeof(uint64_t));
//    memset(product2, 0, N*sizeof(uint64_t));
//    memset(product3, 0, N*sizeof(uint64_t));
//
//    product0[4] = mp_mul_1(product0, pRawB, Fq_N64, pRawA[0]);
//
//    np0 = Fq_np_static * product0[0];
//    product1[1] = mp_addmul_1(product0, mq, N, np0);
//
//    product1[4] = mp_addmul_1(product1, pRawB, Fq_N64, pRawA[1]);
//    mp_add(product1, product1, N, product0+1, N-1);
//
//    np0 = Fq_np_static * product1[0];
//    product2[1] = mp_addmul_1(product1, mq, N, np0);
//
//    product2[4] = mp_addmul_1(product2, pRawB, Fq_N64, pRawA[2]);
//    mp_add(product2, product2, N, product1+1, N-1);
//
//    np0 = Fq_np_static * product2[0];
//    product3[1] = mp_addmul_1(product2, mq, N, np0);
//
//    product3[4] = mp_addmul_1(product3, pRawB, Fq_N64, pRawA[3]);
//    mp_add(product3, product3, N, product2+1, N-1);
//
//    np0 = Fq_np_static * product3[0];
//    mp_addmul_1(product3, mq, N, np0);
//
//    mp_copy(pRawResult, product3+1, Fq_N64);
//
//    if (mp_cmp(pRawResult, mq, Fq_N64) >= 0)
//    {
//        mp_sub_n(pRawResult, pRawResult, mq, Fq_N64);
//    }
//}



// Define the number of 32-bit limbs (twice the number of 64-bit limbs)
#define Fq_N32 8

// Convert static constants from 64-bit to 32-bit
static const uint32_t Fq_rawq_32[Fq_N32+2] = {
    0xd87cfd47, 0x3c208c16, // Limbs 0 and 1 (from 64-bit limb 0)
    0x6871ca8d, 0x97816a91, // Limbs 2 and 3 (from 64-bit limb 1)
    0x8181585d, 0xb85045b6, // Limbs 4 and 5 (from 64-bit limb 2)
    0xe131a029, 0x30644e72, // Limbs 6 and 7 (from 64-bit limb 3)
    0, 0                    // Extra limbs for calculations
};

static const uint32_t Fq_rawR2_32[Fq_N32] = {
    0x538afa89, 0xf32cfc5b, // Limbs 0 and 1 (from 64-bit limb 0)
    0xd44501fb, 0xb5e71911, // Limbs 2 and 3 (from 64-bit limb 1)
    0x0a417ff6, 0x47ab1eff, // Limbs 4 and 5 (from 64-bit limb 2)
    0xcab8351f, 0x06d89f71  // Limbs 6 and 7 (from 64-bit limb 3)
};

// The 32-bit version of np_static for Montgomery reduction
static const uint32_t Fq_np_32 = 0xe4866389; // Lower 32 bits of 0x87d20782e4866389

/**
 * Montgomery multiplication implementation using 32-bit arithmetic
 */
void Fq_rawMMul(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB) {
    // Convert inputs to 32-bit representation
    uint32_t rawA32[Fq_N32];
    uint32_t rawB32[Fq_N32];
    convert_64_to_32(rawA32, pRawA, Fq_N64);
    convert_64_to_32(rawB32, pRawB, Fq_N64);

    const size_t N = Fq_N32 + 2;  // Need two extra limbs for 32-bit operations
    uint32_t np0;

    // Temporary products for each step of Montgomery reduction
    uint32_t product[N];
    uint32_t temp[N];

    // Clear the product array
    memset(product, 0, N * sizeof(uint32_t));

    // Montgomery multiplication loop - process each 32-bit limb of pRawA
    for (size_t i = 0; i < Fq_N32; i++) {
        // Multiply the current limb of A with all limbs of B and add to product
        uint32_t carry = mp_mul_1_32(temp, rawB32, Fq_N32, rawA32[i]);
        temp[Fq_N32] = carry;
        temp[Fq_N32+1] = 0;

        carry = mp_add_n_32(product, product, temp, N);

        // Calculate the quotient limb q = product[0] * np mod 2^32
        np0 = product[0] * Fq_np_32;

        // Multiply q by the modulus and add to the product (Montgomery reduction step)
        carry = mp_mul_1_32(temp, Fq_rawq_32, N, np0);
        carry = mp_add_n_32(product, product, temp, N);

        // Divide by 2^32 (shift right by one 32-bit limb)
        for (size_t j = 0; j < N-1; j++) {
            product[j] = product[j+1];
        }
        product[N-1] = 0;
    }

    // Final conditional subtraction
    uint32_t result32[Fq_N32];
    mp_copy_32(result32, product, Fq_N32);

    if (mp_cmp_32(result32, Fq_rawq_32, Fq_N32) >= 0) {
        printf("Substraction!");
        mp_sub_n_32(result32, result32, Fq_rawq_32, Fq_N32);
    } else {
      printf("Clear");
    }

    // Convert back to 64-bit representation
    convert_32_to_64(pRawResult, result32, Fq_N64);
}

static const uint32_t Fq_rawq32[] = {
    0xd87cfd47, 0x3c208c16,
    0x6871ca8d, 0x97816a91,
    0x8181585d, 0xb85045b6,
    0xe131a029, 0x30644e72
};
static const uint32_t Fq_np32 = 0xe4866389;

void Fq_rawMMul_old(FqRawElement pRawResult, const FqRawElement pRawA, const FqRawElement pRawB) {
    // Convert inputs to 32-bit representation
    uint32_t rawA32[Fq_N32];
    uint32_t rawB32[Fq_N32];
    convert_64_to_32(rawA32, pRawA, Fq_N64);
    convert_64_to_32(rawB32, pRawB, Fq_N64);

    const size_t N = Fq_N32 + 1;  // Need two extra limbs for 32-bit operations
    const uint32_t *mq = Fq_rawq32;

    uint32_t np0;

    uint32_t product0[N];
    uint32_t product1[N];
    uint32_t product2[N];
    uint32_t product3[N];
    uint32_t product4[N];
    uint32_t product5[N];
    uint32_t product6[N];
    uint32_t product7[N];
    uint32_t product8[N];

    memset(product0, 0, N * sizeof(uint32_t));
    memset(product1, 0, N * sizeof(uint32_t));
    memset(product2, 0, N * sizeof(uint32_t));
    memset(product3, 0, N * sizeof(uint32_t));
    memset(product4, 0, N * sizeof(uint32_t));
    memset(product5, 0, N * sizeof(uint32_t));
    memset(product6, 0, N * sizeof(uint32_t));
    memset(product7, 0, N * sizeof(uint32_t));
    memset(product8, 0, N * sizeof(uint32_t));

    // Process each 32-bit limb of pRawA
    // First limb
    product0[N-1] = mp_mul_1_32(product0, rawB32, Fq_N32, rawA32[0]);
    np0 = Fq_np32 * product0[0];
    product1[1] = mp_addmul_1_32(product0, mq, N-1, np0);

    // Second limb
    product1[N-1] = mp_addmul_1_32(product1, rawB32, Fq_N32, rawA32[1]);
    mp_add_32(product1, product1, N, product0+1, N-1);
    np0 = Fq_np_32 * product1[0];
    product2[1] = mp_addmul_1_32(product1, mq, N-1, np0);

    // Third limb
    product2[N-1] = mp_addmul_1_32(product2, rawB32, Fq_N32, rawA32[2]);
    mp_add_32(product2, product2, N, product1+1, N-1);
    np0 = Fq_np32 * product2[0];
    product3[1] = mp_addmul_1_32(product2, mq, N-1, np0);

    // Fourth limb
    product3[N-1] = mp_addmul_1_32(product3, rawB32, Fq_N32, rawA32[3]);
    mp_add_32(product3, product3, N, product2+1, N-1);
    np0 = Fq_np_32 * product3[0];
    product4[1] = mp_addmul_1_32(product3, mq, N-1, np0);

    // Fifth limb
    product4[N-1] = mp_addmul_1_32(product4, rawB32, Fq_N32, rawA32[4]);
    mp_add_32(product4, product4, N, product3+1, N-1);
    np0 = Fq_np32 * product4[0];
    product5[1] = mp_addmul_1_32(product4, mq, N-1, np0);

    // Sixth limb
    product5[N-1] = mp_addmul_1_32(product5, rawB32, Fq_N32, rawA32[5]);
    mp_add_32(product5, product5, N, product4+1, N-1);
    np0 = Fq_np32 * product5[0];
    product6[1] = mp_addmul_1_32(product5, mq, N-1, np0);

    // Seventh limb
    product6[N-1] = mp_addmul_1_32(product6, rawB32, Fq_N32, rawA32[6]);
    mp_add_32(product6, product6, N, product5+1, N-1);
    np0 = Fq_np32 * product6[0];
    product7[1] = mp_addmul_1_32(product6, mq, N-1, np0);

    // Eighth limb
    product7[N-1] = mp_addmul_1_32(product7, rawB32, Fq_N32, rawA32[7]);
    mp_add_32(product7, product7, N, product6+1, N-1);
    np0 = Fq_np32 * product7[0];
    product8[1] = mp_addmul_1_32(product7, mq, N-1, np0);

    mp_add_32(product8, product8, N, product7+1, N-1);

    // Final reduction
    uint32_t result32[Fq_N32];
    memset(result32, 0, Fq_N32 * sizeof(uint32_t));
    mp_copy_32(result32, product8+1, Fq_N32);

    if (mp_cmp_32(result32, mq, Fq_N32) >= 0)
      mp_sub_n_32(result32, result32, mq, Fq_N32);

    // Convert back to 64-bit representation
    convert_32_to_64(pRawResult, result32, Fq_N64);
}

/**
 * Montgomery multiplication by single limb using 32-bit arithmetic
 */
void Fq_rawMMul1(FqRawElement pRawResult, const FqRawElement pRawA, uint64_t pRawB) {
    // Convert inputs to 32-bit representation
    uint32_t rawA32[Fq_N32];
    uint32_t rawB32[2];
    convert_64_to_32(rawA32, pRawA, Fq_N64);
    convert_64_to_32(rawB32, &pRawB, 1);

    const size_t N = Fq_N32 + 2;  // Need extra limbs for calculations
    uint32_t np0;

    // Temporary products for each step of Montgomery reduction
    uint32_t product[N];
    memset(product, 0, N*sizeof(uint32_t));
    uint32_t temp[N];
    memset(temp, 0, N*sizeof(uint32_t));

    // Process first 32-bit limb of B
    uint32_t carry = mp_mul_1_32(product, rawA32, Fq_N32, rawB32[0]);
    product[Fq_N32] = carry;
    product[Fq_N32+1] = 0;

    // Montgomery reduction for first limb
    np0 = product[0] * Fq_np_32;
    carry = mp_mul_1_32(temp, Fq_rawq_32, N, np0);
    carry = mp_add_n_32(product, product, temp, N);

    // Shift right by 32 bits
    for (size_t j = 0; j < N-1; j++) {
        product[j] = product[j+1];
    }
    product[N-1] = 0;

    // Process second 32-bit limb of B (if non-zero)
    if (rawB32[1] != 0) {
        memset(temp, 0, N * sizeof(uint32_t));
        carry = mp_mul_1_32(temp, rawA32, Fq_N32, rawB32[1]);
        temp[Fq_N32] = carry;
        temp[Fq_N32+1] = 0;

        carry = mp_add_n_32(product, product, temp, N);

        // Montgomery reduction for second limb
        np0 = product[0] * Fq_np_32;
        memset(temp, 0, N * sizeof(uint32_t));
        carry = mp_mul_1_32(temp, Fq_rawq_32, N, np0);
        carry = mp_add_n_32(product, product, temp, N);

        // Shift right by 32 bits
        for (size_t j = 0; j < N-1; j++) {
            product[j] = product[j+1];
        }
        product[N-1] = 0;
    }

    // Perform remaining Montgomery reductions (total of 8 for Fq_N32)
    for (size_t i = 2; i < Fq_N32; i++) {
        np0 = product[0] * Fq_np_32;
        memset(temp, 0, N * sizeof(uint32_t));
        carry = mp_mul_1_32(temp, Fq_rawq_32, N, np0);
        carry = mp_add_n_32(product, product, temp, N);

        // Shift right by 32 bits
        for (size_t j = 0; j < N-1; j++) {
            product[j] = product[j+1];
        }
        product[N-1] = 0;
    }

    // Final conditional subtraction
    uint32_t result32[Fq_N32];
    mp_copy_32(result32, product, Fq_N32);

    if (mp_cmp_32(result32, Fq_rawq_32, Fq_N32) >= 0) {
        mp_sub_n_32(result32, result32, Fq_rawq_32, Fq_N32);
    }

    // Convert back to 64-bit representation
    convert_32_to_64(pRawResult, result32, Fq_N64);
}




void Fq_rawMSquare(FqRawElement pRawResult, const FqRawElement pRawA)
{
    Fq_rawMMul(pRawResult, pRawA, pRawA);
}

//void Fq_rawMMul1(FqRawElement pRawResult, const FqRawElement pRawA, uint64_t pRawB)
//{
//    const mp_size_t  N = Fq_N64+1;
//    const uint64_t  *mq = Fq_rawq_static;
//
//    uint64_t  np0;
//
//    uint64_t  product0[N];
//    uint64_t  product1[N];
//    uint64_t  product2[N];
//    uint64_t  product3[N];
//
//    memset(product0, 0, N*sizeof(uint64_t));
//    memset(product1, 0, N*sizeof(uint64_t));
//    memset(product2, 0, N*sizeof(uint64_t));
//    memset(product3, 0, N*sizeof(uint64_t));
//
//    product0[4] = mp_mul_1(product0, pRawA, Fq_N64, pRawB);
//
//    np0 = Fq_np_static * product0[0];
//    product1[1] = mp_addmul_1(product0, mq, N, np0);
//    mp_add(product1, product1, N, product0+1, N-1);
//
//    np0 = Fq_np_static * product1[0];
//    product2[1] = mp_addmul_1(product1, mq, N, np0);
//    mp_add(product2, product2, N, product1+1, N-1);
//
//    np0 = Fq_np_static * product2[0];
//    product3[1] = mp_addmul_1(product2, mq, N, np0);
//    mp_add(product3, product3, N, product2+1, N-1);
//
//    np0 = Fq_np_static * product3[0];
//    mp_addmul_1(product3, mq, N, np0);
//
//    mp_copy(pRawResult, product3+1, Fq_N64);
//
//    if (mp_cmp(pRawResult, mq, Fq_N64) >= 0)
//    {
//        mp_sub_n(pRawResult, pRawResult, mq, Fq_N64);
//    }
//}

void Fq_rawToMontgomery(FqRawElement pRawResult, const FqRawElement pRawA)
{
    Fq_rawMMul(pRawResult, pRawA, Fq_rawR2_static);
}

void Fq_rawFromMontgomery(FqRawElement pRawResult, const FqRawElement pRawA)
{
    const mp_size_t  N = Fq_N64+1;
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

    mpn_copyi(product0, pRawA, Fq_N64); 
    product0[4] = 0;

    np0 = Fq_np_static * product0[0];
    product1[1] = mpn_addmul_1(product0, mq, N, np0);
    mpn_add(product1, product1, N, product0+1, N-1);

    np0 = Fq_np_static * product1[0];
    product2[1] = mpn_addmul_1(product1, mq, N, np0);
    mpn_add(product2, product2, N, product1+1, N-1);

    np0 = Fq_np_static * product2[0];
    product3[1] = mpn_addmul_1(product2, mq, N, np0);
    mpn_add(product3, product3, N, product2+1, N-1);

    np0 = Fq_np_static * product3[0];
    mpn_addmul_1(product3, mq, N, np0);

    mpn_copyi(pRawResult, product3+1, Fq_N64);

    if (mpn_cmp(pRawResult, mq, Fq_N64) >= 0)
    {
        mpn_sub_n(pRawResult, pRawResult, mq, Fq_N64);
    }
}

int Fq_rawIsZero(const FqRawElement rawA)
{
    return mpn_zero_p(rawA, Fq_N64) ? 1 : 0;
}

