/**
 * test_prover_c.c - Unit tests for FQ field operations
 * Pure C implementation converted from C++
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "fq_c.h"  /* Assuming fq_c.h is the C header version of fq_c.hpp */

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

    /* Result variables for the actual computations */
    FqRawElement pRawResult0_c = {0};
    FqRawElement pRawResult1_c = {0};
    FqRawElement pRawResult2_c = {0};
    FqRawElement pRawResult3_c = {0};
    FqRawElement pRawResult4_c = {0};
    FqRawElement pRawResult5_c = {0};
    FqRawElement pRawResult8_c = {0};

    /* Perform the operations */
    Fq_rawMMul(pRawResult0_c, pRawA0, pRawB0);
    Fq_rawMMul(pRawResult1_c, pRawA1, pRawB1);
    Fq_rawMMul(pRawResult2_c, pRawA2, pRawB2);
    Fq_rawMMul(pRawResult3_c, pRawA3, pRawB3);
    Fq_rawMMul(pRawResult4_c, pRawA4, pRawB4);
    Fq_rawMMul(pRawResult5_c, pRawA5, pRawB5);
    Fq_rawMMul(pRawResult8_c, pRawA8, pRawB8);

    /* Compare expected and actual results */
    compare_raw_result(pRawResult0, pRawResult0_c, pRawA0, pRawB0, 0, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult1, pRawResult1_c, pRawA1, pRawB1, 1, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult2, pRawResult2_c, pRawA2, pRawB2, 2, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult3, pRawResult3_c, pRawA3, pRawB3, 3, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult4, pRawResult4_c, pRawA4, pRawB4, 4, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult5, pRawResult5_c, pRawA5, pRawB5, 5, "Fq_Rw_mul_unit_test");
    compare_raw_result(pRawResult8, pRawResult8_c, pRawA8, pRawB8, 8, "Fq_Rw_mul_unit_test");
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

    /* Result variables for the actual computations */
    FqRawElement pRawResult0_c = {0};
    FqRawElement pRawResult1_c = {0};
    FqRawElement pRawResult2_c = {0};
    FqRawElement pRawResult3_c = {0};
    FqRawElement pRawResult9_c = {0};

    /* Perform the operations */
    Fq_rawMMul1(pRawResult0_c, pRawA0, pRawB0[0]);
    Fq_rawMMul1(pRawResult1_c, pRawA1, pRawB1[0]);
    Fq_rawMMul1(pRawResult2_c, pRawA2, pRawB2[0]);
    Fq_rawMMul1(pRawResult3_c, pRawA3, pRawB3[0]);
    Fq_rawMMul1(pRawResult9_c, pRawA9, pRawB9[0]);

    /* Compare expected and actual results */
    compare_raw_result(pRawResult0, pRawResult0_c, pRawA0, pRawB0, 0, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult1, pRawResult1_c, pRawA1, pRawB1, 1, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult2, pRawResult2_c, pRawA2, pRawB2, 2, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult3, pRawResult3_c, pRawA3, pRawB3, 3, "Fq_Rw_mul1_unit_test");
    compare_raw_result(pRawResult9, pRawResult9_c, pRawA9, pRawB9, 9, "Fq_Rw_mul1_unit_test");
}

void print_results(void)
{
    printf("Results: %d tests were run, %d failed.\n", tests_run, tests_failed);
}

int main(void)
{
    Fq_Rw_mul_unit_test();
    Fq_Rw_mul1_unit_test();
    print_results();

    return tests_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
