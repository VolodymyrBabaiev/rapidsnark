/**
 * msm_c.c - Multi-Scalar Multiplication implementation in C
 * Converted from C++ template implementation
 * Single-threaded version
 */

#include <stdlib.h>
#include <string.h>
#include "msm_c.h"
#include "curve_c.h"

// Configuration constants
#define MIN_CHUNK_SIZE_BITS 3
#define MAX_CHUNK_SIZE_BITS 16

#ifdef MSM_BITS_PER_CHUNK
// Use predefined chunk size if defined
uint64_t msm_get_bits_per_chunk(MSM* msm, uint64_t n, uint64_t scalarSize) {
    return MSM_BITS_PER_CHUNK;
}
#else
// Calculate optimal chunk size
uint64_t msm_get_bits_per_chunk(MSM* msm, uint64_t n, uint64_t scalarSize) {
    return msm_calc_bits_per_chunk(msm, n, scalarSize);
}
#endif

// Calculate adds count for a given configuration
uint64_t msm_calc_adds_count(MSM* msm, uint64_t nPoints, uint64_t scalarSize, uint64_t bitsPerChunk) {
    return msm_calc_chunk_count(msm, scalarSize, bitsPerChunk) * 
           (nPoints + ((uint64_t)1 << bitsPerChunk) + bitsPerChunk + 1);
}

// Calculate optimal bits per chunk
uint64_t msm_calc_bits_per_chunk(MSM* msm, uint64_t n, uint64_t scalarSize) {
    uint64_t bitsPerChunk = MIN_CHUNK_SIZE_BITS;
    uint64_t minAdds = msm_calc_adds_count(msm, n, scalarSize, bitsPerChunk);

    for (uint64_t k = MIN_CHUNK_SIZE_BITS + 1; k <= MAX_CHUNK_SIZE_BITS; k++) {
        const uint64_t curAdds = msm_calc_adds_count(msm, n, scalarSize, k);

        if (curAdds < minAdds) {
            minAdds = curAdds;
            bitsPerChunk = k;
        }
    }
    return bitsPerChunk;
}

// Calculate number of chunks required
uint64_t msm_calc_chunk_count(MSM* msm, uint64_t scalarSize, uint64_t bitsPerChunk) {
    return ((scalarSize * 8 - 1) / bitsPerChunk) + 1;
}

// Calculate number of buckets required
uint64_t msm_calc_bucket_count(MSM* msm, uint64_t bitsPerChunk) {
    return ((uint64_t)1 << (bitsPerChunk-1));
}

// Get bucket index for a given scalar and chunk
uint64_t msm_get_bucket_index(MSM* msm, uint64_t scalarIdx, uint64_t chunkIdx) {
    uint64_t bitStart = chunkIdx * msm->bitsPerChunk;
    uint64_t byteStart = bitStart / 8;
    uint64_t effectiveBitsPerChunk = msm->bitsPerChunk;

    if (byteStart > msm->scalarSize - 8) {
        byteStart = msm->scalarSize - 8;
    }
    
    if (bitStart + msm->bitsPerChunk > msm->scalarSize * 8) {
        effectiveBitsPerChunk = msm->scalarSize * 8 - bitStart;
    }

    uint64_t shift = bitStart - byteStart * 8;
    uint64_t v = *(uint64_t *)(msm->scalars + scalarIdx * msm->scalarSize + byteStart);

    v = v >> shift;
    v = v & (((uint64_t)1 << effectiveBitsPerChunk) - 1);

    return v;
}

// Initialize an MSM context
MSM* msm_init(Curve* curve) {
    MSM* msm = (MSM*)malloc(sizeof(MSM));
    if (!msm) return NULL;
    
    msm->curve = curve;
    msm->scalars = NULL;
    msm->scalarSize = 0;
    msm->bitsPerChunk = 0;
    
    return msm;
}

// Free MSM resources
void msm_free(MSM* msm) {
    if (msm) {
        free(msm);
    }
}

// Run the MSM algorithm
void msm_run(MSM* msm, 
             CurvePoint* r,
             CurvePointAffine* bases,
             uint8_t* scalars,
             uint64_t scalarSize,
             uint64_t nPoints) {
    
    // Store inputs in MSM context
    msm->scalars = scalars;
    msm->scalarSize = scalarSize;
    
    // Handle special cases
    if (nPoints == 0) {
        curve_copy(msm->curve, r, &msm->curve->zero);
        return;
    }
    
    if (nPoints == 1) {
        curve_mul_by_scalar(msm->curve, r, bases, scalars, scalarSize);
        return;
    }

    // Calculate parameters
    msm->bitsPerChunk = msm_get_bits_per_chunk(msm, nPoints, scalarSize);
    uint64_t nChunks = msm_calc_chunk_count(msm, scalarSize, msm->bitsPerChunk);
    uint64_t nBuckets = msm_calc_bucket_count(msm, msm->bitsPerChunk);

    // Allocate memory for intermediate results
    CurvePoint* buckets = (CurvePoint*)malloc(nBuckets * sizeof(CurvePoint));
    int32_t* slicedScalars = (int32_t*)malloc(nChunks * nPoints * sizeof(int32_t));
    CurvePoint* chunks = (CurvePoint*)malloc(nChunks * sizeof(CurvePoint));
    
    if (!buckets || !slicedScalars || !chunks) {
        if (buckets) free(buckets);
        if (slicedScalars) free(slicedScalars);
        if (chunks) free(chunks);
        return; // Memory allocation error
    }
    
    // Pre-process scalars
    for (uint64_t i = 0; i < nPoints; i++) {
        int32_t carry = 0;

        for (uint64_t j = 0; j < nChunks; j++) {
            uint64_t rawBucketIndex = msm_get_bucket_index(msm, i, j);
            int32_t bucketIndex = (int32_t)rawBucketIndex + carry;

            if (bucketIndex >= nBuckets) {
                bucketIndex -= nBuckets*2;
                carry = 1;
            } else {
                carry = 0;
            }

            slicedScalars[i*nChunks + j] = bucketIndex;
        }
    }
    
    // Process each chunk
    for (uint64_t j = 0; j < nChunks; j++) {
        // Initialize buckets for this chunk
        for (uint64_t i = 0; i < nBuckets; i++) {
            curve_copy(msm->curve, &buckets[i], &msm->curve->zero);
        }
        
        // Accumulate points into buckets
        for (uint64_t i = 0; i < nPoints; i++) {
            const int32_t bucketIndex = slicedScalars[i*nChunks + j];
            
            if (bucketIndex > 0) {
                curve_add_mixed(msm->curve, &buckets[bucketIndex-1], 
                                &buckets[bucketIndex-1], &bases[i]);
            } else if (bucketIndex < 0) {
                CurvePointAffine negPoint;
                curve_neg_affine(msm->curve, &negPoint, &bases[i]);
                curve_add_mixed(msm->curve, &buckets[(-bucketIndex)-1], 
                                &buckets[(-bucketIndex)-1], &negPoint);
            }
        }
        
        // Compute running sum and final chunk result using bucket method
        CurvePoint sum, tmp;
        curve_copy(msm->curve, &sum, &buckets[nBuckets - 1]);
        curve_copy(msm->curve, &tmp, &sum);
        
        for (int64_t i = nBuckets - 2; i >= 0; i--) {
            curve_add(msm->curve, &tmp, &tmp, &buckets[i]);
            curve_add(msm->curve, &sum, &sum, &tmp);
        }
        
        curve_copy(msm->curve, &chunks[j], &sum);
    }
    
    // Combine chunks into final result
    curve_copy(msm->curve, r, &chunks[nChunks - 1]);
    
    for (int64_t j = nChunks - 2; j >= 0; j--) {
        // Double r by bitsPerChunk times
        for (uint64_t i = 0; i < msm->bitsPerChunk; i++) {
            curve_dbl(msm->curve, r, r);
        }
        curve_add(msm->curve, r, r, &chunks[j]);
    }
    
    // Free allocated memory
    free(buckets);
    free(slicedScalars);
    free(chunks);
}
