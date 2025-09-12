#ifndef ZKEY_POINT_STORAGE_HPP
#define ZKEY_POINT_STORAGE_HPP

#include "point_storage_interface.hpp"

/**
 * Implementation for traditional zkey format
 * Direct memory access to point data
 */
template<typename Point>
class ZKeyPointStorage : public PointStorageInterface<Point> {
private:
    const uint8_t* point_data;
    uint32_t point_count;
    uint32_t point_size;

public:
    /**
    * Constructor for zkey point storage
     * @param raw_data Pointer to point section data
     * @param point_size Size of the point data in bytes
     */
    ZKeyPointStorage(const void* raw_data, uint64_t points_size): point_data(static_cast<const uint8_t*>(raw_data)) {
        point_size = sizeof(Point);
        point_count = points_size / point_size;
    }

    Point get(uint32_t index) const override {
        if (index >= point_count) {
            throw std::out_of_range("Point index out of range");
        }

        const uint8_t* point_ptr = point_data + index * point_size;

        return *reinterpret_cast<const Point*>(point_ptr);
    }

    /**
     * Get the total number of coefficients
     */
    uint32_t getPointCount() const override {
        return point_count;
    }
};

#endif //ZKEY_POINT_STORAGE_HPP