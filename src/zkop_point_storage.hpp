#ifndef ZKOP_POINT_STRORAGE_HPP
#define ZKOP_POINT_STRORAGE_HPP

#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <array>
#include "point_storage_interface.hpp"
#include "zero_tracker.hpp""

/**
 * Implementation for zkop format
 * Direct memory access to point data
 */
template<typename Point>
class ZKopPointStorage : public PointStorageInterface<Point> {
private:
    MemoryEfficientZeroTracker zeroTracker;
    uint32_t point_count;
    uint32_t point_size;
    const uint8_t* point_data;

public:
    /**
    * Constructor for zkey point storage
     * @param raw_data Pointer to point section data
     * @param data_size Size of the point data in bytes
     */
    ZKopPointStorage(const void* raw_data, uint64_t data_size) : zeroTracker(*static_cast<const uint32_t*>(raw_data)) {
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

        const uint8_t* point_ptr = point_data + compressed_index * point_size;
        return *reinterpret_cast<const Point*>(point_ptr);
    }

    /**
     * Get the total number of coefficients
     */
    uint32_t getPointCount() const override {
        return point_count;
    }
};

#endif // ZKOP_POINT_STRORAGE_HPP