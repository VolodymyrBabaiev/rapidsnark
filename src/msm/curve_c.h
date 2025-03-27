/**
 * curve.h - Elliptic Curve implementation in C
 * Converted from C++ template implementation
 */

#ifndef CURVE_H
#define CURVE_H

#include <stdint.h>
#include <stdbool.h>
#include "fq_c.h"  // For FqRawElement and field operations

// Enum for type of curve parameter A
typedef enum {
    A_IS_ZERO,
    A_IS_ONE,
    A_IS_NEGONE,
    A_IS_LONG
} TypeOfA;

// Point representation in projective coordinates
typedef struct {
    FqRawElement x;
    FqRawElement y;
    FqRawElement zz;    // z^2
    FqRawElement zzz;   // z^3
} CurvePoint;

// Point representation in affine coordinates
typedef struct {
    FqRawElement x;
    FqRawElement y;
} CurvePointAffine;

// Curve structure
typedef struct {
    // Field reference
    PFqElement field;
    
    // Curve parameters: y^2 = x^3 + a*x + b
    FqRawElement a;
    FqRawElement b;
    
    // Generator points
    CurvePoint one;
    CurvePointAffine oneAffine;
    
    // Points at infinity
    CurvePoint zero;
    CurvePointAffine zeroAffine;
    
    // Type of parameter A for optimization
    TypeOfA typeOfA;
    
#ifdef COUNT_OPS
    // Counters for operation tracking
    int cntAddMixed;
    int cntAdd;
    int cntAddAffine;
    int cntDbl;
    int cntEq;
    int cntEqMixed;    
    int cntDblMixed;
    int cntToAffine;
#endif // COUNT_OPS
} Curve;

// Constructor and initialization
Curve* curve_init(PFqElement field, FqRawElement* a, FqRawElement* b, FqRawElement* gx, FqRawElement* gy);
Curve* curve_init_str(PFqElement field, const char* a_str, const char* b_str, const char* gx_str, const char* gy_str);
void curve_init_params(Curve* curve, FqRawElement* a, FqRawElement* b, FqRawElement* gx, FqRawElement* gy);
void curve_free(Curve* curve);

// Helper functions
void curve_mul_by_a(Curve* curve, FqRawElement* r, FqRawElement* ab);

// Core point operations
void curve_add(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePoint* p2);
void curve_add_mixed(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePointAffine* p2);
void curve_add_affine(Curve* curve, CurvePoint* p3, CurvePointAffine* p1, CurvePointAffine* p2);
void curve_dbl(Curve* curve, CurvePoint* r, CurvePoint* a);
void curve_dbl_mixed(Curve* curve, CurvePoint* r, CurvePointAffine* a);
void curve_neg(Curve* curve, CurvePoint* r, CurvePoint* a);
void curve_neg_affine(Curve* curve, CurvePointAffine* r, CurvePointAffine* a);

// Subtraction operations
void curve_sub(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePoint* p2);
void curve_sub_mixed(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePointAffine* p2);
void curve_sub_affine(Curve* curve, CurvePoint* p3, CurvePointAffine* p1, CurvePointAffine* p2);

// Comparison operations
bool curve_eq(Curve* curve, CurvePoint* p1, CurvePoint* p2);
bool curve_eq_mixed(Curve* curve, CurvePoint* p1, CurvePointAffine* p2);
bool curve_eq_affine(Curve* curve, CurvePointAffine* p1, CurvePointAffine* p2);
bool curve_is_zero(Curve* curve, CurvePoint* p1);
bool curve_is_zero_affine(Curve* curve, CurvePointAffine* p1);

// Copy operations
void curve_copy(Curve* curve, CurvePoint* r, CurvePoint* a);
void curve_copy_affine_to_point(Curve* curve, CurvePoint* r, CurvePointAffine* a);
void curve_copy_point_to_affine(Curve* curve, CurvePointAffine* r, CurvePoint* a);
void curve_copy_affine(Curve* curve, CurvePointAffine* r, CurvePointAffine* a);

// String operations
char* curve_point_to_string(Curve* curve, CurvePoint* p, uint32_t radix);

// Scalar multiplication
void curve_mul_by_scalar(Curve* curve, CurvePoint* r, CurvePoint* p, uint8_t* scalar, unsigned int scalarSize);
void curve_mul_affine_by_scalar(Curve* curve, CurvePoint* r, CurvePointAffine* p, uint8_t* scalar, unsigned int scalarSize);

// Multi-scalar multiplication
void curve_multi_mul_by_scalar(Curve* curve, CurvePoint* r, CurvePointAffine* bases, 
                               uint8_t* scalars, unsigned int scalarSize, 
                               unsigned int n, unsigned int nThreads);

void curve_multi_mul_by_scalar_with_indices(Curve* curve, CurvePoint* r, CurvePointAffine* bases, 
                                           uint8_t* scalars, unsigned int scalarSize, 
                                           unsigned int n, uint32_t nx, uint64_t* x, 
                                           unsigned int nThreads);

#ifdef COUNT_OPS
void curve_reset_counters(Curve* curve);
void curve_print_counters(Curve* curve);
#endif // COUNT_OPS

#endif // CURVE_H
