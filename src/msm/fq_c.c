/**
 * fq.c - Field element operations for the Fq field
 * C version converted from C++ implementation
 */

#include "fq_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>

// Global field elements
FqElement Fq_q  = {0, 0x80000000, {0x3c208c16d87cfd47,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029}};
FqElement Fq_R2 = {0, 0x80000000, {0xf32cfc5b538afa89,0xb5e71911d44501fb,0x47ab1eff0a417ff6,0x06d89f71cab8351f}};
FqElement Fq_R3 = {0, 0x80000000, {0xb1cd6dafda1530df,0x62f210e6a7283db6,0xef7f0b0c0ada0afb,0x20fd6e902d592544}};

// Static variables
static mpz_t q;
static mpz_t zero;
static mpz_t one;
static mpz_t mask;
static size_t nBits;
static bool initialized = false;
FqRawElement half = {0x9e10460b6c3e7ea3,0xcbc0b548b438e546,0xdc2822db40c0ac2e,0x183227397098d014};
FqRawElement Fq_rawq = {0x3c208c16d87cfd47,0x97816a916871ca8d,0xb85045b68181585d,0x30644e72e131a029};
FqRawElement Fq_rawR3 = {0xb1cd6dafda1530df,0x62f210e6a7283db6,0xef7f0b0c0ada0afb,0x20fd6e902d592544};
static const uint64_t Fq_np = 0x87d20782e4866389;
static const uint64_t lboMask = 0x3fffffffffffffff;

// Conversion to/from MPZ
//void Fq_toMpz(mpz_t r, PFqElement pE) {
//    FqElement tmp;
//    Fq_toNormal(&tmp, pE);
//    if (!(tmp.type & Fq_LONG)) {
//        mpz_set_si(r, tmp.shortVal);
//        if (tmp.shortVal < 0) {
//            mpz_add(r, r, q);
//        }
//    } else {
//        mpz_import(r, Fq_N64, -1, 8, -1, 0, (const void *)tmp.longVal);
//    }
//}
//
//void Fq_fromMpz(PFqElement pE, mpz_t v) {
//    if (mpz_fits_sint_p(v)) {
//        pE->type = Fq_SHORT;
//        pE->shortVal = mpz_get_si(v);
//    } else {
//        pE->type = Fq_LONG;
//        for (int i = 0; i < Fq_N64; i++) pE->longVal[i] = 0;
//        mpz_export((void *)(pE->longVal), NULL, -1, 8, -1, 0, v);
//    }
//}

// Field initialization
//bool Fq_init() {
//    if (initialized) return false;
//    initialized = true;
//    mpz_init(q);
//    mpz_import(q, Fq_N64, -1, 8, -1, 0, (const void *)Fq_q.longVal);
//    mpz_init_set_ui(zero, 0);
//    mpz_init_set_ui(one, 1);
//    nBits = mpz_sizeinbase(q, 2);
//    mpz_init(mask);
//    mpz_mul_2exp(mask, one, nBits);
//    mpz_sub(mask, mask, one);
//    return true;
//}

// String to element conversion
//void Fq_str2element(PFqElement pE, const char *s, unsigned int base) {
//    mpz_t mr;
//    mpz_init_set_str(mr, s, base);
//    mpz_fdiv_r(mr, mr, q);
//    Fq_fromMpz(pE, mr);
//    mpz_clear(mr);
//}

// Element to string conversion
//char *Fq_element2str(PFqElement pE) {
//    FqElement tmp;
//    mpz_t r;
//    if (!(pE->type & Fq_LONG)) {
//        if (pE->shortVal >= 0) {
//            char *r = (char*)malloc(32);
//            sprintf(r, "%d", pE->shortVal);
//            return r;
//        } else {
//            mpz_init_set_si(r, pE->shortVal);
//            mpz_add(r, r, q);
//        }
//    } else {
//        Fq_toNormal(&tmp, pE);
//        mpz_init(r);
//        mpz_import(r, Fq_N64, -1, 8, -1, 0, (const void *)tmp.longVal);
//    }
//    char *res = mpz_get_str(0, 10, r);
//    mpz_clear(r);
//    return res;
//}

// Integer division
//void Fq_idiv(PFqElement r, PFqElement a, PFqElement b) {
//    mpz_t ma;
//    mpz_t mb;
//    mpz_t mr;
//    mpz_init(ma);
//    mpz_init(mb);
//    mpz_init(mr);
//
//    Fq_toMpz(ma, a);
//    Fq_toMpz(mb, b);
//    mpz_fdiv_q(mr, ma, mb);
//    Fq_fromMpz(r, mr);
//
//    mpz_clear(ma);
//    mpz_clear(mb);
//    mpz_clear(mr);
//}

// Modular division
//void Fq_mod(PFqElement r, PFqElement a, PFqElement b) {
//    mpz_t ma;
//    mpz_t mb;
//    mpz_t mr;
//    mpz_init(ma);
//    mpz_init(mb);
//    mpz_init(mr);
//
//    Fq_toMpz(ma, a);
//    Fq_toMpz(mb, b);
//    mpz_fdiv_r(mr, ma, mb);
//    Fq_fromMpz(r, mr);
//
//    mpz_clear(ma);
//    mpz_clear(mb);
//    mpz_clear(mr);
//}

// Power operation
//void Fq_pow(PFqElement r, PFqElement a, PFqElement b) {
//    mpz_t ma;
//    mpz_t mb;
//    mpz_t mr;
//    mpz_init(ma);
//    mpz_init(mb);
//    mpz_init(mr);
//
//    Fq_toMpz(ma, a);
//    Fq_toMpz(mb, b);
//    mpz_powm(mr, ma, mb, q);
//    Fq_fromMpz(r, mr);
//
//    mpz_clear(ma);
//    mpz_clear(mb);
//    mpz_clear(mr);
//}

// Inverse operation
//void Fq_inv(PFqElement r, PFqElement a) {
//    mpz_t ma;
//    mpz_t mr;
//    mpz_init(ma);
//    mpz_init(mr);
//
//    Fq_toMpz(ma, a);
//    mpz_invert(mr, ma, q);
//    Fq_fromMpz(r, mr);
//    mpz_clear(ma);
//    mpz_clear(mr);
//}

// Division operation
//void Fq_div(PFqElement r, PFqElement a, PFqElement b) {
//    FqElement tmp;
//    Fq_inv(&tmp, b);
//    Fq_mul(r, a, &tmp);
//}

// Error handlers
//void Fq_fail() {
//    fprintf(stderr, "Fq error\n");
//    exit(1);
//}

//void Fq_longErr() {
//    Fq_fail();
//}

// Copy an element
void Fq_copy(PFqElement r, PFqElement a) {
    *r = *a;
}

// Convert to normal form
//void Fq_toNormal(PFqElement r, PFqElement a) {
//    if (a->type == Fq_LONGMONTGOMERY) {
//        r->type = Fq_LONG;
//        Fq_rawFromMontgomery(r->longVal, a->longVal);
//    } else {
//        Fq_copy(r, a);
//    }
//}

// Utility function to check for integer overflow
//static inline int has_mul32_overflow(int64_t val) {
//    int64_t sign = val >> 31;
//    if (sign) {
//        sign = ~sign;
//    }
//    return sign ? 1 : 0;
//}

// Raw signed multiplication
//static inline int Fq_rawSMul(int64_t *r, int32_t a, int32_t b) {
//    *r = (int64_t)a * b;
//    return has_mul32_overflow(*r);
//}

// Multiplication of short integers
//static inline void mul_s1s2(PFqElement r, PFqElement a, PFqElement b) {
//    int64_t result;
//    int overflow = Fq_rawSMul(&result, a->shortVal, b->shortVal);
//
//    if (overflow) {
//        Fq_rawCopyS2L(r->longVal, result);
//        r->type = Fq_LONG;
//        r->shortVal = 0;
//    } else {
//        // done the same way as in intel asm implementation
//        r->shortVal = (int32_t)result;
//        r->type = Fq_SHORT;
//        //
//
//        Fq_rawCopyS2L(r->longVal, result);
//        r->type = Fq_LONG;
//        r->shortVal = 0;
//    }
//}

// Multiplication functions for different element types
//static inline void mul_l1nl2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONGMONTGOMERY;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//    Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
//}

//static inline void mul_l1nl2m(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

//static inline void mul_l1ml2m(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONGMONTGOMERY;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

//static inline void mul_l1ml2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

//static inline void mul_l1ns2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONGMONTGOMERY;
//
//    if (b->shortVal < 0) {
//        int64_t b_shortVal = b->shortVal;
//        Fq_rawMMul1(r->longVal, a->longVal, -b_shortVal);
//        Fq_rawNeg(r->longVal, r->longVal);
//    } else {
//        Fq_rawMMul1(r->longVal, a->longVal, b->shortVal);
//    }
//
//    Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
//}

//static inline void mul_s1nl2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONGMONTGOMERY;
//
//    if (a->shortVal < 0) {
//        int64_t a_shortVal = a->shortVal;
//        Fq_rawMMul1(r->longVal, b->longVal, -a_shortVal);
//        Fq_rawNeg(r->longVal, r->longVal);
//    } else {
//        Fq_rawMMul1(r->longVal, b->longVal, a->shortVal);
//    }
//
//    Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
//}

//static inline void mul_l1ms2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//
//    if (b->shortVal < 0) {
//        int64_t b_shortVal = b->shortVal;
//        Fq_rawMMul1(r->longVal, a->longVal, -b_shortVal);
//        Fq_rawNeg(r->longVal, r->longVal);
//    } else {
//        Fq_rawMMul1(r->longVal, a->longVal, b->shortVal);
//    }
//}

//static inline void mul_s1nl2m(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//
//    if (a->shortVal < 0) {
//        int64_t a_shortVal = a->shortVal;
//        Fq_rawMMul1(r->longVal, b->longVal, -a_shortVal);
//        Fq_rawNeg(r->longVal, r->longVal);
//    } else {
//        Fq_rawMMul1(r->longVal, b->longVal, a->shortVal);
//    }
//}

//static inline void mul_l1ns2m(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

//static inline void mul_l1ms2m(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONGMONTGOMERY;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

//static inline void mul_s1ml2m(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONGMONTGOMERY;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

//static inline void mul_s1ml2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
//}

// Multiplication
/*
void Fq_mul(PFqElement r, PFqElement a, PFqElement b) {
    if (a->type & Fq_LONG) {
        if (b->type & Fq_LONG) {
            if (a->type & Fq_MONTGOMERY) {
                if (b->type & Fq_MONTGOMERY) {
                    mul_l1ml2m(r, a, b);
                } else {
                    mul_l1ml2n(r, a, b);
                }
            } else {
                if (b->type & Fq_MONTGOMERY) {
                    mul_l1nl2m(r, a, b);
                } else {
                    mul_l1nl2n(r, a, b);
                }
            }
        } else if (a->type & Fq_MONTGOMERY) {
            if (b->type & Fq_MONTGOMERY) {
                mul_l1ms2m(r, a, b);
            } else {
                mul_l1ms2n(r, a, b);
            }
        } else {
            if (b->type & Fq_MONTGOMERY) {
                mul_l1ns2m(r, a, b);
            } else {
                mul_l1ns2n(r, a, b);
            }
        }
    } else if (b->type & Fq_LONG) {
        if (a->type & Fq_MONTGOMERY) {
            if (b->type & Fq_MONTGOMERY) {
                mul_s1ml2m(r, a, b);
            } else {
                mul_s1ml2n(r, a, b);
            }
        } else if (b->type & Fq_MONTGOMERY) {
            mul_s1nl2m(r, a, b);
        } else {
            mul_s1nl2n(r, a, b);
        }
    } else {
        mul_s1s2(r, a, b);
    }
}
 */

// Convert to long normal form
//void Fq_toLongNormal(PFqElement r, PFqElement a) {
//    if (a->type & Fq_LONG) {
//        if (a->type & Fq_MONTGOMERY) {
//            Fq_rawFromMontgomery(r->longVal, a->longVal);
//            r->type = Fq_LONG;
//        } else {
//            Fq_copy(r, a);
//        }
//    } else {
//        Fq_rawCopyS2L(r->longVal, a->shortVal);
//        r->type = Fq_LONG;
//        r->shortVal = 0;
//    }
//}

// Convert to Montgomery form
void Fq_toMontgomery(PFqElement r, PFqElement a) {
    if (a->type & Fq_MONTGOMERY) {
        Fq_copy(r, a);
    } else if (a->type & Fq_LONG) {
        r->shortVal = a->shortVal;
        Fq_rawMMul(r->longVal, a->longVal, Fq_R2.longVal);
        r->type = Fq_LONGMONTGOMERY;
    } else if (a->shortVal < 0) {
        int64_t a_shortVal = a->shortVal;
        Fq_rawMMul1(r->longVal, Fq_R2.longVal, -a_shortVal);
        Fq_rawNeg(r->longVal, r->longVal);
        r->type = Fq_SHORTMONTGOMERY;
    } else {
        Fq_rawMMul1(r->longVal, Fq_R2.longVal, a->shortVal);
        r->type = Fq_SHORTMONTGOMERY;
    }
}

// Copy n elements
void Fq_copyn(PFqElement r, PFqElement a, int n) {
    memcpy(r, a, n * sizeof(FqElement));
}

// Check for addition overflow
//static inline int has_add32_overflow(int64_t val) {
//    int64_t signs = (val >> 31) & 0x3;
//    return signs == 1 || signs == 2;
//}

// Signed subtraction
//static inline int Fq_rawSSub(int64_t *r, int32_t a, int32_t b) {
//    *r = (int64_t)a - b;
//    return has_add32_overflow(*r);
//}

// Subtraction of short elements
//static inline void sub_s1s2(PFqElement r, PFqElement a, PFqElement b) {
//    int64_t diff;
//    int overflow = Fq_rawSSub(&diff, a->shortVal, b->shortVal);
//
//    if (overflow) {
//        Fq_rawCopyS2L(r->longVal, diff);
//        r->type = Fq_LONG;
//        r->shortVal = 0;
//    } else {
//        r->type = Fq_SHORT;
//        r->shortVal = (int32_t)diff;
//    }
//}

// Different subtraction implementations based on element types
//static inline void sub_l1nl2n(PFqElement r, PFqElement a, PFqElement b) {
//    r->type = Fq_LONG;
//    Fq_rawSub(r->longVal, a->longVal, b->longVal);
//}

/**
 * Addition operation for field elements
 */
void Fq_add(PFqElement r, PFqElement a, PFqElement b)
{
    if (a->type & Fq_LONG) {
        if (b->type & Fq_LONG) {
            if (a->type & Fq_MONTGOMERY) {
                if (b->type & Fq_MONTGOMERY) {
                    // Montgomery + Montgomery
                    r->type = Fq_LONGMONTGOMERY;
                    Fq_rawAdd(r->longVal, a->longVal, b->longVal);
                } else {
                    // Montgomery + Normal
                    r->type = Fq_LONGMONTGOMERY;

                    // Convert b to Montgomery form
                    FqElement b_m;
                    Fq_toMontgomery(&b_m, b);

                    Fq_rawAdd(r->longVal, a->longVal, b_m.longVal);
                }
            } else {
                if (b->type & Fq_MONTGOMERY) {
                    // Normal + Montgomery
                    r->type = Fq_LONGMONTGOMERY;

                    // Convert a to Montgomery form
                    FqElement a_m;
                    Fq_toMontgomery(&a_m, a);

                    Fq_rawAdd(r->longVal, a_m.longVal, b->longVal);
                } else {
                    // Normal + Normal
                    r->type = Fq_LONG;
                    Fq_rawAdd(r->longVal, a->longVal, b->longVal);
                }
            }
        } else if (a->type & Fq_MONTGOMERY) {
            // Long Montgomery + Short
            if (b->type & Fq_MONTGOMERY) {
                // Montgomery + Montgomery
                r->type = Fq_LONGMONTGOMERY;
                Fq_rawAdd(r->longVal, a->longVal, b->longVal);
            } else {
                // Montgomery + Normal
                r->type = Fq_LONGMONTGOMERY;

                // Convert b to Montgomery form
                FqElement b_m;
                Fq_toMontgomery(&b_m, b);

                Fq_rawAdd(r->longVal, a->longVal, b_m.longVal);
            }
        } else {
            // Long Normal + Short
            r->type = Fq_LONG;

            if (b->shortVal >= 0) {
                Fq_rawAddLS(r->longVal, a->longVal, (uint64_t)b->shortVal);
            } else {
                int64_t b_shortVal = b->shortVal;
                Fq_rawSubLS(r->longVal, a->longVal, (uint64_t)(-b_shortVal));
            }
        }
    } else if (b->type & Fq_LONG) {
        // Short + Long (commutative, reuse the Long + Short code)
        Fq_add(r, b, a);
    } else {
        // Short + Short
        int64_t sum;
        int32_t a_shortVal = a->shortVal;
        int32_t b_shortVal = b->shortVal;

        sum = (int64_t)a_shortVal + b_shortVal;

        // Check if the result fits in a short value
        int64_t signs = (sum >> 31) & 0x3;
        int overflow = (signs == 1 || signs == 2);

        if (overflow) {
            Fq_rawCopyS2L(r->longVal, sum);
            r->type = Fq_LONG;
            r->shortVal = 0;
        } else {
            r->type = Fq_SHORT;
            r->shortVal = (int32_t)sum;
        }
    }
}

/**
 * Subtraction operation for field elements
 */
void Fq_sub(PFqElement r, PFqElement a, PFqElement b)
{
    if (a->type & Fq_LONG) {
        if (b->type & Fq_LONG) {
            if (a->type & Fq_MONTGOMERY) {
                if (b->type & Fq_MONTGOMERY) {
                    // Montgomery - Montgomery
                    r->type = Fq_LONGMONTGOMERY;
                    Fq_rawSub(r->longVal, a->longVal, b->longVal);
                } else {
                    // Montgomery - Normal
                    r->type = Fq_LONGMONTGOMERY;

                    // Convert b to Montgomery
                    FqElement b_m;
                    Fq_toMontgomery(&b_m, b);

                    Fq_rawSub(r->longVal, a->longVal, b_m.longVal);
                }
            } else if (b->type & Fq_MONTGOMERY) {
                // Normal - Montgomery
                r->type = Fq_LONGMONTGOMERY;

                // Convert a to Montgomery
                FqElement a_m;
                Fq_toMontgomery(&a_m, a);

                Fq_rawSub(r->longVal, a_m.longVal, b->longVal);
            } else {
                // Normal - Normal
                r->type = Fq_LONG;
                Fq_rawSub(r->longVal, a->longVal, b->longVal);
            }
        } else if (a->type & Fq_MONTGOMERY) {
            // Long Montgomery - Short
            if (b->type & Fq_MONTGOMERY) {
                // Montgomery - Montgomery
                r->type = Fq_LONGMONTGOMERY;
                Fq_rawSub(r->longVal, a->longVal, b->longVal);
            } else {
                // Montgomery - Normal
                r->type = Fq_LONGMONTGOMERY;

                // Convert b to Montgomery
                FqElement b_m;
                Fq_toMontgomery(&b_m, b);

                Fq_rawSub(r->longVal, a->longVal, b_m.longVal);
            }
        } else {
            // Long Normal - Short
            r->type = Fq_LONG;

            if (b->shortVal < 0) {
                int64_t b_shortVal = b->shortVal;
                Fq_rawAddLS(r->longVal, a->longVal, (uint64_t)(-b_shortVal));
            } else {
                Fq_rawSubLS(r->longVal, a->longVal, (uint64_t)b->shortVal);
            }
        }
    } else if (b->type & Fq_LONG) {
        // Short - Long
        if (b->type & Fq_MONTGOMERY) {
            if (a->type & Fq_MONTGOMERY) {
                // Montgomery - Montgomery
                r->type = Fq_LONGMONTGOMERY;
                Fq_rawSub(r->longVal, a->longVal, b->longVal);
            } else {
                // Normal - Montgomery
                r->type = Fq_LONGMONTGOMERY;

                // Convert a to Montgomery
                FqElement a_m;
                Fq_toMontgomery(&a_m, a);

                Fq_rawSub(r->longVal, a_m.longVal, b->longVal);
            }
        } else {
            // Short - Long Normal
            r->type = Fq_LONG;

            if (a->shortVal >= 0) {
                Fq_rawSubSL(r->longVal, (uint64_t)a->shortVal, b->longVal);
            } else {
                int64_t a_shortVal = a->shortVal;
                Fq_rawNegLS(r->longVal, b->longVal, (uint64_t)(-a_shortVal));
            }
        }
    } else {
        // Short - Short
        int64_t diff;
        int32_t a_shortVal = a->shortVal;
        int32_t b_shortVal = b->shortVal;

        diff = (int64_t)a_shortVal - b_shortVal;

        // Check if the result fits in a short value
        int64_t signs = (diff >> 31) & 0x3;
        int overflow = (signs == 1 || signs == 2);

        if (overflow) {
            Fq_rawCopyS2L(r->longVal, diff);
            r->type = Fq_LONG;
            r->shortVal = 0;
        } else {
            r->type = Fq_SHORT;
            r->shortVal = (int32_t)diff;
        }
    }
}

/**
 * Negation operation for field elements
 */
void Fq_neg(PFqElement r, PFqElement a)
{
    if (a->type & Fq_LONG) {
        r->type = a->type;
        r->shortVal = a->shortVal;
        Fq_rawNeg(r->longVal, a->longVal);
    } else {
        int64_t a_shortVal;

        a_shortVal = -(int64_t)a->shortVal;

        // Check if the result fits in a short value
        int64_t signs = (a_shortVal >> 31) & 0x3;
        int overflow = (signs == 1 || signs == 2);

        if (overflow) {
            Fq_rawCopyS2L(r->longVal, a_shortVal);
            r->type = Fq_LONG;
            r->shortVal = 0;
        } else {
            r->type = Fq_SHORT;
            r->shortVal = (int32_t)a_shortVal;
        }
    }
}

/**
 * Multiplication operation for field elements
 */
void Fq_mul(PFqElement r, PFqElement a, PFqElement b)
{
    if (a->type & Fq_LONG) {
        if (b->type & Fq_LONG) {
            if (a->type & Fq_MONTGOMERY) {
                if (b->type & Fq_MONTGOMERY) {
                    // Montgomery * Montgomery
                    r->type = Fq_LONGMONTGOMERY;
                    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
                } else {
                    // Montgomery * Normal
                    r->type = Fq_LONG;
                    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
                }
            } else {
                if (b->type & Fq_MONTGOMERY) {
                    // Normal * Montgomery
                    r->type = Fq_LONG;
                    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
                } else {
                    // Normal * Normal
                    r->type = Fq_LONGMONTGOMERY;
                    Fq_rawMMul(r->longVal, a->longVal, b->longVal);
                    Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
                }
            }
        } else if (a->type & Fq_MONTGOMERY) {
            // Long Montgomery * Short
            if (b->type & Fq_MONTGOMERY) {
                // Montgomery * Montgomery
                r->type = Fq_LONGMONTGOMERY;
                Fq_rawMMul(r->longVal, a->longVal, b->longVal);
            } else {
                // Montgomery * Normal
                r->type = Fq_LONG;

                if (b->shortVal < 0) {
                    int64_t b_shortVal = b->shortVal;
                    Fq_rawMMul1(r->longVal, a->longVal, (uint64_t)(-b_shortVal));
                    Fq_rawNeg(r->longVal, r->longVal);
                } else {
                    Fq_rawMMul1(r->longVal, a->longVal, (uint64_t)b->shortVal);
                }
            }
        } else {
            // Long Normal * Short
            if (b->type & Fq_MONTGOMERY) {
                // Normal * Montgomery
                r->type = Fq_LONG;
                Fq_rawMMul(r->longVal, a->longVal, b->longVal);
            } else {
                // Normal * Normal
                r->type = Fq_LONGMONTGOMERY;

                if (b->shortVal < 0) {
                    int64_t b_shortVal = b->shortVal;
                    Fq_rawMMul1(r->longVal, a->longVal, (uint64_t)(-b_shortVal));
                    Fq_rawNeg(r->longVal, r->longVal);
                } else {
                    Fq_rawMMul1(r->longVal, a->longVal, (uint64_t)b->shortVal);
                }

                Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
            }
        }
    } else if (b->type & Fq_LONG) {
        // Short * Long
        if (a->type & Fq_MONTGOMERY) {
            if (b->type & Fq_MONTGOMERY) {
                // Montgomery * Montgomery
                r->type = Fq_LONGMONTGOMERY;
                Fq_rawMMul(r->longVal, a->longVal, b->longVal);
            } else {
                // Montgomery * Normal
                r->type = Fq_LONG;
                Fq_rawMMul(r->longVal, a->longVal, b->longVal);
            }
        } else if (b->type & Fq_MONTGOMERY) {
            // Normal * Montgomery
            r->type = Fq_LONG;

            if (a->shortVal < 0) {
                int64_t a_shortVal = a->shortVal;
                Fq_rawMMul1(r->longVal, b->longVal, (uint64_t)(-a_shortVal));
                Fq_rawNeg(r->longVal, r->longVal);
            } else {
                Fq_rawMMul1(r->longVal, b->longVal, (uint64_t)a->shortVal);
            }
        } else {
            // Normal * Normal
            r->type = Fq_LONGMONTGOMERY;

            if (a->shortVal < 0) {
                int64_t a_shortVal = a->shortVal;
                Fq_rawMMul1(r->longVal, b->longVal, (uint64_t)(-a_shortVal));
                Fq_rawNeg(r->longVal, r->longVal);
            } else {
                Fq_rawMMul1(r->longVal, b->longVal, (uint64_t)a->shortVal);
            }

            Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
        }
    } else {
        // Short * Short
        int64_t result;
        int32_t a_shortVal = a->shortVal;
        int32_t b_shortVal = b->shortVal;

        result = (int64_t)a_shortVal * b_shortVal;

        // Check for overflow
        int64_t a_sign = a_shortVal >> 31;
        if (a_sign) {
            a_sign = ~a_sign;
        }
        int overflow = a_sign ? 1 : 0;

        if (overflow) {
            Fq_rawCopyS2L(r->longVal, result);
            r->type = Fq_LONG;
            r->shortVal = 0;
        } else {
            // Following the Intel ASM implementation
            r->shortVal = (int32_t)result;
            r->type = Fq_SHORT;

            Fq_rawCopyS2L(r->longVal, result);
            r->type = Fq_LONG;
            r->shortVal = 0;
        }
    }
}

/**
 * Square operation for field elements
 */
void Fq_square(PFqElement r, PFqElement a)
{
    if (a->type & Fq_LONG) {
        if (a->type & Fq_MONTGOMERY) {
            // Montgomery form
            r->type = Fq_LONGMONTGOMERY;
            Fq_rawMSquare(r->longVal, a->longVal);
        } else {
            // Normal form
            r->type = Fq_LONGMONTGOMERY;
            Fq_rawMSquare(r->longVal, a->longVal);
            Fq_rawMMul(r->longVal, r->longVal, Fq_R3.longVal);
        }
    } else {
        // Short form
        int64_t result;
        int32_t a_shortVal = a->shortVal;

        result = (int64_t)a_shortVal * a_shortVal;

        // Check for overflow
        int64_t a_sign = a_shortVal >> 31;
        if (a_sign) {
            a_sign = ~a_sign;
        }
        int overflow = a_sign ? 1 : 0;

        if (overflow) {
            Fq_rawCopyS2L(r->longVal, result);
            r->type = Fq_LONG;
            r->shortVal = 0;
        } else {
            // Following the Intel ASM implementation
            r->shortVal = (int32_t)result;
            r->type = Fq_SHORT;

            Fq_rawCopyS2L(r->longVal, result);
            r->type = Fq_LONG;
            r->shortVal = 0;
        }
    }
}

/**
 * Check if an element is zero
 */
int Fq_isZero(const PFqElement a)
{
    if (a->type & Fq_LONG) {
        return Fq_rawIsZero(a->longVal);
    } else {
        return a->shortVal == 0;
    }
}

/**
 * Copy an element
 */
//void Fq_copy(PFqElement r, PFqElement a)
//{
//    *r = *a;
//}

