/**
 * msm_c.h - Multi-Scalar Multiplication implementation in C
 * Converted from C++ template implementation
 * Single-threaded version
 */

#ifndef MSM_H
#define MSM_H

#include <stdint.h>
#include <stdbool.h>
#include "curve_c.h"

/**
 * MSM structure for Multi-Scalar Multiplication
 */
typedef struct {
    Curve* curve;
    uint8_t* scalars;
    uint64_t scalarSize;
    uint64_t bitsPerChunk;
} MSM;

/**
 * Initialize an MSM context for the given curve
 */
MSM* msm_init(Curve* curve);

/**
 * Free resources used by an MSM context
 */
void msm_free(MSM* msm);

/**
 * Execute Multi-Scalar Multiplication and store result in r
 * 
 * @param msm The MSM context
 * @param r Result point where computation is stored
 * @param bases Array of affine points to use as bases
 * @param scalars Array of scalars to multiply by
 * @param scalarSize Size of each scalar in bytes
 * @param nPoints Number of points/scalars
 */
void msm_run(MSM* msm, 
             CurvePoint* r,
             CurvePointAffine* bases,
             uint8_t* scalars,
             uint64_t scalarSize,
             uint64_t nPoints);

/**
 * Internal function to calculate bits per chunk
 */
uint64_t msm_get_bits_per_chunk(MSM* msm, uint64_t n, uint64_t scalarSize);

/**
 * Internal function to calculate optimal chunk size
 */
uint64_t msm_calc_bits_per_chunk(MSM* msm, uint64_t n, uint64_t scalarSize);

/**
 * Internal function to calculate chunk count
 */
uint64_t msm_calc_chunk_count(MSM* msm, uint64_t scalarSize, uint64_t bitsPerChunk);

/**
 * Internal function to calculate bucket count
 */
uint64_t msm_calc_bucket_count(MSM* msm, uint64_t bitsPerChunk);

/**
 * Internal function to get bucket index for a given scalar/chunk
 */
uint64_t msm_get_bucket_index(MSM* msm, uint64_t scalarIdx, uint64_t chunkIdx);

#endif // MSM_H
