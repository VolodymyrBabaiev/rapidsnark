/**
 * curve.c - Elliptic Curve implementation in C
 * Converted from C++ template implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curve_c.h"
// #include "naf.h" // For Non-Adjacent Form scalar multiplication
// #include "msm.h"  // For Multi-Scalar Multiplication
// #include "multiexp.h" // For ParallelMultiexp

/* ========== Constructor and Initialization ========== */

Curve* curve_init(PFqElement field, FqRawElement* aa, FqRawElement* ab, FqRawElement* agx, FqRawElement* agy) {
    Curve* curve = (Curve*)malloc(sizeof(Curve));
    if (!curve) return NULL;
    
    curve->field = field;
    curve_init_params(curve, aa, ab, agx, agy);
    
    return curve;
}

Curve* curve_init_str(PFqElement field, const char* a_str, const char* b_str, const char* gx_str, const char* gy_str) {
    FqElement aa, ab, agx, agy;
    FqRawElement a_raw, b_raw, gx_raw, gy_raw;
    
    Fq_str2element(&aa, a_str, 10);
    Fq_str2element(&ab, b_str, 10);
    Fq_str2element(&agx, gx_str, 10);
    Fq_str2element(&agy, gy_str, 10);
    
    // Convert to raw elements
    Fq_toNormal(&aa, &aa);
    Fq_toNormal(&ab, &ab);
    Fq_toNormal(&agx, &agx);
    Fq_toNormal(&agy, &agy);
    
    if (aa.type & Fq_LONG) {
        Fq_rawCopy(a_raw, aa.longVal);
    } else {
        Fq_rawCopyS2L(a_raw, aa.shortVal);
    }
    
    if (ab.type & Fq_LONG) {
        Fq_rawCopy(b_raw, ab.longVal);
    } else {
        Fq_rawCopyS2L(b_raw, ab.shortVal);
    }
    
    if (agx.type & Fq_LONG) {
        Fq_rawCopy(gx_raw, agx.longVal);
    } else {
        Fq_rawCopyS2L(gx_raw, agx.shortVal);
    }
    
    if (agy.type & Fq_LONG) {
        Fq_rawCopy(gy_raw, agy.longVal);
    } else {
        Fq_rawCopyS2L(gy_raw, agy.shortVal);
    }
    
    return curve_init(field, &a_raw, &b_raw, &gx_raw, &gy_raw);
}

void curve_free(Curve* curve) {
    if (curve) {
        free(curve);
    }
}

void curve_init_params(Curve* curve, FqRawElement* aa, FqRawElement* ab, FqRawElement* agx, FqRawElement* agy) {
    FqElement zero_elem, one_elem, neg_one_elem;
    FqRawElement zero_raw, one_raw, neg_one_raw;
    
    // Get field constants
    Fq_set_zero(&zero_elem);
    Fq_set_one(&one_elem);
    Fq_set_neg_one(&neg_one_elem);
    
    Fq_toNormal(&zero_elem, &zero_elem);
    Fq_toNormal(&one_elem, &one_elem);
    Fq_toNormal(&neg_one_elem, &neg_one_elem);
    
    if (zero_elem.type & Fq_LONG) {
        Fq_rawCopy(zero_raw, zero_elem.longVal);
    } else {
        Fq_rawCopyS2L(zero_raw, zero_elem.shortVal);
    }
    
    if (one_elem.type & Fq_LONG) {
        Fq_rawCopy(one_raw, one_elem.longVal);
    } else {
        Fq_rawCopyS2L(one_raw, one_elem.shortVal);
    }
    
    if (neg_one_elem.type & Fq_LONG) {
        Fq_rawCopy(neg_one_raw, neg_one_elem.longVal);
    } else {
        Fq_rawCopyS2L(neg_one_raw, neg_one_elem.shortVal);
    }
    
    // Copy curve parameters
    Fq_rawCopy(curve->a, *aa);
    Fq_rawCopy(curve->b, *ab);
    
    // Initialize generator point (one) in projective coordinates
    Fq_rawCopy(curve->one.x, *agx);
    Fq_rawCopy(curve->one.y, *agy);
    Fq_rawCopy(curve->one.zz, one_raw);
    Fq_rawCopy(curve->one.zzz, one_raw);
    
    // Initialize generator point in affine coordinates
    Fq_rawCopy(curve->oneAffine.x, *agx);
    Fq_rawCopy(curve->oneAffine.y, *agy);
    
    // Initialize point at infinity in projective coordinates
    Fq_rawCopy(curve->zero.x, one_raw);
    Fq_rawCopy(curve->zero.y, one_raw);
    Fq_rawCopy(curve->zero.zz, zero_raw);
    Fq_rawCopy(curve->zero.zzz, zero_raw);
    
    // Initialize point at infinity in affine coordinates
    Fq_rawCopy(curve->zeroAffine.x, zero_raw);
    Fq_rawCopy(curve->zeroAffine.y, zero_raw);
    
    // Determine the type of A for optimization
    if (Fq_rawIsZero(*aa)) {
        curve->typeOfA = A_IS_ZERO;
    } else if (Fq_rawIsEq(*aa, one_raw)) {
        curve->typeOfA = A_IS_ONE;
    } else if (Fq_rawIsEq(*aa, neg_one_raw)) {
        curve->typeOfA = A_IS_NEGONE;
    } else {
        curve->typeOfA = A_IS_LONG;
    }
    
#ifdef COUNT_OPS
    curve_reset_counters(curve);
#endif
}

/* ========== Helper Functions ========== */

void curve_mul_by_a(Curve* curve, FqRawElement* r, FqRawElement* ab) {
    switch (curve->typeOfA) {
        case A_IS_ZERO:
            Fq_rawCopy(*r, curve->zeroAffine.x); // Field's zero
            break;
        case A_IS_ONE:
            Fq_rawCopy(*r, *ab);
            break;
        case A_IS_NEGONE:
            Fq_rawNeg(*r, *ab);
            break;
        case A_IS_LONG:
            Fq_rawMMul(*r, curve->a, *ab);
            break;
    }
}

/* ========== Core Point Operations ========== */

void curve_add(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePoint* p2) {
#ifdef COUNT_OPS
    curve->cntAdd++;
#endif
    
    // Check for special cases
    if (curve_is_zero(curve, p1)) {
        curve_copy(curve, p3, p2);
        return;
    }
    
    if (curve_is_zero(curve, p2)) {
        curve_copy(curve, p3, p1);
        return;
    }
    
    // Temporary variables
    FqRawElement u1, u2, s1, s2, p, r, pp, ppp, q, tmp;
    
    // U1 = X1*ZZ2
    Fq_rawMMul(u1, p1->x, p2->zz);
    
    // U2 = X2*ZZ1
    Fq_rawMMul(u2, p2->x, p1->zz);
    
    // S1 = Y1*ZZZ2
    Fq_rawMMul(s1, p1->y, p2->zzz);
    
    // S2 = Y2*ZZZ1
    Fq_rawMMul(s2, p2->y, p1->zzz);
    
    // P = U2-U1
    Fq_rawSub(p, u2, u1);
    
    // R = S2-S1
    Fq_rawSub(r, s2, s1);
    
    // Handle doubling case
    if (Fq_rawIsZero(p) && Fq_rawIsZero(r)) {
        curve_dbl(curve, p3, p1);
        return;
    }
    
    // PP = P^2
    Fq_rawMSquare(pp, p);
    
    // PPP = P*PP
    Fq_rawMMul(ppp, p, pp);
    
    // Q = U1*PP
    Fq_rawMMul(q, u1, pp);
    
    // X3 = R^2-PPP-2*Q
    Fq_rawMSquare(p3->x, r);
    Fq_rawSub(p3->x, p3->x, ppp);
    Fq_rawSub(p3->x, p3->x, q);
    Fq_rawSub(p3->x, p3->x, q);
    
    // Y3 = R*(Q-X3)-S1*PPP
    Fq_rawMMul(tmp, s1, ppp);
    Fq_rawSub(p3->y, q, p3->x);
    Fq_rawMMul(p3->y, p3->y, r);
    Fq_rawSub(p3->y, p3->y, tmp);
    
    // ZZ3 = ZZ1*ZZ2*PP
    Fq_rawMMul(p3->zz, p1->zz, p2->zz);
    Fq_rawMMul(p3->zz, p3->zz, pp);
    
    // ZZZ3 = ZZZ1*ZZZ2*PPP
    Fq_rawMMul(p3->zzz, p1->zzz, p2->zzz);
    Fq_rawMMul(p3->zzz, p3->zzz, ppp);
}

void curve_add_mixed(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePointAffine* p2) {
#ifdef COUNT_OPS
    curve->cntAddMixed++;
#endif
    
    // Check special cases
    if (curve_is_zero(curve, p1)) {
        curve_copy_affine_to_point(curve, p3, p2);
        return;
    }
    
    if (curve_is_zero_affine(curve, p2)) {
        curve_copy(curve, p3, p1);
        return;
    }
    
    // Temporary variables
    FqRawElement u2, s2, p, r, pp, ppp, q, tmp;
    
    // U2 = X2*ZZ1
    Fq_rawMMul(u2, p2->x, p1->zz);
    
    // S2 = Y2*ZZZ1
    Fq_rawMMul(s2, p2->y, p1->zzz);
    
    // P = U2-X1
    Fq_rawSub(p, u2, p1->x);
    
    // R = S2-Y1
    Fq_rawSub(r, s2, p1->y);
    
    // Handle doubling case
    if (Fq_rawIsZero(p) && Fq_rawIsZero(r)) {
        curve_dbl_mixed(curve, p3, p2);
        return;
    }
    
    // PP = P^2
    Fq_rawMSquare(pp, p);
    
    // PPP = P*PP
    Fq_rawMMul(ppp, p, pp);
    
    // Q = X1*PP
    Fq_rawMMul(q, p1->x, pp);
    
    // X3 = R^2-PPP-2*Q
    Fq_rawMSquare(p3->x, r);
    Fq_rawSub(p3->x, p3->x, ppp);
    Fq_rawSub(p3->x, p3->x, q);
    Fq_rawSub(p3->x, p3->x, q);
    
    // Y3 = R*(Q-X3)-Y1*PPP
    Fq_rawMMul(tmp, p1->y, ppp);
    Fq_rawSub(p3->y, q, p3->x);
    Fq_rawMMul(p3->y, p3->y, r);
    Fq_rawSub(p3->y, p3->y, tmp);
    
    // ZZ3 = ZZ1*PP
    Fq_rawMMul(p3->zz, p1->zz, pp);
    
    // ZZZ3 = ZZZ1*PPP
    Fq_rawMMul(p3->zzz, p1->zzz, ppp);
}

void curve_add_affine(Curve* curve, CurvePoint* p3, CurvePointAffine* p1, CurvePointAffine* p2) {
#ifdef COUNT_OPS
    curve->cntAddAffine++;
#endif
    
    // Check special cases
    if (curve_is_zero_affine(curve, p1)) {
        curve_copy_affine_to_point(curve, p3, p2);
        return;
    }
    
    if (curve_is_zero_affine(curve, p2)) {
        curve_copy_affine_to_point(curve, p3, p1);
        return;
    }
    
    // Temporary variables
    FqRawElement p, r, pp, ppp, q, tmp;
    
    // P = X2-X1
    Fq_rawSub(p, p2->x, p1->x);
    
    // R = Y2-Y1
    Fq_rawSub(r, p2->y, p1->y);
    
    // Handle doubling case
    if (Fq_rawIsZero(p) && Fq_rawIsZero(r)) {
        CurvePoint tmp_point;
        curve_dbl_mixed(curve, &tmp_point, p2);
        curve_copy(curve, p3, &tmp_point);
        return;
    }
    
    // PP = P^2
    Fq_rawMSquare(pp, p);
    
    // PPP = P*PP
    Fq_rawMMul(ppp, p, pp);
    
    // Q = X1*PP
    Fq_rawMMul(q, p1->x, pp);
    
    // X3 = R^2-PPP-2*Q
    Fq_rawMSquare(p3->x, r);
    Fq_rawSub(p3->x, p3->x, ppp);
    Fq_rawSub(p3->x, p3->x, q);
    Fq_rawSub(p3->x, p3->x, q);
    
    // Y3 = R*(Q-X3)-Y1*PPP
    Fq_rawMMul(tmp, p1->y, ppp);
    Fq_rawSub(p3->y, q, p3->x);
    Fq_rawMMul(p3->y, p3->y, r);
    Fq_rawSub(p3->y, p3->y, tmp);
    
    // ZZ3 = PP
    Fq_rawCopy(p3->zz, pp);
    
    // ZZZ3 = PPP
    Fq_rawCopy(p3->zzz, ppp);
}

void curve_dbl(Curve* curve, CurvePoint* p3, CurvePoint* p1) {
#ifdef COUNT_OPS
    curve->cntDbl++;
#endif
    
    // Check special case
    if (curve_is_zero(curve, p1)) {
        curve_copy(curve, p3, p1);
        return;
    }
    
    // Temporary variables
    FqRawElement u, v, w, s, m, tmp;
    
    // U = 2*Y1
    Fq_rawAdd(u, p1->y, p1->y);
    
    // V = U^2
    Fq_rawMSquare(v, u);
    
    // W = U*V
    Fq_rawMMul(w, u, v);
    
    // S = X1*V
    Fq_rawMMul(s, p1->x, v);
    
    // M = 3*X1^2+a*ZZ1^2
    Fq_rawMSquare(m, p1->x);
    Fq_rawAdd(tmp, m, m);
    Fq_rawAdd(m, m, tmp);
    
    if (curve->typeOfA != A_IS_ZERO) {
        Fq_rawMSquare(tmp, p1->zz);
        curve_mul_by_a(curve, &tmp, &tmp);
        Fq_rawAdd(m, m, tmp);
    }
    
    // X3 = M^2-2*S
    Fq_rawMSquare(p3->x, m);
    Fq_rawSub(p3->x, p3->x, s);
    Fq_rawSub(p3->x, p3->x, s);
    
    // Y3 = M*(S-X3)-W*Y1
    Fq_rawMMul(tmp, w, p1->y);
    Fq_rawSub(p3->y, s, p3->x);
    Fq_rawMMul(p3->y, m, p3->y);
    Fq_rawSub(p3->y, p3->y, tmp);
    
    // ZZ3 = V*ZZ1
    Fq_rawMMul(p3->zz, v, p1->zz);
    
    // ZZZ3 = W*ZZZ1
    Fq_rawMMul(p3->zzz, w, p1->zzz);
}

void curve_dbl_mixed(Curve* curve, CurvePoint* p3, CurvePointAffine* p1) {
#ifdef COUNT_OPS
    curve->cntDblMixed++;
#endif
    
    // Check special case
    if (curve_is_zero_affine(curve, p1)) {
        curve_copy_affine_to_point(curve, p3, p1);
        return;
    }
    
    // Get field constants
    FqElement one_elem;
    FqRawElement one_raw;
    
    Fq_set_one(&one_elem);
    Fq_toNormal(&one_elem, &one_elem);
    
    if (one_elem.type & Fq_LONG) {
        Fq_rawCopy(one_raw, one_elem.longVal);
    } else {
        Fq_rawCopyS2L(one_raw, one_elem.shortVal);
    }
    
    // Temporary variables
    FqRawElement u, m, s, tmp;
    
    // U = 2*Y1
    Fq_rawAdd(u, p1->y, p1->y);
    
    // V = U^2 (directly stored in ZZ3)
    Fq_rawMSquare(p3->zz, u);
    
    // W = U*V (directly stored in ZZZ3)
    Fq_rawMMul(p3->zzz, u, p3->zz);
    
    // S = X1*V
    Fq_rawMMul(s, p1->x, p3->zz);
    
    // M = 3*X1^2+a
    Fq_rawMSquare(m, p1->x);
    Fq_rawAdd(tmp, m, m);
    Fq_rawAdd(m, tmp, m);
    Fq_rawAdd(m, m, curve->a);
    
    // X3 = M^2-2*S
    Fq_rawMSquare(p3->x, m);
    Fq_rawSub(p3->x, p3->x, s);
    Fq_rawSub(p3->x, p3->x, s);
    
    // Y3 = M*(S-X3)-W*Y1
    Fq_rawMMul(tmp, p3->zzz, p1->y);
    Fq_rawSub(p3->y, s, p3->x);
    Fq_rawMMul(p3->y, m, p3->y);
    Fq_rawSub(p3->y, p3->y, tmp);
    
    // ZZ3 = V  (already set)
    // ZZZ3 = W (already set)
}

void curve_neg(Curve* curve, CurvePoint* r, CurvePoint* a) {
    Fq_rawCopy(r->x, a->x);
    Fq_rawNeg(r->y, a->y);
    Fq_rawCopy(r->zz, a->zz);
    Fq_rawCopy(r->zzz, a->zzz);
}

void curve_neg_affine(Curve* curve, CurvePointAffine* r, CurvePointAffine* a) {
    Fq_rawCopy(r->x, a->x);
    Fq_rawNeg(r->y, a->y);
}

/* ========== Subtraction Operations ========== */

void curve_sub(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePoint* p2) {
    CurvePoint tmp;
    curve_neg(curve, &tmp, p2);
    curve_add(curve, p3, p1, &tmp);
}

void curve_sub_mixed(Curve* curve, CurvePoint* p3, CurvePoint* p1, CurvePointAffine* p2) {
    CurvePointAffine tmp;
    curve_neg_affine(curve, &tmp, p2);
    curve_add_mixed(curve, p3, p1, &tmp);
}

void curve_sub_affine(Curve* curve, CurvePoint* p3, CurvePointAffine* p1, CurvePointAffine* p2) {
    CurvePointAffine tmp;
    curve_neg_affine(curve, &tmp, p2);
    curve_add_affine(curve, p3, p1, &tmp);
}

/* ========== Comparison Operations */