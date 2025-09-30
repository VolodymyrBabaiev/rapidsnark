#ifndef ZKOP_COMPRESS_POINT_STRORAGE_HPP
#define ZKOP_COMPRESS_POINT_STRORAGE_HPP

#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <array>
#include "point_storage_interface.hpp"
#include "zero_tracker.hpp"
#include "point_compress.hpp"

/**
 * Implementation for compressed zkop format
 * Direct memory access to point data
 */
template<typename Point, typename Engine>
class ZKopCompressPointStorage : public PointStorageInterface<Point> {
private:
    MemoryEfficientZeroTracker zeroTracker;
    uint32_t point_count;
    uint32_t point_size;
    const uint8_t* point_data;

    //const uint8_t* compressed_data;
    //uint32_t point_count;
    uint32_t compressed_point_size;
    Engine& engine;

    // Size of a coordinate (32 bytes for Alt-BN128)
    static constexpr size_t COORDINATE_SIZE = 32;
    // Size of compressed point: x-coordinate + 1 parity bit (packed in 1 byte)
    static constexpr size_t COMPRESSED_SIZE = COORDINATE_SIZE;

public:
    /**
    * Constructor for zkey point storage
     * @param raw_data Pointer to point section data
     * @param data_size Size of the point data in bytes
     */
    ZKopCompressPointStorage(const void* raw_data, uint64_t data_size, Engine& eng) : zeroTracker(*static_cast<const uint32_t*>(raw_data)), engine(eng)  {
        point_count = *static_cast<const uint32_t*>(raw_data);
        size_t bytes_needed = (point_count + 7) / 8;

        point_size = sizeof(Point);
        const uint8_t* data  = static_cast<const uint8_t*>(raw_data) + 4;
        zeroTracker.setBitArray(data, point_count);
        point_data = data + bytes_needed;
    }

    Point get(uint32_t index) const override {
        if (index >= point_count) {
            throw std::out_of_range("Point index out of range");
        }

        // Check if this point is zero
        if (zeroTracker.isZeroAt(index)) {
            // Return zero point
            Point zero_point;
            std::memset(&zero_point, 0, sizeof(Point));
            return zero_point;
        }

        // Calculate position in compressed data
        uint32_t compressed_index = index - zeroTracker.getZerosBeforePosition(index);

        const uint8_t* point_ptr = point_data + compressed_index * COMPRESSED_SIZE;
        return decompressYCoordinateMontgomery(*reinterpret_cast<const typename Engine::F1::Element*>(point_ptr), engine);
    }

    /**
     * Get the total number of coefficients
     */
    uint32_t getPointCount() const override {
        return point_count;
    }
};

#endif // ZKOP_COMPRESS_POINT_STRORAGE_HPP