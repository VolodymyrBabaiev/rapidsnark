#ifndef POINT_STORAGE_INTERFACE_HPP
#define POINT_STORAGE_INTERFACE_HPP

/**
 * Abstract base class for point access
 * Provides common interface for both zkey and zkop formats
 */
template<typename Point>
class PointStorageInterface {
public:
    virtual ~PointStorageInterface() = default;

    /**
     * Get Point by index
     * @param index The point index (0 to getPointCount()-1)
     * @return Point
     */
    virtual Point get(uint32_t index) const = 0;

    /**
     * Get the total number of points
     */
    virtual uint32_t getPointCount() const = 0;

    /**
     * Get point count (alternative method name for compatibility)
     */
    uint32_t size() const { return getPointCount(); }

    /**
     * Check if index is valid
     */
    virtual bool isValidIndex(uint32_t index) const {
        return index < getPointCount();
    }
};

#endif // POINT_STORAGE_INTERFACE_HPP