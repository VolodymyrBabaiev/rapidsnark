/**
 * test_prover_c.c - Unit tests for FQ field operations
 * Pure C implementation converted from C++
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "fq_c.h"  /* Assuming fq_c.h is the C header version of fq_c.hpp */
#include <limits.h>

int tests_run = 0;
int tests_failed = 0;

/* Helper functions for comparisons and output */

int is_equal_raw(const FqRawElement a, const FqRawElement b)
{
    return memcmp(a, b, sizeof(FqRawElement)) == 0;
}

int is_equal_element(const PFqElement a, const PFqElement b)
{
    return memcmp(a, b, sizeof(FqElement)) == 0;
}

/* Format a uint64_t value as a hex string */
void format_uint64(char* buffer, uint64_t val)
{
    sprintf(buffer, "0x%016llx", (unsigned long long)val);
}

/* Format a uint32_t value as a hex string */
void format_uint32(char* buffer, uint32_t val)
{
    sprintf(buffer, "0x%08x", val);
}

/* Format a int32_t value as a hex string */
void format_int32(char* buffer, int32_t val)
{
    sprintf(buffer, "0x%08x", val);
}

//FqElement fq_short(int32_t val)
//{
//    return {val, Fq_SHORT, {0, 0, 0, 0}};
//}

FqElement fq_short(int32_t val)
{
    FqElement result = {val, Fq_SHORT, {0, 0, 0, 0}};
    return result;
}

//FqElement fq_mshort(int32_t val)
//{
//    return {val, Fq_SHORTMONTGOMERY, {0, 0, 0, 0}};
//}

FqElement fq_long(uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3)
{
    FqElement result = {0, Fq_LONG, {val0, val1, val2, val3}};
    return result;
}

FqElement fq_mlong(uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3)
{
    FqElement result = {0, Fq_LONGMONTGOMERY, {val0, val1, val2, val3}};
    return result;
}

//FqElement fq_long(uint64_t val0, uint64_t val1 = 0, uint64_t val2 = 0, uint64_t val3 = 0)
//{
//    return {0, Fq_LONG, {val0, val1, val2, val3}};
//}
//
//FqElement fq_mlong(uint64_t val0, uint64_t val1 = 0, uint64_t val2 = 0, uint64_t val3 = 0)
//{
//    return {0, Fq_LONGMONTGOMERY, {val0, val1, val2, val3}};
//}

/* Print a raw field element to stdout */
void print_raw_element(const FqRawElement val)
{
    char buffer[20];
    
    format_uint64(buffer, val[0]);
    printf("%s,", buffer);
    format_uint64(buffer, val[1]);
    printf("%s,", buffer);
    format_uint64(buffer, val[2]);
    printf("%s,", buffer);
    format_uint64(buffer, val[3]);
    printf("%s", buffer);
}

/* Print a field element to stdout */
void print_fq_element(const PFqElement val)
{
    char buffer[20];
    
    format_int32(buffer, val->shortVal);
    printf("%s, ", buffer);
    format_uint32(buffer, val->type);
    printf("%s, ", buffer);
    print_raw_element(val->longVal);
}

/* Comparison function for raw element test results */
void compare_raw_result(const FqRawElement expected, 
                        const FqRawElement computed,
                        const FqRawElement A, 
                        const FqRawElement B, 
                        int idx, 
                        const char* test_name)
{
    if (!is_equal_raw(expected, computed))
    {
        printf("%s:%d failed!\n", test_name, idx);
        printf("A: ");
        print_raw_element(A);
        printf("\n");
        printf("B: ");
        print_raw_element(B);
        printf("\n");
        printf("Expected: ");
        print_raw_element(expected);
        printf("\n");
        printf("Computed: ");
        print_raw_element(computed);
        printf("\n\n");
        tests_failed++;
    }

    tests_run++;
}

/* Single operand comparison function for raw element test results */
void compare_raw_result_single(const FqRawElement expected, 
                              const FqRawElement computed,
                              const FqRawElement A,
                              int idx, 
                              const char* test_name)
{
    if (!is_equal_raw(expected, computed))
    {
        printf("%s:%d failed!\n", test_name, idx);
        printf("A: ");
        print_raw_element(A);
        printf("\n");
        printf("Expected: ");
        print_raw_element(expected);
        printf("\n");
        printf("Computed: ");
        print_raw_element(computed);
        printf("\n\n");
        tests_failed++;
    }

    tests_run++;
}

/* Compare and report test results */
void compare_Result(const PFqElement expected, const PFqElement computed,
                    const PFqElement A, const PFqElement B,
                    int idx, const char* test_name)
{
    if (!is_equal_raw(expected, computed))
    {
        printf("%s:%d failed!\n", test_name, idx);
        printf("A: "); print_fq_element(A); printf("\n");
        printf("B: "); print_fq_element(B); printf("\n");
        printf("Expected: "); print_fq_element(expected); printf("\n");
        printf("Computed: "); print_fq_element(computed); printf("\n\n");
        tests_failed++;
    }
    tests_run++;
}

void compare_Result_single(const PFqElement expected, const PFqElement computed,
                    const PFqElement A, int idx, const char* test_name)
{
    if (!is_equal_raw(expected, computed))
    {
        printf("%s:%d failed!\n", test_name, idx);
        printf("A: "); print_fq_element(A); printf("\n");
        printf("Expected: "); print_fq_element(expected); printf("\n");
        printf("Computed: "); print_fq_element(computed); printf("\n\n");
        tests_failed++;
    }
    tests_run++;
}

/* Test functions */

void Fq_Rw_mul_unit_test(void)
{
    /* Test case 0 */
    FqRawElement pRawA0 = {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014};
    FqRawElement pRawB0 = {0x1bb8e645ae216da7, 0x53fe3ab1e35c59e3, 0x8c49833d53bb8085, 0x216d0b17f4e44a5};
    FqRawElement pRawResult0 = {0x1187da3e296269a8, 0xd0139eb206e57eeb, 0xdb5973382f0e9301, 0x2e40d99a3c8089fb};
    
    /* Test case 1 */
    FqRawElement pRawA1 = {0x1, 0x0, 0x0, 0x0};
    FqRawElement pRawB1 = {0x2, 0x0, 0x0, 0x0};
    FqRawElement pRawResult1 = {0x9ee8847d2a18f727, 0x3ebeda789c801164, 0xe6778de8ed07cd56, 0x2c69dc6fd299ec49};
    
    /* Test case 2 */
    FqRawElement pRawA2 = {0xfffffffffffffffe, 0x0, 0x0, 0x0};
    FqRawElement pRawB2 = {0xffffffffffffffff, 0x0, 0x0, 0x0};
    FqRawElement pRawResult2 = {0x1b332e37e22aea3c, 0x6d7519cca22ac926, 0xa2b9e2fdbc1f2a77, 0x3058d8944ed69677};
    
    /* Test case 3 */
    FqRawElement pRawA3 = {0xfffffffffffffffe, 0xfffffffffffffffe, 0xfffffffffffffffe, 0xfffffffffffffffe};
    FqRawElement pRawB3 = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
    FqRawElement pRawResult3 = {0x1e51892c7f798de, 0x49c1eec88964fb31, 0xe7524f2299ec0ee2, 0x337a0489fce7555};
    
    /* Test case 4 */
    FqRawElement pRawA4 = {0xfffffffffffffffe, 0xfffffffffffffffe, 0xfffffffffffffffe, 0xfffffffffffffffe};
    FqRawElement pRawB4 = {0xffffffffffffffff, 0x0, 0x0, 0x0};
    FqRawElement pRawResult4 = {0xebb3da0ac591a7d2, 0xdc19acc8059254c6, 0xc31f14f32c65f257, 0x373ff2663c811ac};
    
    /* Test case 5 */
    FqRawElement pRawA5 = {0x0, 0x0, 0x0, 0x0};
    FqRawElement pRawB5 = {0x2, 0x0, 0x0, 0x0};
    FqRawElement pRawResult5 = {0x0, 0x0, 0x0, 0x0};
    
    /* Test case 8 */
    FqRawElement pRawA8 = {0x1, 0x0, 0x0, 0x0};
    FqRawElement pRawB8 = {0x3c208c16d87cfd47, 0x97816a916871ca8d, 0xb85045b68181585d, 0x30644e72e131a029};
    FqRawElement pRawResult8 = {0x0, 0x0, 0x0, 0x0};

    /* Test case 9 */
    FqRawElement pRawA9 = {0xd48ef8eb6f0a70a7, 0x83590aa4708b6780, 0x6603a7198a84f5b5, 0x27049057c6edb906};
    FqRawElement pRawB9 = {0xb1cd6dafda1530df,0x62f210e6a7283db6,0xef7f0b0c0ada0afb,0x20fd6e902d592544};
    FqRawElement pRawResult9 = {0xba658bb3c5668e7a, 0x8b6747b10d51d35a, 0x871359d9f90f6f90, 0xfd7c8811e0fe4b};

    /* Result variables for the actual computations */
    FqRawElement pRawResult0_c = {0};
    FqRawElement pRawResult1_c = {0};
    FqRawElement pRawResult2_c = {0};
    FqRawElement pRawResult3_c = {0};
    FqRawElement pRawResult4_c = {0};
    FqRawElement pRawResult5_c = {0};
    FqRawElement pRawResult8_c = {0};
    FqRawElement pRawResult9_c = {0};

    /* Perform the operations */
    Fq_rawMMul(pRawResult0_c, pRawA0, pRawB0);
    Fq_rawMMul(pRawResult1_c, pRawA1, pRawB1);
    Fq_rawMMul(pRawResult2_c, pRawA2, pRawB2);
    Fq_rawMMul(pRawResult3_c, pRawA3, pRawB3);
    Fq_rawMMul(pRawResult4_c, pRawA4, pRawB4);
    Fq_rawMMul(pRawResult5_c, pRawA5, pRawB5);
    Fq_rawMMul(pRawResult8_c, pRawA8, pRawB8);
    Fq_rawMMul(pRawResult9_c, pRawA9, pRawB9);

    /* Compare expected and actual results */
    compare_raw_result(pRawResult0, pRawResult0_c, pRawA0, pRawB0, 0, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult1, pRawResult1_c, pRawA1, pRawB1, 1, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult2, pRawResult2_c, pRawA2, pRawB2, 2, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult3, pRawResult3_c, pRawA3, pRawB3, 3, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult4, pRawResult4_c, pRawA4, pRawB4, 4, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult5, pRawResult5_c, pRawA5, pRawB5, 5, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult8, pRawResult8_c, pRawA8, pRawB8, 8, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult9, pRawResult9_c, pRawA9, pRawB9, 9, "Fq_Rw_mul_unit_test");
}

void Fq_Rw_mul1_unit_test(void)
{
    /* Test case 0 */
    FqRawElement pRawA0 = {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014};
    FqRawElement pRawB0 = {0x1bb8e645ae216da7, 0x53fe3ab1e35c59e3, 0x8c49833d53bb8085, 0x216d0b17f4e44a5};
    FqRawElement pRawResult0 = {0x8b363b7691ff055d, 0xb5ada052b1165e8f, 0x4b56ee9c6be00e25, 0x2cb43dbcbe503199};
    
    /* Test case 1 */
    FqRawElement pRawA1 = {0x1, 0x0, 0x0, 0x0};
    FqRawElement pRawB1 = {0x2, 0x0, 0x0, 0x0};
    FqRawElement pRawResult1 = {0x9ee8847d2a18f727, 0x3ebeda789c801164, 0xe6778de8ed07cd56, 0x2c69dc6fd299ec49};
    
    /* Test case 2 */
    FqRawElement pRawA2 = {0xfffffffffffffffe, 0x0, 0x0, 0x0};
    FqRawElement pRawB2 = {0xffffffffffffffff, 0x0, 0x0, 0x0};
    FqRawElement pRawResult2 = {0x1b332e37e22aea3c, 0x6d7519cca22ac926, 0xa2b9e2fdbc1f2a77, 0x3058d8944ed69677};
    
    /* Test case 3 */
    FqRawElement pRawA3 = {0xfffffffffffffffe, 0xfffffffffffffffe, 0xfffffffffffffffe, 0xfffffffffffffffe};
    FqRawElement pRawB3 = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
    FqRawElement pRawResult3 = {0x95b6aeefa3f8e52, 0x3bca00aff22ad49, 0x78ca497c3a602fb9, 0x217bf6416a170b5e};
    
    /* Test case 9 */
    FqRawElement pRawA9 = {0x3c208c16d87cfd47, 0x97816a916871ca8d, 0xb85045b68181585d, 0x30644e72e131a029};
    FqRawElement pRawB9 = {0x1, 0x0, 0x0, 0x0};
    FqRawElement pRawResult9 = {0x0, 0x0, 0x0, 0x0};

    //Fq_Rw_mul1_test 10:
    FqRawElement pRawA10= {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014};
    FqRawElement pRawB10= {0x1bb8,0x0,0x0,0x0};
    FqRawElement pRawResult10= {0xd48ef8eb6f0a70a7, 0x83590aa4708b6780, 0x6603a7198a84f5b5, 0x27049057c6edb906};


    /* Result variables for the actual computations */
    FqRawElement pRawResult0_c = {0};
    FqRawElement pRawResult1_c = {0};
    FqRawElement pRawResult2_c = {0};
    FqRawElement pRawResult3_c = {0};
    FqRawElement pRawResult9_c = {0};
    FqRawElement pRawResult10_c = {0};

    /* Perform the operations */
    Fq_rawMMul1(pRawResult0_c, pRawA0, pRawB0[0]);
    Fq_rawMMul1(pRawResult1_c, pRawA1, pRawB1[0]);
    Fq_rawMMul1(pRawResult2_c, pRawA2, pRawB2[0]);
    Fq_rawMMul1(pRawResult3_c, pRawA3, pRawB3[0]);
    Fq_rawMMul1(pRawResult9_c, pRawA9, pRawB9[0]);
    Fq_rawMMul1(pRawResult10_c, pRawA10, pRawB10[0]);

    /* Compare expected and actual results */
    compare_raw_result(pRawResult0, pRawResult0_c, pRawA0, pRawB0, 0, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult1, pRawResult1_c, pRawA1, pRawB1, 1, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult2, pRawResult2_c, pRawA2, pRawB2, 2, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult3, pRawResult3_c, pRawA3, pRawB3, 3, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult9, pRawResult9_c, pRawA9, pRawB9, 9, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult10, pRawResult10_c, pRawA10, pRawB10, 10, "Fq_Rw_mul1_unit_test");
}


// Substraction group unit tests

void Fq_add_s1s2_unit_test(void)
{
	FqElement pA_s1s20 = {0x1, 0x0, {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014}};
	FqElement pB_s1s20 = {0x2, 0x0, {0x1bb8e645ae216da7, 0x53fe3ab1e35c59e3, 0x8c49833d53bb8085, 0x216d0b17f4e44a5}};
	FqElement pResult_s1s20 = {0x3, 0x0, {0x0, 0x0, 0x0, 0x0}};

	FqElement pA_s1s21 = {0x0, 0x0, {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014}};
	FqElement pB_s1s21 = {0x2, 0x0, {0x1bb8e645ae216da7, 0x53fe3ab1e35c59e3, 0x8c49833d53bb8085, 0x216d0b17f4e44a5}};
	FqElement pResult_s1s21 = {0x2, 0x0, {0x0, 0x0, 0x0, 0x0}};

	FqElement pA_s1s22 = {0xa1f0, 0x0, {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014}};
	FqElement pB_s1s22 = {0x1bb8, 0x0, {0x1bb8e645ae216da7, 0x53fe3ab1e35c59e3, 0x8c49833d53bb8085, 0x216d0b17f4e44a5}};
	FqElement pResult_s1s22 = {0xbda8, 0x0, {0x0, 0x0, 0x0, 0x0}};

	FqElement pA_s1s23 = {0x7fffffff, 0x0, {0xa1f0fac9f8000000, 0x9419f4243cdcb848, 0xdc2822db40c0ac2e, 0x183227397098d014}};
	FqElement pB_s1s23 = {0x7fffffff, 0x0, {0x1bb8e645ae216da7, 0x53fe3ab1e35c59e3, 0x8c49833d53bb8085, 0x216d0b17f4e44a5}};
	FqElement pResult_s1s23 = {0x0, 0x80000000, {0xfffffffe, 0x0, 0x0, 0x0}};

	FqElement Result0_c = {0,0,{0,0,0,0}};
	FqElement Result1_c = {0,0,{0,0,0,0}};
	FqElement Result2_c = {0,0,{0,0,0,0}};
	FqElement Result3_c = {0,0,{0,0,0,0}};

	Fq_add(&Result0_c, &pA_s1s20, &pB_s1s20);
	Fq_add(&Result1_c, &pA_s1s21, &pB_s1s21);
	Fq_add(&Result2_c, &pA_s1s22, &pB_s1s22);
	Fq_add(&Result3_c, &pA_s1s23, &pB_s1s23);

	compare_Result(&pResult_s1s20, &Result0_c, &pA_s1s20, &pB_s1s20, 0, "Fq_add_s1s2_unit_test");
	compare_Result(&pResult_s1s21, &Result1_c, &pA_s1s21, &pB_s1s21, 1, "Fq_add_s1s2_unit_test");
	compare_Result(&pResult_s1s22, &Result2_c, &pA_s1s22, &pB_s1s22, 2, "Fq_add_s1s2_unit_test");
	compare_Result(&pResult_s1s23, &Result3_c, &pA_s1s23, &pB_s1s23, 3, "Fq_add_s1s2_unit_test");
}

void Fq_add_l1nl2n_unit_test()
{
    //Fq_add_l1nl2n_test 0:
    FqElement pA_l1nl2n0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1nl2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2n0= {0x0,0x80000000,{0x3,0x0,0x0,0x0}};
    //Fq_add_l1nl2n_test 1:
    FqElement pA_l1nl2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1nl2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2n1= {0x0,0x80000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_l1nl2n_test 2:
    FqElement pA_l1nl2n2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1nl2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1nl2n2= {0x0,0x80000000,{0xbda9e10fa6216da7,0xe8182ed62039122b,0x6871a618947c2cb3,0x1a48f7eaefe714ba}};
    //Fq_add_l1nl2n_test 3:
    FqElement pA_l1nl2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1nl2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1nl2n3= {0x0,0x80000000,{0xc3df73e9278302b7,0x687e956e978e3572,0x47afba497e7ea7a2,0xcf9bb18d1ece5fd6}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1nl2n0, &pB_l1nl2n0);
    Fq_add(&Result1_c, &pA_l1nl2n1, &pB_l1nl2n1);
    Fq_add(&Result2_c, &pA_l1nl2n2, &pB_l1nl2n2);
    Fq_add(&Result3_c, &pA_l1nl2n3, &pB_l1nl2n3);

    compare_Result(&pResult_l1nl2n0, &Result0_c,&pA_l1nl2n0, &pB_l1nl2n0, 0, "Fq_add_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n1, &Result1_c,&pA_l1nl2n1, &pB_l1nl2n1, 1, "Fq_add_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n2, &Result2_c,&pA_l1nl2n2, &pB_l1nl2n2, 2, "Fq_add_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n3, &Result3_c,&pA_l1nl2n3, &pB_l1nl2n3, 3, "Fq_add_l1nl2n_unit_test");
}

void Fq_add_l1ml2n_unit_test()
{
    //Fq_add_l1ml2n_test 0:
    FqElement pA_l1ml2n0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ml2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2n0= {0x0,0xc0000000,{0xa6ba871b8b1e1b3b,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_add_l1ml2n_test 1:
    FqElement pA_l1ml2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ml2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2n1= {0x0,0xc0000000,{0xa6ba871b8b1e1b3a,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_add_l1ml2n_test 2:
    FqElement pA_l1ml2n2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ml2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ml2n2= {0x0,0xc0000000,{0x87845142c515cf1e,0xa9a563c777305e58,0x02f8a1df90824147,0x20ea3ba8a906f1d3}};
    //Fq_add_l1ml2n_test 3:
    FqElement pA_l1ml2n3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ml2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ml2n3= {0x0,0xc0000000,{0x1fcfb8cd8dfbeceb,0xab6e2de8de7df6be,0xe13c7b8f91c839ca,0xf8ce27b030b055ef}};
    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1ml2n0, &pB_l1ml2n0);
    Fq_add(&Result1_c, &pA_l1ml2n1, &pB_l1ml2n1);
    Fq_add(&Result2_c, &pA_l1ml2n2, &pB_l1ml2n2);
    Fq_add(&Result3_c, &pA_l1ml2n3, &pB_l1ml2n3);

    compare_Result(&pResult_l1ml2n0, &Result0_c,&pA_l1ml2n0, &pB_l1ml2n0, 0, "Fq_add_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n1, &Result1_c,&pA_l1ml2n1, &pB_l1ml2n1, 1, "Fq_add_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n2, &Result2_c,&pA_l1ml2n2, &pB_l1ml2n2, 2, "Fq_add_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n3, &Result3_c,&pA_l1ml2n3, &pB_l1ml2n3, 3, "Fq_add_l1ml2n_unit_test");
}

void Fq_add_l1ml2m_unit_test()
{
    //Fq_add_l1ml2m_test 0:
    FqElement pA_l1ml2m0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ml2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2m0= {0x0,0xc0000000,{0x3,0x0,0x0,0x0}};
    //Fq_add_l1ml2m_test 1:
    FqElement pA_l1ml2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ml2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2m1= {0x0,0xc0000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_l1ml2m_test 2:
    FqElement pA_l1ml2m2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ml2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ml2m2= {0x0,0xc0000000,{0xbda9e10fa6216da7,0xe8182ed62039122b,0x6871a618947c2cb3,0x1a48f7eaefe714ba}};
    //Fq_add_l1ml2m_test 3:
    FqElement pA_l1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ml2m3= {0x0,0xc0000000,{0xc3df73e9278302b7,0x687e956e978e3572,0x47afba497e7ea7a2,0xcf9bb18d1ece5fd6}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1ml2m0, &pB_l1ml2m0);
    Fq_add(&Result1_c, &pA_l1ml2m1, &pB_l1ml2m1);
    Fq_add(&Result2_c, &pA_l1ml2m2, &pB_l1ml2m2);
    Fq_add(&Result3_c, &pA_l1ml2m3, &pB_l1ml2m3);

    compare_Result(&pResult_l1ml2m0, &Result0_c,&pA_l1ml2m0, &pB_l1ml2m0, 0, "Fq_add_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m1, &Result1_c,&pA_l1ml2m1, &pB_l1ml2m1, 1, "Fq_add_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m2, &Result2_c,&pA_l1ml2m2, &pB_l1ml2m2, 2, "Fq_add_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m3, &Result3_c,&pA_l1ml2m3, &pB_l1ml2m3, 3, "Fq_add_l1ml2m_unit_test");
}

void Fq_add_l1nl2m_unit_test()
{
    //Fq_add_l1nl2m_test 0:
    FqElement pA_l1nl2m0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1nl2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2m0= {0x0,0xc0000000,{0xd35d438dc58f0d9f,0x0a78eb28f5c70b3d,0x666ea36f7879462c,0x0e0a77c19a07df2f}};
    //Fq_add_l1nl2m_test 1:
    FqElement pA_l1nl2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1nl2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2m1= {0x0,0xc0000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_l1nl2m_test 2:
    FqElement pA_l1nl2m2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1nl2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1nl2m2= {0x0,0xc0000000,{0x863eddcec7a38339,0x9de6473ab08436f3,0xc4b96387269c60bb,0x0e0d02e01861062c}};
    //Fq_add_l1nl2m_test 3:
    FqElement pA_l1nl2m3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1nl2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1nl2m3= {0x0,0xc0000000,{0x1fcfb8cd8dfbeceb,0xab6e2de8de7df6be,0xe13c7b8f91c839ca,0xf8ce27b030b055ef}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1nl2m0, &pB_l1nl2m0);
    Fq_add(&Result1_c, &pA_l1nl2m1, &pB_l1nl2m1);
    Fq_add(&Result2_c, &pA_l1nl2m2, &pB_l1nl2m2);
    Fq_add(&Result3_c, &pA_l1nl2m3, &pB_l1nl2m3);

    compare_Result(&pResult_l1nl2m0, &Result0_c,&pA_l1nl2m0, &pB_l1nl2m0, 0, "Fq_add_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m1, &Result1_c,&pA_l1nl2m1, &pB_l1nl2m1, 1, "Fq_add_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m2, &Result2_c,&pA_l1nl2m2, &pB_l1nl2m2, 2, "Fq_add_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m3, &Result3_c,&pA_l1nl2m3, &pB_l1nl2m3, 3, "Fq_add_l1nl2m_unit_test");
}

void Fq_add_s1nl2m_unit_test()
{
    //Fq_add_s1nl2m_test 0:
    FqElement pA_s1nl2m0= {0x1,0x0,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1nl2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2m0= {0x0,0xc0000000,{0xd35d438dc58f0d9f,0x0a78eb28f5c70b3d,0x666ea36f7879462c,0x0e0a77c19a07df2f}};
    //Fq_add_s1nl2m_test 1:
    FqElement pA_s1nl2m1= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1nl2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2m1= {0x0,0xc0000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_s1nl2m_test 2:
    FqElement pA_s1nl2m2= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1nl2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1nl2m2= {0x0,0xc0000000,{0xe16e630a4b41d9f3,0xccb46bc339a001f1,0x4d406b95d15b2f0a,0x1861fa84426b2851}};
    //Fq_add_s1nl2m_test 3:
    FqElement pA_s1nl2m3= {-1,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1nl2m3= {0x7fffffff,0xc0000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_s1nl2m3= {0x0,0xc0000000,{0xaca2bc723a70f262,0x758714d70a38f4c1,0x19915c908786b9d3,0x71f5883e65f820d0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_s1nl2m0, &pB_s1nl2m0);
    Fq_add(&Result1_c, &pA_s1nl2m1, &pB_s1nl2m1);
    Fq_add(&Result2_c, &pA_s1nl2m2, &pB_s1nl2m2);
    Fq_add(&Result3_c, &pA_s1nl2m3, &pB_s1nl2m3);

    compare_Result(&pResult_s1nl2m0, &Result0_c,&pA_s1nl2m0, &pB_s1nl2m0, 0, "Fq_add_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m1, &Result1_c,&pA_s1nl2m1, &pB_s1nl2m1, 1, "Fq_add_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m2, &Result2_c,&pA_s1nl2m2, &pB_s1nl2m2, 2, "Fq_add_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m3, &Result3_c,&pA_s1nl2m3, &pB_s1nl2m3, 3, "Fq_add_s1nl2m_unit_test");
}

void Fq_add_l1ms2n_unit_test()
{
    //Fq_add_l1ms2n_test 0:
    FqElement pA_l1ms2n0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ms2n0= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2n0= {0x0,0xc0000000,{0xa6ba871b8b1e1b3b,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_add_l1ms2n_test 1:
    FqElement pA_l1ms2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ms2n1= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2n1= {0x0,0xc0000000,{0xa6ba871b8b1e1b3a,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_add_l1ms2n_test 2:
    FqElement pA_l1ms2n2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ms2n2= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ms2n2= {0x0,0xc0000000,{0xc952c0c6d7f83dcb,0x4dec2bfdd6e43410,0xb949a9cb0700082f,0x1396afa5c0a80a66}};
    //Fq_add_l1ms2n_test 3:
    FqElement pA_l1ms2n3= {0xffff,0xc0000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pB_l1ms2n3= {-1,0x0,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_l1ms2n3= {0x0,0xc0000000,{0xaca2bc723a70f262,0x758714d70a38f4c1,0x19915c908786b9d3,0x71f5883e65f820d0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1ms2n0, &pB_l1ms2n0);
    Fq_add(&Result1_c, &pA_l1ms2n1, &pB_l1ms2n1);
    Fq_add(&Result2_c, &pA_l1ms2n2, &pB_l1ms2n2);
    Fq_add(&Result3_c, &pA_l1ms2n3, &pB_l1ms2n3);

    compare_Result(&pResult_l1ms2n0, &Result0_c,&pA_l1ms2n0, &pB_l1ms2n0, 0, "Fq_add_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n1, &Result1_c,&pA_l1ms2n1, &pB_l1ms2n1, 1, "Fq_add_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n2, &Result2_c,&pA_l1ms2n2, &pB_l1ms2n2, 2, "Fq_add_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n3, &Result3_c,&pA_l1ms2n3, &pB_l1ms2n3, 3, "Fq_add_l1ms2n_unit_test");
}

void Fq_add_l1ms2m_unit_test()
{
    //Fq_add_l1ms2m_test 0:
    FqElement pA_l1ms2m0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ms2m0= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2m0= {0x0,0xc0000000,{0x3,0x0,0x0,0x0}};
    //Fq_add_l1ms2m_test 1:
    FqElement pA_l1ms2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ms2m1= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2m1= {0x0,0xc0000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_l1ms2m_test 2:
    FqElement pA_l1ms2m2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ms2m2= {0x1bb8,0x40000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ms2m2= {0x0,0xc0000000,{0xbda9e10fa6216da7,0xe8182ed62039122b,0x6871a618947c2cb3,0x1a48f7eaefe714ba}};
    //Fq_add_l1ms2m_test 3:
    FqElement pA_l1ms2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ms2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ms2m3= {0x0,0xc0000000,{0xc3df73e9278302b7,0x687e956e978e3572,0x47afba497e7ea7a2,0xcf9bb18d1ece5fd6}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1ms2m0, &pB_l1ms2m0);
    Fq_add(&Result1_c, &pA_l1ms2m1, &pB_l1ms2m1);
    Fq_add(&Result2_c, &pA_l1ms2m2, &pB_l1ms2m2);
    Fq_add(&Result3_c, &pA_l1ms2m3, &pB_l1ms2m3);

    compare_Result(&pResult_l1ms2m0, &Result0_c,&pA_l1ms2m0, &pB_l1ms2m0, 0, "Fq_add_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m1, &Result1_c,&pA_l1ms2m1, &pB_l1ms2m1, 1, "Fq_add_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m2, &Result2_c,&pA_l1ms2m2, &pB_l1ms2m2, 2, "Fq_add_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m3, &Result3_c,&pA_l1ms2m3, &pB_l1ms2m3, 3, "Fq_add_l1ms2m_unit_test");
}

void Fq_add_s1ml2m_unit_test()
{
    //Fq_add_s1ml2m_test 0:
    FqElement pA_s1ml2m0= {0x1,0x40000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1ml2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2m0= {0x0,0xc0000000,{0x3,0x0,0x0,0x0}};
    //Fq_add_s1ml2m_test 1:
    FqElement pA_s1ml2m1= {0x0,0x40000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1ml2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2m1= {0x0,0xc0000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_s1ml2m_test 2:
    FqElement pA_s1ml2m2= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1ml2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1ml2m2= {0x0,0xc0000000,{0xbda9e10fa6216da7,0xe8182ed62039122b,0x6871a618947c2cb3,0x1a48f7eaefe714ba}};
    //Fq_add_s1ml2m_test 3:
    FqElement pA_s1ml2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1ml2m3= {0x0,0xc0000000,{0xc3df73e9278302b7,0x687e956e978e3572,0x47afba497e7ea7a2,0xcf9bb18d1ece5fd6}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_s1ml2m0, &pB_s1ml2m0);
    Fq_add(&Result1_c, &pA_s1ml2m1, &pB_s1ml2m1);
    Fq_add(&Result2_c, &pA_s1ml2m2, &pB_s1ml2m2);
    Fq_add(&Result3_c, &pA_s1ml2m3, &pB_s1ml2m3);

    compare_Result(&pResult_s1ml2m0, &Result0_c,&pA_s1ml2m0, &pB_s1ml2m0, 0, "Fq_add_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m1, &Result1_c,&pA_s1ml2m1, &pB_s1ml2m1, 1, "Fq_add_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m2, &Result2_c,&pA_s1ml2m2, &pB_s1ml2m2, 2, "Fq_add_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m3, &Result3_c,&pA_s1ml2m3, &pB_s1ml2m3, 3, "Fq_add_s1ml2m_unit_test");
}

void Fq_add_l1ns2_unit_test()
{
    //Fq_add_l1ns2_test 0:
    FqElement pA_l1ns20= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ns20= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns20= {0x0,0x80000000,{0x3,0x0,0x0,0x0}};
    //Fq_add_l1ns2_test 1:
    FqElement pA_l1ns21= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ns21= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns21= {0x0,0x80000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_l1ns2_test 2:
    FqElement pA_l1ns22= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ns22= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ns22= {0x0,0x80000000,{0xa1f0fac9f8001bb8,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_add_l1ns2_test 3:
    FqElement pA_l1ns23= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ns23= {0xffff,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ns23= {0x0,0x80000000,{0xc3df73e9278402b7,0x687e956e978e3572,0x47afba497e7ea7a2,0xcf9bb18d1ece5fd6}};
    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_l1ns20, &pB_l1ns20);
    Fq_add(&Result1_c, &pA_l1ns21, &pB_l1ns21);
    Fq_add(&Result2_c, &pA_l1ns22, &pB_l1ns22);
    Fq_add(&Result3_c, &pA_l1ns23, &pB_l1ns23);

    compare_Result(&pResult_l1ns20, &Result0_c,&pA_l1ns20, &pB_l1ns20, 0, "Fq_add_l1ns2_unit_test");
    compare_Result(&pResult_l1ns21, &Result1_c,&pA_l1ns21, &pB_l1ns21, 1, "Fq_add_l1ns2_unit_test");
    compare_Result(&pResult_l1ns22, &Result2_c,&pA_l1ns22, &pB_l1ns22, 2, "Fq_add_l1ns2_unit_test");
    compare_Result(&pResult_l1ns23, &Result3_c,&pA_l1ns23, &pB_l1ns23, 3, "Fq_add_l1ns2_unit_test");
}

void Fq_add_s1l2n_unit_test()
{
    //Fq_add_s1l2n_test 0:
    FqElement pA_s1l2n0= {0x1,0x0,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1l2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1l2n0= {0x0,0x80000000,{0x3,0x0,0x0,0x0}};
    //Fq_add_s1l2n_test 1:
    FqElement pA_s1l2n1= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1l2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1l2n1= {0x0,0x80000000,{0x2,0x0,0x0,0x0}};
    //Fq_add_s1l2n_test 2:
    FqElement pA_s1l2n2= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1l2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1l2n2= {0x0,0x80000000,{0x1bb8e645ae220f97,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    //Fq_add_s1l2n_test 3:
    FqElement pA_s1l2n3= {0xffff,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1l2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1l2n3= {0x0,0x80000000,{0xc3df73e9278402b7,0x687e956e978e3572,0x47afba497e7ea7a2,0xcf9bb18d1ece5fd6}};
    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_add(&Result0_c, &pA_s1l2n0, &pB_s1l2n0);
    Fq_add(&Result1_c, &pA_s1l2n1, &pB_s1l2n1);
    Fq_add(&Result2_c, &pA_s1l2n2, &pB_s1l2n2);
    Fq_add(&Result3_c, &pA_s1l2n3, &pB_s1l2n3);

    compare_Result(&pResult_s1l2n0, &Result0_c,&pA_s1l2n0, &pB_s1l2n0, 0, "Fq_add_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n1, &Result1_c,&pA_s1l2n1, &pB_s1l2n1, 1, "Fq_add_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n2, &Result2_c,&pA_s1l2n2, &pB_s1l2n2, 2, "Fq_add_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n3, &Result3_c,&pA_s1l2n3, &pB_s1l2n3, 3, "Fq_add_s1l2n_unit_test");
}

void Fq_sub_s1s2_unit_test()
{
    //Fq_sub_s1s2_test 0:
    FqElement pA_s1s20= {0x1,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s20= {0x2,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s20= {-1,0x0,{0x0,0x0,0x0,0x0}};
    //Fq_sub_s1s2_test 1:
    FqElement pA_s1s21= {0x0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s21= {0x2,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s21= {-2,0x0,{0x0,0x0,0x0,0x0}};
    //Fq_sub_s1s2_test 2:
    FqElement pA_s1s22= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s22= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s22= {0x8638,0x0,{0x0,0x0,0x0,0x0}};
    //Fq_sub_s1s2_test 3:
    FqElement pA_s1s23= {0x7fffffff,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s23= {0x7fffffff,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s23= {0x0,0x0,{0x0,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_s1s20, &pB_s1s20);
    Fq_sub(&Result1_c, &pA_s1s21, &pB_s1s21);
    Fq_sub(&Result2_c, &pA_s1s22, &pB_s1s22);
    Fq_sub(&Result3_c, &pA_s1s23, &pB_s1s23);

    compare_Result(&pResult_s1s20, &Result0_c,&pA_s1s20, &pB_s1s20, 0, "Fq_sub_s1s2_unit_test");
    compare_Result(&pResult_s1s21, &Result1_c,&pA_s1s21, &pB_s1s21, 1, "Fq_sub_s1s2_unit_test");
    compare_Result(&pResult_s1s22, &Result2_c,&pA_s1s22, &pB_s1s22, 2, "Fq_sub_s1s2_unit_test");
    compare_Result(&pResult_s1s23, &Result3_c,&pA_s1s23, &pB_s1s23, 3, "Fq_sub_s1s2_unit_test");
}

void Fq_sub_l1nl2n_unit_test()
{
    //Fq_sub_l1nl2n_test 0:
    FqElement pA_l1nl2n0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1nl2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2n0= {0x0,0x80000000,{0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1nl2n_test 1:
    FqElement pA_l1nl2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1nl2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2n1= {0x0,0x80000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1nl2n_test 2:
    FqElement pA_l1nl2n2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1nl2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1nl2n2= {0x0,0x80000000,{0x8638148449de9259,0x401bb97259805e65,0x4fde9f9ded052ba9,0x161b5687f14a8b6f}};
    //Fq_sub_l1nl2n_test 3:
    FqElement pA_l1nl2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1nl2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1nl2n3= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1nl2n0, &pB_l1nl2n0);
    Fq_sub(&Result1_c, &pA_l1nl2n1, &pB_l1nl2n1);
    Fq_sub(&Result2_c, &pA_l1nl2n2, &pB_l1nl2n2);
    Fq_sub(&Result3_c, &pA_l1nl2n3, &pB_l1nl2n3);

    compare_Result(&pResult_l1nl2n0, &Result0_c,&pA_l1nl2n0, &pB_l1nl2n0, 0, "Fq_sub_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n1, &Result1_c,&pA_l1nl2n1, &pB_l1nl2n1, 1, "Fq_sub_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n2, &Result2_c,&pA_l1nl2n2, &pB_l1nl2n2, 2, "Fq_sub_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n3, &Result3_c,&pA_l1nl2n3, &pB_l1nl2n3, 3, "Fq_sub_l1nl2n_unit_test");
}

void Fq_sub_l1ml2n_unit_test()
{
    //Fq_sub_l1ml2n_test 0:
    FqElement pA_l1ml2n0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ml2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2n0= {0x0,0xc0000000,{0x956604fb4d5ee20e,0x828f943f7ce3b411,0xeb72fed7908ecc05,0x144f5eefad21e1ca}};
    //Fq_sub_l1ml2n_test 1:
    FqElement pA_l1ml2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ml2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2n1= {0x0,0xc0000000,{0x956604fb4d5ee20d,0x828f943f7ce3b411,0xeb72fed7908ecc05,0x144f5eefad21e1ca}};
    //Fq_sub_l1ml2n_test 2:
    FqElement pA_l1ml2n2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ml2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ml2n2= {0x0,0xc0000000,{0xbc5da4512aea30e2,0x7e8e848102891238,0xb557a3d6f0ff1715,0x0f7a12ca382aae56}};
    //Fq_sub_l1ml2n_test 3:
    FqElement pA_l1ml2n3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ml2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ml2n3= {0x0,0xc0000000,{0xa40fbb1b998715cc,0xbd106785b9103eb4,0x66733eb9ecb66dd7,0xd6cd89dcee1e09e6}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1ml2n0, &pB_l1ml2n0);
    Fq_sub(&Result1_c, &pA_l1ml2n1, &pB_l1ml2n1);
    Fq_sub(&Result2_c, &pA_l1ml2n2, &pB_l1ml2n2);
    Fq_sub(&Result3_c, &pA_l1ml2n3, &pB_l1ml2n3);

    compare_Result(&pResult_l1ml2n0, &Result0_c,&pA_l1ml2n0, &pB_l1ml2n0, 0, "Fq_sub_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n1, &Result1_c,&pA_l1ml2n1, &pB_l1ml2n1, 1, "Fq_sub_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n2, &Result2_c,&pA_l1ml2n2, &pB_l1ml2n2, 2, "Fq_sub_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n3, &Result3_c,&pA_l1ml2n3, &pB_l1ml2n3, 3, "Fq_sub_l1ml2n_unit_test");
}

void Fq_sub_l1ml2m_unit_test()
{
    //Fq_sub_l1ml2m_test 0:
    FqElement pA_l1ml2m0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ml2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2m0= {0x0,0xc0000000,{0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1ml2m_test 1:
    FqElement pA_l1ml2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ml2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2m1= {0x0,0xc0000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1ml2m_test 2:
    FqElement pA_l1ml2m2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ml2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ml2m2= {0x0,0xc0000000,{0x8638148449de9259,0x401bb97259805e65,0x4fde9f9ded052ba9,0x161b5687f14a8b6f}};
    //Fq_sub_l1ml2m_test 3:
    FqElement pA_l1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ml2m3= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1ml2m0, &pB_l1ml2m0);
    Fq_sub(&Result1_c, &pA_l1ml2m1, &pB_l1ml2m1);
    Fq_sub(&Result2_c, &pA_l1ml2m2, &pB_l1ml2m2);
    Fq_sub(&Result3_c, &pA_l1ml2m3, &pB_l1ml2m3);

    compare_Result(&pResult_l1ml2m0, &Result0_c,&pA_l1ml2m0, &pB_l1ml2m0, 0, "Fq_sub_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m1, &Result1_c,&pA_l1ml2m1, &pB_l1ml2m1, 1, "Fq_sub_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m2, &Result2_c,&pA_l1ml2m2, &pB_l1ml2m2, 2, "Fq_sub_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m3, &Result3_c,&pA_l1ml2m3, &pB_l1ml2m3, 3, "Fq_sub_l1ml2m_unit_test");
}

void Fq_sub_l1nl2m_unit_test()
{
    //Fq_sub_l1nl2m_test 0:
    FqElement pA_l1nl2m0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1nl2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2m0= {0x0,0xc0000000,{0xd35d438dc58f0d9b,0x0a78eb28f5c70b3d,0x666ea36f7879462c,0x0e0a77c19a07df2f}};
    //Fq_sub_l1nl2m_test 1:
    FqElement pA_l1nl2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1nl2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2m1= {0x0,0xc0000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1nl2m_test 2:
    FqElement pA_l1nl2m2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1nl2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1nl2m2= {0x0,0xc0000000,{0x4ecd11436b60a7eb,0xf5e9d1d6e9cb832d,0xac265d0c7f255fb0,0x09df617d19c47ce1}};
    //Fq_sub_l1nl2m_test 3:
    FqElement pA_l1nl2m3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1nl2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1nl2m3= {0x0,0xc0000000,{0x9810d0fb3ef5e77b,0xda71030baf618bd8,0x51dd06fc94caea85,0x5996c495f3139643}};
    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1nl2m0, &pB_l1nl2m0);
    Fq_sub(&Result1_c, &pA_l1nl2m1, &pB_l1nl2m1);
    Fq_sub(&Result2_c, &pA_l1nl2m2, &pB_l1nl2m2);
    Fq_sub(&Result3_c, &pA_l1nl2m3, &pB_l1nl2m3);

    compare_Result(&pResult_l1nl2m0, &Result0_c,&pA_l1nl2m0, &pB_l1nl2m0, 0, "Fq_sub_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m1, &Result1_c,&pA_l1nl2m1, &pB_l1nl2m1, 1, "Fq_sub_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m2, &Result2_c,&pA_l1nl2m2, &pB_l1nl2m2, 2, "Fq_sub_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m3, &Result3_c,&pA_l1nl2m3, &pB_l1nl2m3, 3, "Fq_sub_l1nl2m_unit_test");
}

void Fq_sub_s1nl2m_unit_test()
{
    //Fq_sub_s1nl2m_test 0:
    FqElement pA_s1nl2m0= {0x1,0x0,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1nl2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2m0= {0x0,0xc0000000,{0xd35d438dc58f0d9b,0x0a78eb28f5c70b3d,0x666ea36f7879462c,0x0e0a77c19a07df2f}};
    //Fq_sub_s1nl2m_test 1:
    FqElement pA_s1nl2m1= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1nl2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2m1= {0x0,0xc0000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_s1nl2m_test 2:
    FqElement pA_s1nl2m2= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1nl2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1nl2m2= {0x0,0xc0000000,{0xa9fc967eeefefea5,0x24b7f65f72e74e2b,0x34ad651b29e42e00,0x1434592143ce9f06}};
    //Fq_sub_s1nl2m_test 3:
    FqElement pA_s1nl2m3= {-1,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1nl2m3= {0x7fffffff,0xc0000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_s1nl2m3= {0x0,0xc0000000,{0x24e3d49feb6aecf2,0xa489e9f9db1c89dd,0x8a31e7fd8a896a8f,0xd2be2524285b6124}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_s1nl2m0, &pB_s1nl2m0);
    Fq_sub(&Result1_c, &pA_s1nl2m1, &pB_s1nl2m1);
    Fq_sub(&Result2_c, &pA_s1nl2m2, &pB_s1nl2m2);
    Fq_sub(&Result3_c, &pA_s1nl2m3, &pB_s1nl2m3);

    compare_Result(&pResult_s1nl2m0, &Result0_c,&pA_s1nl2m0, &pB_s1nl2m0, 0, "Fq_sub_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m1, &Result1_c,&pA_s1nl2m1, &pB_s1nl2m1, 1, "Fq_sub_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m2, &Result2_c,&pA_s1nl2m2, &pB_s1nl2m2, 2, "Fq_sub_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m3, &Result3_c,&pA_s1nl2m3, &pB_s1nl2m3, 3, "Fq_sub_s1nl2m_unit_test");
}

void Fq_sub_l1ms2n_unit_test()
{
    //Fq_sub_l1ms2n_test 0:
    FqElement pA_l1ms2n0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ms2n0= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2n0= {0x0,0xc0000000,{0x956604fb4d5ee20e,0x828f943f7ce3b411,0xeb72fed7908ecc05,0x144f5eefad21e1ca}};
    //Fq_sub_l1ms2n_test 1:
    FqElement pA_l1ms2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ms2n1= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2n1= {0x0,0xc0000000,{0x956604fb4d5ee20d,0x828f943f7ce3b411,0xeb72fed7908ecc05,0x144f5eefad21e1ca}};
    //Fq_sub_l1ms2n_test 2:
    FqElement pA_l1ms2n2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ms2n2= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ms2n2= {0x0,0xc0000000,{0x7a8f34cd1807c235,0xda47bc4aa2d53c80,0xff069beb7a81502d,0x1ccd9ecd208995c2}};
    //Fq_sub_l1ms2n_test 3:
    FqElement pA_l1ms2n3= {0xffff,0xc0000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pB_l1ms2n3= {-1,0x0,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_l1ms2n3= {0x0,0xc0000000,{0x173cb776ed121055,0xf2f780978d5540b0,0x2e1e5db8f6f7edcd,0x5da6294eb8d63f05}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1ms2n0, &pB_l1ms2n0);
    Fq_sub(&Result1_c, &pA_l1ms2n1, &pB_l1ms2n1);
    Fq_sub(&Result2_c, &pA_l1ms2n2, &pB_l1ms2n2);
    Fq_sub(&Result3_c, &pA_l1ms2n3, &pB_l1ms2n3);

    compare_Result(&pResult_l1ms2n0, &Result0_c,&pA_l1ms2n0, &pB_l1ms2n0, 0, "Fq_sub_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n1, &Result1_c,&pA_l1ms2n1, &pB_l1ms2n1, 1, "Fq_sub_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n2, &Result2_c,&pA_l1ms2n2, &pB_l1ms2n2, 2, "Fq_sub_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n3, &Result3_c,&pA_l1ms2n3, &pB_l1ms2n3, 3, "Fq_sub_l1ms2n_unit_test");
}

void Fq_sub_l1ms2m_unit_test()
{
    //Fq_sub_l1ms2m_test 0:
    FqElement pA_l1ms2m0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ms2m0= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2m0= {0x0,0xc0000000,{0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1ms2m_test 1:
    FqElement pA_l1ms2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ms2m1= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2m1= {0x0,0xc0000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1ms2m_test 2:
    FqElement pA_l1ms2m2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ms2m2= {0x1bb8,0x40000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ms2m2= {0x0,0xc0000000,{0x8638148449de9259,0x401bb97259805e65,0x4fde9f9ded052ba9,0x161b5687f14a8b6f}};
    //Fq_sub_l1ms2m_test 3:
    FqElement pA_l1ms2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ms2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ms2m3= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1ms2m0, &pB_l1ms2m0);
    Fq_sub(&Result1_c, &pA_l1ms2m1, &pB_l1ms2m1);
    Fq_sub(&Result2_c, &pA_l1ms2m2, &pB_l1ms2m2);
    Fq_sub(&Result3_c, &pA_l1ms2m3, &pB_l1ms2m3);

    compare_Result(&pResult_l1ms2m0, &Result0_c,&pA_l1ms2m0, &pB_l1ms2m0, 0, "Fq_sub_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m1, &Result1_c,&pA_l1ms2m1, &pB_l1ms2m1, 1, "Fq_sub_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m2, &Result2_c,&pA_l1ms2m2, &pB_l1ms2m2, 2, "Fq_sub_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m3, &Result3_c,&pA_l1ms2m3, &pB_l1ms2m3, 3, "Fq_sub_l1ms2m_unit_test");
}

void Fq_sub_s1ml2m_unit_test()
{
    //Fq_sub_s1ml2m_test 0:
    FqElement pA_s1ml2m0= {0x1,0x40000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1ml2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2m0= {0x0,0xc0000000,{0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_s1ml2m_test 1:
    FqElement pA_s1ml2m1= {0x0,0x40000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1ml2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2m1= {0x0,0xc0000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_s1ml2m_test 2:
    FqElement pA_s1ml2m2= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1ml2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1ml2m2= {0x0,0xc0000000,{0x8638148449de9259,0x401bb97259805e65,0x4fde9f9ded052ba9,0x161b5687f14a8b6f}};
    //Fq_sub_s1ml2m_test 3:
    FqElement pA_s1ml2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1ml2m3= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_s1ml2m0, &pB_s1ml2m0);
    Fq_sub(&Result1_c, &pA_s1ml2m1, &pB_s1ml2m1);
    Fq_sub(&Result2_c, &pA_s1ml2m2, &pB_s1ml2m2);
    Fq_sub(&Result3_c, &pA_s1ml2m3, &pB_s1ml2m3);

    compare_Result(&pResult_s1ml2m0, &Result0_c,&pA_s1ml2m0, &pB_s1ml2m0, 0, "Fq_sub_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m1, &Result1_c,&pA_s1ml2m1, &pB_s1ml2m1, 1, "Fq_sub_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m2, &Result2_c,&pA_s1ml2m2, &pB_s1ml2m2, 2, "Fq_sub_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m3, &Result3_c,&pA_s1ml2m3, &pB_s1ml2m3, 3, "Fq_sub_s1ml2m_unit_test");
}

void Fq_sub_l1ns2_unit_test()
{
    //Fq_sub_l1ns2_test 0:
    FqElement pA_l1ns20= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ns20= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns20= {0x0,0x80000000,{0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1ns2_test 1:
    FqElement pA_l1ns21= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ns21= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns21= {0x0,0x80000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_l1ns2_test 2:
    FqElement pA_l1ns22= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ns22= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ns22= {0x0,0x80000000,{0xa1f0fac9f7ffe448,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_sub_l1ns2_test 3:
    FqElement pA_l1ns23= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ns23= {0xffff,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ns23= {0x0,0x80000000,{0xffffffffffff0000,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_l1ns20, &pB_l1ns20);
    Fq_sub(&Result1_c, &pA_l1ns21, &pB_l1ns21);
    Fq_sub(&Result2_c, &pA_l1ns22, &pB_l1ns22);
    Fq_sub(&Result3_c, &pA_l1ns23, &pB_l1ns23);

    compare_Result(&pResult_l1ns20, &Result0_c,&pA_l1ns20, &pB_l1ns20, 0, "Fq_sub_l1ns2_unit_test");
    compare_Result(&pResult_l1ns21, &Result1_c, &pA_l1ns21, &pB_l1ns21, 1, "Fq_sub_l1ns2_unit_test");
    compare_Result(&pResult_l1ns22, &Result2_c, &pA_l1ns22, &pB_l1ns22, 2, "Fq_sub_l1ns2_unit_test");
    compare_Result(&pResult_l1ns23, &Result3_c,&pA_l1ns23, &pB_l1ns23, 3, "Fq_sub_l1ns2_unit_test");
}

void Fq_sub_s1l2n_unit_test()
{
    //Fq_sub_s1l2n_test 0:
    FqElement pA_s1l2n0= {0x1,0x0,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1l2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1l2n0= {0x0,0x80000000,{0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_s1l2n_test 1:
    FqElement pA_s1l2n1= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1l2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1l2n1= {0x0,0x80000000,{0x3c208c16d87cfd45,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_s1l2n_test 2:
    FqElement pA_s1l2n2= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1l2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1l2n2= {0x0,0x80000000,{0x2067a5d12a5c3190,0x43832fdf851570aa,0x2c06c2792dc5d7d8,0x2e4d7dc161e35b84}};
    //Fq_sub_s1l2n_test 3:
    FqElement pA_s1l2n3= {0xffff,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1l2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1l2n3= {0x0,0x80000000,{0x3c208c16d87dfd47,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
    //Fq_sub_s1l2n_test 4:
    FqElement pA_s1l2n4= {-1,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1l2n4= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1l2n4= {0x0,0x80000000,{0x7841182db0f9fa8e,0x2f02d522d0e3951a,0x70a08b6d0302b0bb,0x60c89ce5c2634053}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};
    FqElement Result4_c= {0,0,{0,0,0,0}};

    Fq_sub(&Result0_c, &pA_s1l2n0, &pB_s1l2n0);
    Fq_sub(&Result1_c, &pA_s1l2n1, &pB_s1l2n1);
    Fq_sub(&Result2_c, &pA_s1l2n2, &pB_s1l2n2);
    Fq_sub(&Result3_c, &pA_s1l2n3, &pB_s1l2n3);
    Fq_sub(&Result4_c, &pA_s1l2n4, &pB_s1l2n4);

    compare_Result(&pResult_s1l2n0, &Result0_c,&pA_s1l2n0, &pB_s1l2n0, 0, "Fq_sub_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n1, &Result1_c,&pA_s1l2n1, &pB_s1l2n1, 1, "Fq_sub_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n2, &Result2_c,&pA_s1l2n2, &pB_s1l2n2, 2, "Fq_sub_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n3, &Result3_c,&pA_s1l2n3, &pB_s1l2n3, 3, "Fq_sub_s1l2n_unit_test");
    compare_Result(&pResult_s1l2n4, &Result4_c,&pA_s1l2n4, &pB_s1l2n4, 4, "Fq_sub_s1l2n_unit_test");
}

void Fq_mul_s1s2_unit_test()
{
    //Fq_mul_s1s2_test 0:
    FqElement pA_s1s20= {0x1,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s20= {0x2,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s20= {0x0,0x80000000,{0x2,0x0,0x0,0x0}};
    //Fq_mul_s1s2_test 1:
    FqElement pA_s1s21= {0x0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s21= {0x2,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s21= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_s1s2_test 2:
    FqElement pA_s1s22= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s22= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s22= {0x0,0x80000000,{0x1188b480,0x0,0x0,0x0}};
    //Fq_mul_s1s2_test 3:
    FqElement pA_s1s23= {0x7fffffff,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1s23= {0x7fffffff,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1s23= {0x0,0x80000000,{0x3fffffff00000001,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_s1s20, &pB_s1s20);
    Fq_mul(&Result1_c, &pA_s1s21, &pB_s1s21);
    Fq_mul(&Result2_c, &pA_s1s22, &pB_s1s22);
    Fq_mul(&Result3_c, &pA_s1s23, &pB_s1s23);

    compare_Result(&pResult_s1s20, &Result0_c,&pA_s1s20, &pB_s1s20, 0, "Fq_mul_s1s2_unit_test");
    compare_Result(&pResult_s1s21, &Result1_c,&pA_s1s21, &pB_s1s21, 1, "Fq_mul_s1s2_unit_test");
    compare_Result(&pResult_s1s22, &Result2_c,&pA_s1s22, &pB_s1s22, 2, "Fq_mul_s1s2_unit_test");
    compare_Result(&pResult_s1s23, &Result3_c,&pA_s1s23, &pB_s1s23, 3, "Fq_mul_s1s2_unit_test");
}

void Fq_mul_l1nl2n_unit_test()
{
    //Fq_mul_l1nl2n_test 0:
    FqElement pA_l1nl2n0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1nl2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2n0= {0x0,0xc0000000,{0xa6ba871b8b1e1b3a,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_mul_l1nl2n_test 1:
    FqElement pA_l1nl2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1nl2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1nl2n_test 2:
    FqElement pA_l1nl2n2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1nl2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1nl2n2= {0x0,0xc0000000,{0xcf8964868a91901b,0x7a6a901fa0148d8,0x4db71dbbc02a5dd1,0x16d1da0bfe7853b1}};
    //Fq_mul_l1nl2n_test 3:
    FqElement pA_l1nl2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1nl2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1nl2n3= {0x0,0xc0000000,{0xe41f9cbef04da0d3,0x688ae85d2304ac,0x96aa7c6cf3ab1e4f,0x1e0b0a49c35b0816}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1nl2n0, &pB_l1nl2n0);
    Fq_mul(&Result1_c, &pA_l1nl2n1, &pB_l1nl2n1);
    Fq_mul(&Result2_c, &pA_l1nl2n2, &pB_l1nl2n2);
    Fq_mul(&Result3_c, &pA_l1nl2n3, &pB_l1nl2n3);

    compare_Result(&pResult_l1nl2n0, &Result0_c,&pA_l1nl2n0, &pB_l1nl2n0, 0, "Fq_mul_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n1, &Result1_c,&pA_l1nl2n1, &pB_l1nl2n1, 1, "Fq_mul_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n2, &Result2_c,&pA_l1nl2n2, &pB_l1nl2n2, 2, "Fq_mul_l1nl2n_unit_test");
    compare_Result(&pResult_l1nl2n3, &Result3_c,&pA_l1nl2n3, &pB_l1nl2n3, 3, "Fq_mul_l1nl2n_unit_test");
}

void Fq_mul_l1ml2n_unit_test()
{
    //Fq_mul_l1ml2n_test 0:
    FqElement pA_l1ml2n0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ml2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2n0= {0x0,0x80000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_l1ml2n_test 1:
    FqElement pA_l1ml2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ml2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1ml2n_test 2:
    FqElement pA_l1ml2n2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ml2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ml2n2= {0x0,0x80000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_l1ml2n_test 3:
    FqElement pA_l1ml2n3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ml2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ml2n3= {0x0,0x80000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1ml2n0, &pB_l1ml2n0);
    Fq_mul(&Result1_c, &pA_l1ml2n1, &pB_l1ml2n1);
    Fq_mul(&Result2_c, &pA_l1ml2n2, &pB_l1ml2n2);
    Fq_mul(&Result3_c, &pA_l1ml2n3, &pB_l1ml2n3);

    compare_Result(&pResult_l1ml2n0, &Result0_c,&pA_l1ml2n0, &pB_l1ml2n0, 0, "Fq_mul_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n1, &Result1_c,&pA_l1ml2n1, &pB_l1ml2n1, 1, "Fq_mul_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n2, &Result2_c,&pA_l1ml2n2, &pB_l1ml2n2, 2, "Fq_mul_l1ml2n_unit_test");
    compare_Result(&pResult_l1ml2n3, &Result3_c,&pA_l1ml2n3, &pB_l1ml2n3, 3, "Fq_mul_l1ml2n_unit_test");
}

void Fq_mul_l1ml2m_unit_test()
{
    //Fq_mul_l1ml2m_test 0:
    FqElement pA_l1ml2m0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ml2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2m0= {0x0,0xc0000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_l1ml2m_test 1:
    FqElement pA_l1ml2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ml2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ml2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1ml2m_test 2:
    FqElement pA_l1ml2m2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ml2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ml2m2= {0x0,0xc0000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_l1ml2m_test 3:
    FqElement pA_l1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ml2m3= {0x0,0xc0000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1ml2m0, &pB_l1ml2m0);
    Fq_mul(&Result1_c, &pA_l1ml2m1, &pB_l1ml2m1);
    Fq_mul(&Result2_c, &pA_l1ml2m2, &pB_l1ml2m2);
    Fq_mul(&Result3_c, &pA_l1ml2m3, &pB_l1ml2m3);

    compare_Result(&pResult_l1ml2m0, &Result0_c,&pA_l1ml2m0, &pB_l1ml2m0, 0, "Fq_mul_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m1, &Result1_c,&pA_l1ml2m1, &pB_l1ml2m1, 1, "Fq_mul_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m2, &Result2_c,&pA_l1ml2m2, &pB_l1ml2m2, 2, "Fq_mul_l1ml2m_unit_test");
    compare_Result(&pResult_l1ml2m3, &Result3_c,&pA_l1ml2m3, &pB_l1ml2m3, 3, "Fq_mul_l1ml2m_unit_test");
}

void Fq_mul_l1nl2m_unit_test()
{
    //Fq_mul_l1nl2m_test 0:
    //Fq_mul_l1nl2m_test 0:
    FqElement pA_l1nl2m0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1nl2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2m0= {0x0,0x80000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_l1nl2m_test 1:
    FqElement pA_l1nl2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1nl2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1nl2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1nl2m_test 2:
    FqElement pA_l1nl2m2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1nl2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1nl2m2= {0x0,0x80000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_l1nl2m_test 3:
    FqElement pA_l1nl2m3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1nl2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1nl2m3= {0x0,0x80000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1nl2m0, &pB_l1nl2m0);
    Fq_mul(&Result1_c, &pA_l1nl2m1, &pB_l1nl2m1);
    Fq_mul(&Result2_c, &pA_l1nl2m2, &pB_l1nl2m2);
    Fq_mul(&Result3_c, &pA_l1nl2m3, &pB_l1nl2m3);

    compare_Result(&pResult_l1nl2m0, &Result0_c,&pA_l1nl2m0, &pB_l1nl2m0, 0, "Fq_mul_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m1, &Result1_c,&pA_l1nl2m1, &pB_l1nl2m1, 1, "Fq_mul_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m2, &Result2_c,&pA_l1nl2m2, &pB_l1nl2m2, 2, "Fq_mul_l1nl2m_unit_test");
    compare_Result(&pResult_l1nl2m3, &Result3_c,&pA_l1nl2m3, &pB_l1nl2m3, 3, "Fq_mul_l1nl2m_unit_test");
}

void Fq_mul_l1ns2n_unit_test()
{
    //Fq_mul_l1ns2n_test 0:
    FqElement pA_l1ns2n0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ns2n0= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns2n0= {0x0,0xc0000000,{0xa6ba871b8b1e1b3a,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_mul_l1ns2n_test 1:
    FqElement pA_l1ns2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ns2n1= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1ns2n_test 2:
    FqElement pA_l1ns2n2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ns2n2= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ns2n2= {0x0,0xc0000000,{0xba658bb3c5668e7a,0x8b6747b10d51d35a,0x871359d9f90f6f90,0xfd7c8811e0fe4b}};
    //Fq_mul_l1ns2n_test 3:
    FqElement pA_l1ns2n3= {0x7fffffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ns2n3= {-1,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ns2n3= {0x0,0xc0000000,{0xe030473272041314,0x5491d21721820941,0x1ec384706e37c635,0x731d84fcf4faa10}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1ns2n0, &pB_l1ns2n0);
    Fq_mul(&Result1_c, &pA_l1ns2n1, &pB_l1ns2n1);
    Fq_mul(&Result2_c, &pA_l1ns2n2, &pB_l1ns2n2);
    Fq_mul(&Result3_c, &pA_l1ns2n3, &pB_l1ns2n3);

    compare_Result(&pResult_l1ns2n0, &Result0_c,&pA_l1ns2n0, &pB_l1ns2n0, 0, "Fq_mul_l1ns2n_unit_test");
    compare_Result(&pResult_l1ns2n1, &Result1_c,&pA_l1ns2n1, &pB_l1ns2n1, 1, "Fq_mul_l1ns2n_unit_test");
    compare_Result(&pResult_l1ns2n2, &Result2_c,&pA_l1ns2n2, &pB_l1ns2n2, 2, "Fq_mul_l1ns2n_unit_test");
    compare_Result(&pResult_l1ns2n3, &Result3_c,&pA_l1ns2n3, &pB_l1ns2n3, 3, "Fq_mul_l1ns2n_unit_test");
}

void Fq_mul_s1nl2n_unit_test()
{
    //Fq_mul_s1nl2n_test 0:
    FqElement pA_s1nl2n0= {0x1,0x0,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1nl2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2n0= {0x0,0xc0000000,{0xa6ba871b8b1e1b3a,0x14f1d651eb8e167b,0xccdd46def0f28c58,0x1c14ef83340fbe5e}};
    //Fq_mul_s1nl2n_test 1:
    FqElement pA_s1nl2n1= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1nl2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_s1nl2n_test 2:
    FqElement pA_s1nl2n2= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1nl2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1nl2n2= {0x0,0xc0000000,{0xa1ebd3b0c50a79a5,0x991c1c5109e913a5,0x556dc7319816b73,0x12e84d0df59a5777}};
    //Fq_mul_s1nl2n_test 3:
    FqElement pA_s1nl2n3= {-1,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1nl2n3= {0x7fffffff,0x80000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_s1nl2n3= {0x0,0xc0000000,{0xf7d471598746b6aa,0xc5baff5c4b315cae,0x5913c7393800d697,0x3030eabd6004a0f9}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_s1nl2n0, &pB_s1nl2n0);
    Fq_mul(&Result1_c, &pA_s1nl2n1, &pB_s1nl2n1);
    Fq_mul(&Result2_c, &pA_s1nl2n2, &pB_s1nl2n2);
    Fq_mul(&Result3_c, &pA_s1nl2n3, &pB_s1nl2n3);

    compare_Result(&pResult_s1nl2n0, &Result0_c,&pA_s1nl2n0, &pB_s1nl2n0, 0, "Fq_mul_s1nl2n_unit_test");
    compare_Result(&pResult_s1nl2n1, &Result1_c,&pA_s1nl2n1, &pB_s1nl2n1, 1, "Fq_mul_s1nl2n_unit_test");
    compare_Result(&pResult_s1nl2n2, &Result2_c,&pA_s1nl2n2, &pB_s1nl2n2, 2, "Fq_mul_s1nl2n_unit_test");
    compare_Result(&pResult_s1nl2n3, &Result3_c,&pA_s1nl2n3, &pB_s1nl2n3, 3, "Fq_mul_s1nl2n_unit_test");
}

void Fq_mul_s1nl2m_unit_test()
{
    //Fq_mul_s1nl2m_test 0:
    FqElement pA_s1nl2m0= {0x1,0x0,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1nl2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2m0= {0x0,0x80000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_s1nl2m_test 1:
    FqElement pA_s1nl2m1= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1nl2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1nl2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_s1nl2m_test 2:
    FqElement pA_s1nl2m2= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1nl2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1nl2m2= {0x0,0x80000000,{0xf8fb48ccc33018d3,0xc94964a5af8c4718,0x1a3ee6c0af9b914e,0x137994681281dfa3}};
    //Fq_mul_s1nl2m_test 3:
    FqElement pA_s1nl2m3= {-1,0x0,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1nl2m3= {0x7fffffff,0xc0000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_s1nl2m3= {0x0,0x80000000,{0x950091e095a5f7d6,0x3ac97dbc6f34b24d,0xbc48958051e56dce,0x1625d680784e8f0f}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_s1nl2m0, &pB_s1nl2m0);
    Fq_mul(&Result1_c, &pA_s1nl2m1, &pB_s1nl2m1);
    Fq_mul(&Result2_c, &pA_s1nl2m2, &pB_s1nl2m2);
    Fq_mul(&Result3_c, &pA_s1nl2m3, &pB_s1nl2m3);

    compare_Result(&pResult_s1nl2m0, &Result0_c,&pA_s1nl2m0, &pB_s1nl2m0, 0, "Fq_mul_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m1, &Result1_c,&pA_s1nl2m1, &pB_s1nl2m1, 1, "Fq_mul_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m2, &Result2_c,&pA_s1nl2m2, &pB_s1nl2m2, 2, "Fq_mul_s1nl2m_unit_test");
    compare_Result(&pResult_s1nl2m3, &Result3_c,&pA_s1nl2m3, &pB_s1nl2m3, 3, "Fq_mul_s1nl2m_unit_test");
}

void Fq_mul_l1ms2n_unit_test()
{
    //Fq_mul_l1ms2n_test 0:
    FqElement pA_l1ms2n0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ms2n0= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2n0= {0x0,0x80000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_l1ms2n_test 1:
    FqElement pA_l1ms2n1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ms2n1= {0x2,0x0,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1ms2n_test 2:
    FqElement pA_l1ms2n2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ms2n2= {0x1bb8,0x0,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ms2n2= {0x0,0x80000000,{0xd48ef8eb6f0a70a7,0x83590aa4708b6780,0x6603a7198a84f5b5,0x27049057c6edb906}};
    //Fq_mul_l1ms2n_test 3:
    FqElement pA_l1ms2n3= {0xffff,0xc0000000,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pB_l1ms2n3= {-1,0x0,{0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff,0x7fffffffffffffff}};
    FqElement pResult_l1ms2n3= {0x0,0x80000000,{0x950091e095a5f7d6,0x3ac97dbc6f34b24d,0xbc48958051e56dce,0x1625d680784e8f0f}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1ms2n0, &pB_l1ms2n0);
    Fq_mul(&Result1_c, &pA_l1ms2n1, &pB_l1ms2n1);
    Fq_mul(&Result2_c, &pA_l1ms2n2, &pB_l1ms2n2);
    Fq_mul(&Result3_c, &pA_l1ms2n3, &pB_l1ms2n3);

    compare_Result(&pResult_l1ms2n0, &Result0_c,&pA_l1ms2n0, &pB_l1ms2n0, 0, "Fq_mul_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n1, &Result1_c,&pA_l1ms2n1, &pB_l1ms2n1, 1, "Fq_mul_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n2, &Result2_c,&pA_l1ms2n2, &pB_l1ms2n2, 2, "Fq_mul_l1ms2n_unit_test");
    compare_Result(&pResult_l1ms2n3, &Result3_c,&pA_l1ms2n3, &pB_l1ms2n3, 3, "Fq_mul_l1ms2n_unit_test");
}

void Fq_mul_l1ns2m_unit_test()
{
    //Fq_mul_l1ns2m_test 0:
    FqElement pA_l1ns2m0= {0x1,0x80000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ns2m0= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns2m0= {0x0,0x80000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_l1ns2m_test 1:
    FqElement pA_l1ns2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ns2m1= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ns2m1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1ns2m_test 2:
    FqElement pA_l1ns2m2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ns2m2= {0x1bb8,0x40000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ns2m2= {0x0,0x80000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_l1ns2m_test 3:
    FqElement pA_l1ns2m3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ns2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ns2m3= {0x0,0x80000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1ns2m0, &pB_l1ns2m0);
    Fq_mul(&Result1_c, &pA_l1ns2m1, &pB_l1ns2m1);
    Fq_mul(&Result2_c, &pA_l1ns2m2, &pB_l1ns2m2);
    Fq_mul(&Result3_c, &pA_l1ns2m3, &pB_l1ns2m3);

    compare_Result(&pResult_l1ns2m0, &Result0_c,&pA_l1ns2m0, &pB_l1ns2m0, 0, "Fq_mul_l1ns2m_unit_test");
    compare_Result(&pResult_l1ns2m1, &Result1_c,&pA_l1ns2m1, &pB_l1ns2m1, 1, "Fq_mul_l1ns2m_unit_test");
    compare_Result(&pResult_l1ns2m2, &Result2_c,&pA_l1ns2m2, &pB_l1ns2m2, 2, "Fq_mul_l1ns2m_unit_test");
    compare_Result(&pResult_l1ns2m3, &Result3_c,&pA_l1ns2m3, &pB_l1ns2m3, 3, "Fq_mul_l1ns2m_unit_test");
}

void Fq_mul_l1ms2m_unit_test()
{
    //Fq_mul_l1ms2m_test 0:
    FqElement pA_l1ms2m0= {0x1,0xc0000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_l1ms2m0= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2m0= {0x0,0xc0000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_l1ms2m_test 1:
    FqElement pA_l1ms2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_l1ms2m1= {0x2,0x40000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_l1ms2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_l1ms2m_test 2:
    FqElement pA_l1ms2m2= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_l1ms2m2= {0x1bb8,0x40000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_l1ms2m2= {0x0,0xc0000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_l1ms2m_test 3:
    FqElement pA_l1ms2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_l1ms2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_l1ms2m3= {0x0,0xc0000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_l1ms2m0, &pB_l1ms2m0);
    Fq_mul(&Result1_c, &pA_l1ms2m1, &pB_l1ms2m1);
    Fq_mul(&Result2_c, &pA_l1ms2m2, &pB_l1ms2m2);
    Fq_mul(&Result3_c, &pA_l1ms2m3, &pB_l1ms2m3);

    compare_Result(&pResult_l1ms2m0, &Result0_c,&pA_l1ms2m0, &pB_l1ms2m0, 0, "Fq_mul_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m1, &Result1_c,&pA_l1ms2m1, &pB_l1ms2m1, 1, "Fq_mul_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m2, &Result2_c,&pA_l1ms2m2, &pB_l1ms2m2, 2, "Fq_mul_l1ms2m_unit_test");
    compare_Result(&pResult_l1ms2m3, &Result3_c,&pA_l1ms2m3, &pB_l1ms2m3, 3, "Fq_mul_l1ms2m_unit_test");
}

void Fq_mul_s1ml2m_unit_test()
{
    //Fq_mul_s1ml2m_test 0:
    FqElement pA_s1ml2m0= {0x1,0x40000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1ml2m0= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2m0= {0x0,0xc0000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_s1ml2m_test 1:
    FqElement pA_s1ml2m1= {0x0,0x40000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1ml2m1= {0x2,0xc0000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2m1= {0x0,0xc0000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_s1ml2m_test 2:
    FqElement pA_s1ml2m2= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1ml2m2= {0x1bb8,0xc0000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1ml2m2= {0x0,0xc0000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_s1ml2m_test 3:
    FqElement pA_s1ml2m3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1ml2m3= {0xffff,0xc0000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1ml2m3= {0x0,0xc0000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_s1ml2m0, &pB_s1ml2m0);
    Fq_mul(&Result1_c, &pA_s1ml2m1, &pB_s1ml2m1);
    Fq_mul(&Result2_c, &pA_s1ml2m2, &pB_s1ml2m2);
    Fq_mul(&Result3_c, &pA_s1ml2m3, &pB_s1ml2m3);

    compare_Result(&pResult_s1ml2m0, &Result0_c,&pA_s1ml2m0, &pB_s1ml2m0, 0, "Fq_mul_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m1, &Result1_c,&pA_s1ml2m1, &pB_s1ml2m1, 1, "Fq_mul_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m2, &Result2_c,&pA_s1ml2m2, &pB_s1ml2m2, 2, "Fq_mul_s1ml2m_unit_test");
    compare_Result(&pResult_s1ml2m3, &Result3_c,&pA_s1ml2m3, &pB_s1ml2m3, 3, "Fq_mul_s1ml2m_unit_test");
}

void Fq_mul_s1ml2n_unit_test()
{
    //Fq_mul_s1ml2n_test 0:
    FqElement pA_s1ml2n0= {0x1,0x40000000,{0x1,0x0,0x0,0x0}};
    FqElement pB_s1ml2n0= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2n0= {0x0,0x80000000,{0x9ee8847d2a18f727,0x3ebeda789c801164,0xe6778de8ed07cd56,0x2c69dc6fd299ec49}};
    //Fq_mul_s1ml2n_test 1:
    FqElement pA_s1ml2n1= {0x0,0x40000000,{0x0,0x0,0x0,0x0}};
    FqElement pB_s1ml2n1= {0x2,0x80000000,{0x2,0x0,0x0,0x0}};
    FqElement pResult_s1ml2n1= {0x0,0x80000000,{0x0,0x0,0x0,0x0}};
    //Fq_mul_s1ml2n_test 2:
    FqElement pA_s1ml2n2= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pB_s1ml2n2= {0x1bb8,0x80000000,{0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5}};
    FqElement pResult_s1ml2n2= {0x0,0x80000000,{0x1187da3e296269a8,0xd0139eb206e57eeb,0xdb5973382f0e9301,0x2e40d99a3c8089fb}};
    //Fq_mul_s1ml2n_test 3:
    FqElement pA_s1ml2n3= {0xffff,0x40000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pB_s1ml2n3= {0xffff,0x80000000,{0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff}};
    FqElement pResult_s1ml2n3= {0x0,0x80000000,{0xc5687d1b9df6a369,0xbf4f15d4ad74848f,0x3e250df1ad65c620,0x55308b909171d71}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_mul(&Result0_c, &pA_s1ml2n0, &pB_s1ml2n0);
    Fq_mul(&Result1_c, &pA_s1ml2n1, &pB_s1ml2n1);
    Fq_mul(&Result2_c, &pA_s1ml2n2, &pB_s1ml2n2);
    Fq_mul(&Result3_c, &pA_s1ml2n3, &pB_s1ml2n3);

    compare_Result(&pResult_s1ml2n0, &Result0_c,&pA_s1ml2n0, &pB_s1ml2n0, 0, "Fq_mul_s1ml2n_unit_test");
    compare_Result(&pResult_s1ml2n1, &Result1_c,&pA_s1ml2n1, &pB_s1ml2n1, 1, "Fq_mul_s1ml2n_unit_test");
    compare_Result(&pResult_s1ml2n2, &Result2_c,&pA_s1ml2n2, &pB_s1ml2n2, 2, "Fq_mul_s1ml2n_unit_test");
    compare_Result(&pResult_s1ml2n3, &Result3_c,&pA_s1ml2n3, &pB_s1ml2n3, 3, "Fq_mul_s1ml2n_unit_test");
}

void Fq_square_test(FqElement r_expected, FqElement a, int index)
{
    FqElement r_computed = {0,0,{0,0,0,0}};

    Fq_square(&r_computed, &a);

    compare_Result_single(&r_expected, &r_computed, &a, index, __func__);
}

void Fq_square_short_test(int64_t r_expected, int32_t a, int index)
{
    Fq_square_test(fq_long(r_expected, 0, 0, 0), fq_short(a), index);
}

void Fq_square_unit_test()
{
    Fq_square_short_test(0,                0, 0);
    Fq_square_short_test(1,                1, 1);
    Fq_square_short_test(1,               -1, 2);
    Fq_square_short_test(4,                2, 3);
    Fq_square_short_test(4,               -2, 4);
    Fq_square_short_test(65536,          256, 5);
    Fq_square_short_test(65536,         -256, 6);
    Fq_square_short_test(1067851684,   32678, 7);
    Fq_square_short_test(4294967296,   65536, 8);
    Fq_square_short_test(68719476736, 262144, 9);

    FqElement a1 = fq_short(1048576);
    FqElement a2 = fq_short(16777216);
    FqElement a3 = fq_short(-16777216);
    FqElement a4 = fq_short(2147483647);
    FqElement a5 = fq_short(-2147483647);

    FqElement r1 = fq_long(0x0000010000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000);
    FqElement r2 = fq_long(0x0001000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000);
    FqElement r3 = fq_long(0x0001000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000);
    FqElement r4 = fq_long(0x3fffffff00000001,0x0000000000000000,0x0000000000000000,0x0000000000000000);
    FqElement r5 = fq_long(0x3fffffff00000001,0x0000000000000000,0x0000000000000000,0x0000000000000000);

    Fq_square_test(r1, a1, 11);
    Fq_square_test(r2, a2, 12);
    Fq_square_test(r3, a3, 13);
    Fq_square_test(r4, a4, 14);
    Fq_square_test(r5, a5, 15);

    FqElement a21 = fq_long(0x43e1f593f0000001,0x2833e84879b97091,0xb85045b68181585d,0x30644e72e131a029);
    FqElement a22 = fq_long(0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x0216d0b17f4e44a5);
    FqElement a23 = fq_long(0x5e94d8e1b4bf0040,0x2a489cbe1cfbb6b8,0x893cc664a19fcfed,0x0cf8594b7fcc657c);
    FqElement a24 = fq_long(0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014);
    FqElement a25 = fq_long(0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5);
    FqElement a26 = fq_long(0x1bb8e645ae216da7, 0, 0, 0);

    FqElement r21 = fq_mlong(0xa36e3db5ee5759d2,0x38352a2f67411193,0x827c17423bfb203b,0x2429ad301b3328c5);
    FqElement r22 = fq_mlong(0xcbe13c2bfb664022,0x50f6618240404b24,0xdac1e4a17673233b,0x0583edb5fdd86f35);
    FqElement r23 = fq_mlong(0xa1a671141ea315b1,0x1254a305ec52f02b,0x5c4b5ed24a33f0e3,0x1d80794f124ebcea);
    FqElement r24 = fq_mlong(0x58866a06a6cf3ccd,0xe7675ddd29531728,0xbca78e187e5fec64,0x05aaaec9bf8478e8);
    FqElement r25 = fq_mlong(0xcbe13c2bfb664022,0x50f6618240404b24,0xdac1e4a17673233b,0x0583edb5fdd86f35);
    FqElement r26 = fq_mlong(0xbd7c163fbc00a4c3,0xb02513c97a803400,0x1a4492de859a2863,0x0c878a77effa01c6);

    Fq_square_test(r21, a21, 21);
    Fq_square_test(r22, a22, 22);
    Fq_square_test(r23, a23, 23);
    Fq_square_test(r24, a24, 24);
    Fq_square_test(r25, a25, 25);
    Fq_square_test(r26, a26, 26);

    FqElement a31 = fq_mlong(0x43e1f593f0000001,0x2833e84879b97091,0xb85045b68181585d,0x30644e72e131a029);
    FqElement a32 = fq_mlong(0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x0216d0b17f4e44a5);
    FqElement a33 = fq_mlong(0x5e94d8e1b4bf0040,0x2a489cbe1cfbb6b8,0x893cc664a19fcfed,0x0cf8594b7fcc657c);
    FqElement a34 = fq_mlong(0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014);
    FqElement a35 = fq_mlong(0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5);
    FqElement a36 = fq_mlong(0x1bb8e645ae216da7, 0, 0, 0);

    FqElement r31 = fq_mlong(0x355fdbd1472c705a,0x4cc7e466a7fbb77f,0x8658fb1c77f4a809,0x23aef213fb88c295);
    FqElement r32 = fq_mlong(0x1fa2e058e64e824a,0x053324c431844d78,0x4bf3dac062ea6dad,0x2db3e562977df94a);
    FqElement r33 = fq_mlong(0x644c5ce20a8793bb,0xebc09ef48a61c906,0x0281385bd1007d0c,0x1bce0f38b8cdaad9);
    FqElement r34 = fq_mlong(0xac0b6f222f67487d,0x364d764ea56127d9,0xe5ad1f8aa6ef1ae1,0x2dffef30a4034c35);
    FqElement r35 = fq_mlong(0x1fa2e058e64e824a,0x053324c431844d78,0x4bf3dac062ea6dad,0x2db3e562977df94a);
    FqElement r36 = fq_mlong(0x49d481ec59aa5401,0x804ca61c080d6da3,0x4e6b2f7e337fa8d1,0x0f2dcfc4e7661f81);

    Fq_square_test(r31, a31, 31);
    Fq_square_test(r32, a32, 32);
    Fq_square_test(r33, a33, 33);
    Fq_square_test(r34, a34, 34);
    Fq_square_test(r35, a35, 35);
    Fq_square_test(r36, a36, 36);
}

void Fq_rawCopyS2L_test(FqRawElement r_expected, int64_t a, int idx)
{
    FqRawElement r_computed = {0xb,0xa,0xd,0xd};

    Fq_rawCopyS2L(r_computed, a);

    compare_raw_result(r_expected, r_computed, a, idx, 0, __func__);
}

void Fq_rawCopyS2L_unit_test()
{
    int64_t      a0 = 1;
    FqRawElement r0 = {1,0,0,0};

    int64_t      a1 = -1;
    FqRawElement r1 = {0x3c208c16d87cfd46, 0x97816a916871ca8d, 0xb85045b68181585d, 0x30644e72e131a029};

    int64_t      a2 = -2224;
    FqRawElement r2 = {0x3c208c16d87cf497, 0x97816a916871ca8d, 0xb85045b68181585d, 0x30644e72e131a029};

    int64_t      a3 = 0;
    FqRawElement r3 = {0,0,0,0};

    int64_t      a4 =  2224;
    FqRawElement r4 = {2224,0,0,0};

    Fq_rawCopyS2L_test(r0, a0, 0);
    Fq_rawCopyS2L_test(r1, a1, 1);
    Fq_rawCopyS2L_test(r2, a2, 2);
    Fq_rawCopyS2L_test(r3, a3, 3);
    Fq_rawCopyS2L_test(r4, a4, 4);
}

void Fq_Rw_copy_unit_test()
{
    //Fq_Rw_copy_test 0:
    FqRawElement pRawA0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
    FqRawElement pRawResult0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
    //Fq_Rw_copy_test 1:
    FqRawElement pRawA1= {0x1,0x0,0x0,0x0};
    FqRawElement pRawResult1= {0x1,0x0,0x0,0x0};
    //Fq_Rw_copy_test 2:
    FqRawElement pRawA2= {0xfffffffffffffffe,0x0,0x0,0x0};
    FqRawElement pRawResult2= {0xfffffffffffffffe,0x0,0x0,0x0};
    //Fq_Rw_copy_test 3:
    FqRawElement pRawA3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};
    FqRawElement pRawResult3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};

    FqRawElement pRawResult0_c;
    FqRawElement pRawResult1_c;
    FqRawElement pRawResult2_c;
    FqRawElement pRawResult3_c;

    Fq_rawCopy(pRawResult0_c, pRawA0);
    Fq_rawCopy(pRawResult1_c, pRawA1);
    Fq_rawCopy(pRawResult2_c, pRawA2);
    Fq_rawCopy(pRawResult3_c, pRawA3);

    compare_Result(pRawResult0, pRawResult0_c,pRawA0,pRawA0, 0, "Fq_Rw_copy_unit_test");
    compare_Result(pRawResult1, pRawResult1_c,pRawA1,pRawA1, 1, "Fq_Rw_copy_unit_test");
    compare_Result(pRawResult2, pRawResult2_c,pRawA2,pRawA2, 2, "Fq_Rw_copy_unit_test");
    compare_Result(pRawResult3, pRawResult3_c,pRawA3,pRawA3, 3, "Fq_Rw_copy_unit_test");
}

//void Fq_Rw_add_unit_test()
//{
//    //Fq_rawAdd Test 0:
//    FqRawElement pRawA0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
//    FqRawElement pRawB0= {0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5};
//    FqRawElement pRawResult0= {0xbda9e10fa6216da7,0xe8182ed62039122b,0x6871a618947c2cb3,0x1a48f7eaefe714ba};
//    //Fq_rawAdd Test 1:
//    FqRawElement pRawA1= {0x1,0x0,0x0,0x0};
//    FqRawElement pRawB1= {0x2,0x0,0x0,0x0};
//    FqRawElement pRawResult1= {0x3,0x0,0x0,0x0};
//    //Fq_rawAdd Test 2:
//    FqRawElement pRawA2= {0xfffffffffffffffe,0x0,0x0,0x0};
//    FqRawElement pRawB2= {0xffffffffffffffff,0x0,0x0,0x0};
//    FqRawElement pRawResult2= {0xfffffffffffffffd,0x1,0x0,0x0};
//    //Fq_rawAdd Test 3:
//    FqRawElement pRawA3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};
//    FqRawElement pRawB3= {0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff};
//    FqRawElement pRawResult3= {0xc3df73e9278302b6,0x687e956e978e3571,0x47afba497e7ea7a1,0xcf9bb18d1ece5fd5};
//    //Fq_rawAdd Test 6:
//    FqRawElement pRawA6= {0x3c208c16d87cfd47,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029};
//    FqRawElement pRawB6= {0x0,0x0,0x0,0x0};
//    FqRawElement pRawResult6= {0x0,0x0,0x0,0x0};
//
//    FqRawElement pRawResult0_c;
//    FqRawElement pRawResult1_c;
//    FqRawElement pRawResult2_c;
//    FqRawElement pRawResult3_c;
//    FqRawElement pRawResult6_c;
//
//    Fq_rawAdd(pRawResult0_c, pRawA0, pRawB0);
//    Fq_rawAdd(pRawResult1_c, pRawA1, pRawB1);
//    Fq_rawAdd(pRawResult2_c, pRawA2, pRawB2);
//    Fq_rawAdd(pRawResult3_c, pRawA3, pRawB3);
//    Fq_rawAdd(pRawResult6_c, pRawA6, pRawB6);
//
//    compare_Result(pRawResult0, pRawResult0_c,pRawA0,pRawB0,  0, "Fq_Rw_add_unit_test");
//    compare_Result(pRawResult1, pRawResult1_c,pRawA1, pRawB1, 1, "Fq_Rw_add_unit_test");
//    compare_Result(pRawResult2, pRawResult2_c,pRawA2, pRawB2, 2, "Fq_Rw_add_unit_test");
//    compare_Result(pRawResult3, pRawResult3_c,pRawA3, pRawB3, 3, "Fq_Rw_add_unit_test");
//    compare_Result(pRawResult6, pRawResult6_c,pRawA6, pRawB6, 6, "Fq_Rw_add_unit_test");
//}

void Fq_Rw_sub_unit_test()
{
    //Fq_Rw_sub_test 0:
    FqRawElement pRawA0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
    FqRawElement pRawB0= {0x1bb8e645ae216da7,0x53fe3ab1e35c59e3,0x8c49833d53bb8085,0x216d0b17f4e44a5};
    FqRawElement pRawResult0= {0x8638148449de9259,0x401bb97259805e65,0x4fde9f9ded052ba9,0x161b5687f14a8b6f};
    //Fq_Rw_sub_test 1:
    FqRawElement pRawA1= {0x1,0x0,0x0,0x0};
    FqRawElement pRawB1= {0x2,0x0,0x0,0x0};
    FqRawElement pRawResult1= {0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029};
    //Fq_Rw_sub_test 2:
    FqRawElement pRawA2= {0xfffffffffffffffe,0x0,0x0,0x0};
    FqRawElement pRawB2= {0xffffffffffffffff,0x0,0x0,0x0};
    FqRawElement pRawResult2= {0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029};
    //Fq_Rw_sub_test 3:
    FqRawElement pRawA3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};
    FqRawElement pRawB3= {0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff,0xffffffffffffffff};
    FqRawElement pRawResult3= {0x3c208c16d87cfd46,0x97816a916871ca8c,0xb85045b68181585c,0x30644e72e131a028};

    FqRawElement pRawResult0_c;
    FqRawElement pRawResult1_c;
    FqRawElement pRawResult2_c;
    FqRawElement pRawResult3_c;

    Fq_rawSub(pRawResult0_c, pRawA0, pRawB0);
    Fq_rawSub(pRawResult1_c, pRawA1, pRawB1);
    Fq_rawSub(pRawResult2_c, pRawA2, pRawB2);
    Fq_rawSub(pRawResult3_c, pRawA3, pRawB3);

    compare_Result(pRawResult0, pRawResult0_c,pRawA0, pRawB0, 0, "Fq_Rw_sub_unit_test");
    compare_Result(pRawResult1, pRawResult1_c,pRawA1, pRawB1, 1, "Fq_Rw_sub_unit_test");
    compare_Result(pRawResult2, pRawResult2_c,pRawA2, pRawB2, 2, "Fq_Rw_sub_unit_test");
    compare_Result(pRawResult3, pRawResult3_c,pRawA3, pRawB3, 3, "Fq_Rw_sub_unit_test");
}

void Fq_Rw_Neg_unit_test()
{
    //Fr_Rw_Neg_test 0:
    FqRawElement pRawA0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
    FqRawElement pRawResult0= {0x9a2f914ce07cfd47,0x367766d2b951244,0xdc2822db40c0ac2f,0x183227397098d014};
    //Fr_Rw_Neg_test 1:
    FqRawElement pRawA1= {0x1,0x0,0x0,0x0};
    FqRawElement pRawResult1= {0x3c208c16d87cfd46,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029};
    //Fr_Rw_Neg_test 2:
    FqRawElement pRawA2= {0xfffffffffffffffe,0x0,0x0,0x0};
    FqRawElement pRawResult2= {0x3c208c16d87cfd49,0x97816a916871ca8c,0xb85045b68181585d,0x30644e72e131a029};
    //Fr_Rw_Neg_test 3:
    FqRawElement pRawA3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};
    FqRawElement pRawResult3= {0x3c208c16d87cfd49,0x97816a916871ca8e,0xb85045b68181585e,0x30644e72e131a02a};
    //Fr_Rw_Neg_test 5:
    FqRawElement pRawA5= {0x0,0x0,0x0,0x0};
    FqRawElement pRawResult5= {0x0,0x0,0x0,0x0};

    FqRawElement pRawResult0_c;
    FqRawElement pRawResult1_c;
    FqRawElement pRawResult2_c;
    FqRawElement pRawResult3_c;
    FqRawElement pRawResult5_c;

    Fq_rawNeg(pRawResult0_c, pRawA0);
    Fq_rawNeg(pRawResult1_c, pRawA1);
    Fq_rawNeg(pRawResult2_c, pRawA2);
    Fq_rawNeg(pRawResult3_c, pRawA3);
    Fq_rawNeg(pRawResult5_c, pRawA5);

    compare_Result(pRawResult0, pRawResult0_c,pRawA0,pRawA0, 0, "Fq_Rw_Neg_unit_test");
    compare_Result(pRawResult1, pRawResult1_c,pRawA1,pRawA1, 1, "Fq_Rw_Neg_unit_test");
    compare_Result(pRawResult2, pRawResult2_c,pRawA2,pRawA2, 2, "Fq_Rw_Neg_unit_test");
    compare_Result(pRawResult3, pRawResult3_c,pRawA3,pRawA3, 3, "Fq_Rw_Neg_unit_test");
    compare_Result(pRawResult5, pRawResult5_c,pRawA5,pRawA5, 5, "Fq_Rw_Neg_unit_test");
}

void Fq_Rw_Msquare_unit_test()
{
    //Fq_Rw_Msquare_test 0:
    FqRawElement pRawA0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
    FqRawElement pRawResult0= {0xac0b6f222f67487d,0x364d764ea56127d9,0xe5ad1f8aa6ef1ae1,0x2dffef30a4034c35};
    //Fq_Rw_Msquare_test 1:
    FqRawElement pRawA1= {0x1,0x0,0x0,0x0};
    FqRawElement pRawResult1= {0xed84884a014afa37,0xeb2022850278edf8,0xcf63e9cfb74492d9,0x2e67157159e5c639};
    //Fq_Rw_Msquare_test 2:
    FqRawElement pRawA2= {0xfffffffffffffffe,0x0,0x0,0x0};
    FqRawElement pRawResult2= {0xcac67c2bcf3f94c9,0xb20d5c033f4b535e,0xad88b23a6703c471,0x3688947d16d07fa};
    //Fq_Rw_Msquare_test 3:
    FqRawElement pRawA3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};
    FqRawElement pRawResult3= {0x4c78ebc8ab4ce00d,0xdcbaf4c118eb7001,0x1c8e537a8c87e0f4,0x1fdf7ac5e6e8ec32};

    FqRawElement pRawResult0_c;
    FqRawElement pRawResult1_c;
    FqRawElement pRawResult2_c;
    FqRawElement pRawResult3_c;

    Fq_rawMSquare(pRawResult0_c, pRawA0);
    Fq_rawMSquare(pRawResult1_c, pRawA1);
    Fq_rawMSquare(pRawResult2_c, pRawA2);
    Fq_rawMSquare(pRawResult3_c, pRawA3);

    compare_Result(pRawResult0, pRawResult0_c,pRawA0,pRawA0, 0, "Fq_Rw_Msquare_unit_test");
    compare_Result(pRawResult1, pRawResult1_c,pRawA1,pRawA1, 1, "Fq_Rw_Msquare_unit_test");
    compare_Result(pRawResult2, pRawResult2_c,pRawA2,pRawA2, 2, "Fq_Rw_Msquare_unit_test");
    compare_Result(pRawResult3, pRawResult3_c,pRawA3,pRawA3, 3, "Fq_Rw_Msquare_unit_test");
}

void Fq_rawIsZero_unit_test()
{
    //Fq_rawIsZero_test 0:
    FqRawElement pRawA0= {0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014};
    FqRawElement pRawResult0= {0x0};
    //Fq_rawIsZero_test 1:
    FqRawElement pRawA1= {0x1,0x0,0x0,0x0};
    FqRawElement pRawResult1= {0x0};
    //Fq_rawIsZero_test 2:
    FqRawElement pRawA2= {0xfffffffffffffffe,0x0,0x0,0x0};
    FqRawElement pRawResult2= {0x0};
    //Fq_rawIsZero_test 3:
    FqRawElement pRawA3= {0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe,0xfffffffffffffffe};
    FqRawElement pRawResult3= {0x0};
    //Fq_rawIsZero_test 5:
    FqRawElement pRawA5= {0x0,0x0,0x0,0x0};
    FqRawElement pRawResult5= {0x1};

    FqRawElement pRawResult0_c = {0};
    FqRawElement pRawResult1_c = {0};
    FqRawElement pRawResult2_c = {0};
    FqRawElement pRawResult3_c = {0};
    FqRawElement pRawResult5_c = {0};

    pRawResult0_c[0] = Fq_rawIsZero(pRawA0);
    pRawResult1_c[0] = Fq_rawIsZero(pRawA1);
    pRawResult2_c[0] = Fq_rawIsZero(pRawA2);
    pRawResult3_c[0] = Fq_rawIsZero(pRawA3);
    pRawResult5_c[0] = Fq_rawIsZero(pRawA5);

    compare_Result(pRawResult0, pRawResult0_c,pRawA0,pRawA0, 0, "Fq_rawIsZero_unit_test");
    compare_Result(pRawResult1, pRawResult1_c,pRawA1,pRawA1, 1, "Fq_rawIsZero_unit_test");
    compare_Result(pRawResult2, pRawResult2_c,pRawA2,pRawA2, 2, "Fq_rawIsZero_unit_test");
    compare_Result(pRawResult3, pRawResult3_c,pRawA3,pRawA3, 3, "Fq_rawIsZero_unit_test");
    compare_Result(pRawResult5, pRawResult5_c,pRawA5,pRawA5, 5, "Fq_rawIsZero_unit_test");
}

void Fq_copy_unit_test()
{
    //Fq_copy_test 0:
    FqElement pA0= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult0= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_copy_test 1:
    FqElement pA1= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult1= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_copy_test 2:
    FqElement pA2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_copy_test 3:
    FqElement pA3= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult3= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};


    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_copy(&Result0_c, &pA0);
    Fq_copy(&Result1_c, &pA1);
    Fq_copy(&Result2_c, &pA2);
    Fq_copy(&Result3_c, &pA3);

    compare_Result(&pResult0, &Result0_c, &pA0, &pA0, 0, "Fq_copy_unit_test");
    compare_Result(&pResult1, &Result1_c, &pA1, &pA1, 1, "Fq_copy_unit_test");
    compare_Result(&pResult2, &Result2_c, &pA2, &pA2, 2, "Fq_copy_unit_test");
    compare_Result(&pResult3, &Result3_c, &pA3, &pA3, 3, "Fq_copy_unit_test");
}

void Fq_copyn_unit_test()
{
    //Fq_copy_test 0:
    FqElement pA0= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult0= {0x0,0x0,{0x0,0x0,0x0,0x0}};
    //Fq_copy_test 1:
    FqElement pA1= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult1= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_copy_test 2:
    FqElement pA2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //Fq_copy_test 3:
    FqElement pA3= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult3= {0x0,0x0,{0x0,0x0,0x0,0x0}};


    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};

    Fq_copyn(&Result0_c, &pA0,0);
    Fq_copyn(&Result1_c, &pA1,1);
    Fq_copyn(&Result2_c, &pA2,1);
    Fq_copyn(&Result3_c, &pA3,0);

    compare_Result(&pResult0, &Result0_c, &pA0, &pA0, 0, "Fq_copyn_unit_test");
    compare_Result(&pResult1, &Result1_c, &pA1, &pA1, 1, "Fq_copyn_unit_test");
    compare_Result(&pResult2, &Result2_c, &pA2, &pA2, 2, "Fq_copyn_unit_test");
    compare_Result(&pResult3, &Result3_c, &pA3, &pA3, 3, "Fq_copyn_unit_test");
}

void Fq_neg_unit_test()
{
    //Fq_neg_test 0:
    FqElement pA0= {0xa1f0,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    //FqElement pResult0= {0xffff5e10,0x0,{0x0,0x0,0x0,0x0}};
    FqElement pResult0= {-41456,0x0,{0x0,0x0,0x0,0x0}};
    //Fq_neg_test 1:
    FqElement pA1= {0xa1f0,0x40000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult1= {-41456,0x0,{0x0,0x0,0x0,0x0}};
    //FqElement pResult1= {0xffff5e10,0x0,{0x0,0x0,0x0,0x0}};
    //Fq_neg_test 2:
    FqElement pA2= {0xa1f0,0x80000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult2= {0xa1f0,0x80000000,{0x9a2f914ce07cfd47,0x0367766d2b951244,0xdc2822db40c0ac2f,0x183227397098d014}};
    //Fq_neg_test 3:
    FqElement pA3= {0xa1f0,0xc0000000,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult3= {0xa1f0,0xc0000000,{0x9a2f914ce07cfd47,0x0367766d2b951244,0xdc2822db40c0ac2f,0x183227397098d014}};

    //Fq_neg_test 4:
    FqElement pA4= {INT_MIN,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult4= {0x0,0x80000000,{0x80000000,0x0,0x0,0x0}};

    //Fq_neg_test 5:
    FqElement pA5= {INT_MAX,0x0,{0xa1f0fac9f8000000,0x9419f4243cdcb848,0xdc2822db40c0ac2e,0x183227397098d014}};
    FqElement pResult5= {INT_MIN+1, 0x0,{0x0,0x0,0x0,0x0}};

    FqElement Result0_c = {0,0,{0,0,0,0}};
    FqElement Result1_c = {0,0,{0,0,0,0}};
    FqElement Result2_c= {0,0,{0,0,0,0}};
    FqElement Result3_c= {0,0,{0,0,0,0}};
    FqElement Result4_c= {0,0,{0,0,0,0}};
    FqElement Result5_c= {0,0,{0,0,0,0}};

    Fq_neg(&Result0_c, &pA0);
    Fq_neg(&Result1_c, &pA1);
    Fq_neg(&Result2_c, &pA2);
    Fq_neg(&Result3_c, &pA3);
    Fq_neg(&Result4_c, &pA4);
    Fq_neg(&Result5_c, &pA5);

    compare_Result(&pResult0, &Result0_c,&pA0,&pA0, 0, "Fq_neg_unit_test");
    compare_Result(&pResult1, &Result1_c,&pA1,&pA1, 1, "Fq_neg_unit_test");
    compare_Result(&pResult2, &Result2_c,&pA2,&pA2, 2, "Fq_neg_unit_test");
    compare_Result(&pResult3, &Result3_c,&pA3,&pA3, 3, "Fq_neg_unit_test");
    compare_Result(&pResult4, &Result4_c,&pA4,&pA4, 4, "Fq_neg_unit_test");
    compare_Result(&pResult5, &Result5_c,&pA5,&pA5, 5, "Fq_neg_unit_test");
}

void print_results(void)
{
    printf("Results: %d tests were run, %d failed.\n", tests_run, tests_failed);
}

int main(void)
{
    Fq_Rw_mul_unit_test();
    Fq_Rw_mul1_unit_test();
    Fq_add_s1s2_unit_test();
    Fq_add_l1nl2n_unit_test();
    Fq_add_l1ml2n_unit_test();
    Fq_add_l1ml2m_unit_test();
    Fq_add_l1nl2m_unit_test();
    Fq_add_s1nl2m_unit_test();
    Fq_add_l1ms2n_unit_test();
    Fq_add_l1ms2m_unit_test();
    Fq_add_s1ml2m_unit_test();
    Fq_add_l1ns2_unit_test();
    Fq_add_s1l2n_unit_test();
    Fq_sub_s1s2_unit_test();
    Fq_sub_l1nl2n_unit_test();
    Fq_sub_l1ml2n_unit_test();
    Fq_sub_l1ml2m_unit_test();
    Fq_sub_l1nl2m_unit_test();
    Fq_sub_s1nl2m_unit_test();
    Fq_sub_l1ms2n_unit_test();
    Fq_sub_l1ms2m_unit_test();
    Fq_sub_s1ml2m_unit_test();
    Fq_sub_l1ns2_unit_test();
    Fq_sub_s1l2n_unit_test();
    Fq_mul_s1s2_unit_test();
    Fq_mul_l1nl2n_unit_test();
    Fq_mul_l1ml2n_unit_test();
    Fq_mul_l1ml2m_unit_test();
    Fq_mul_l1nl2m_unit_test();
    Fq_mul_l1ns2n_unit_test();
    Fq_mul_s1nl2n_unit_test();
    Fq_mul_s1nl2m_unit_test();
    Fq_mul_l1ms2n_unit_test();
    Fq_mul_l1ns2m_unit_test();
    Fq_mul_l1ms2m_unit_test();
    Fq_mul_s1ml2m_unit_test();
    Fq_mul_s1ml2n_unit_test();
    Fq_square_unit_test();
    Fq_rawCopyS2L_unit_test();
    Fq_Rw_copy_unit_test();
    Fq_Rw_sub_unit_test();
    Fq_Rw_Neg_unit_test();
    Fq_Rw_Msquare_unit_test();
    Fq_rawIsZero_unit_test();
    Fq_copy_unit_test();
    Fq_copyn_unit_test();
    Fq_neg_unit_test();
//    Fq_Rw_add_unit_test(); //not implemented

    print_results();

    return tests_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
