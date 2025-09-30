#ifndef ZERO_TRACKER_HPP
#define ZERO_TRACKER_HPP

#include <array>
#include <vector>
#include <iostream>
#include <cstdint>

class MemoryEfficientZeroTracker {
private:
    // Use vector<uint8_t> to store 8 bits per element (more cache-friendly)
    std::vector<uint8_t> bit_array;
    size_t array_size;

    // Cache for prefix sums - store every CACHE_INTERVAL positions
    static constexpr size_t CACHE_INTERVAL = 512;  // Smaller interval for uint8_t
    std::vector<uint32_t> prefix_cache;  // cached prefix sums

    // Precomputed lookup table for popcount of uint8_t values (0-255)
    static constexpr std::array<uint8_t, 256> POPCOUNT_LUT = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,  // 0x00 - 0x0F
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  // 0x10 - 0x1F
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  // 0x20 - 0x2F
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  // 0x30 - 0x3F
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  // 0x40 - 0x4F
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  // 0x50 - 0x5F
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  // 0x60 - 0x6F
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  // 0x70 - 0x7F
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  // 0x80 - 0x8F
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  // 0x90 - 0x9F
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  // 0xA0 - 0xAF
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  // 0xB0 - 0xBF
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  // 0xC0 - 0xCF
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  // 0xD0 - 0xDF
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  // 0xE0 - 0xEF
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8   // 0xF0 - 0xFF
    };

    // Helper to get bit at position
    bool getBit(size_t pos) const {
        if (pos >= array_size) return false;
        size_t byte_idx = pos / 8;
        size_t bit_idx = pos % 8;
        return (bit_array[byte_idx] >> bit_idx) & 1;
    }

    // Count set bits in a byte using lookup table
    uint8_t popcount(uint8_t byte) const {
        return POPCOUNT_LUT[byte];
    }

    // Count set bits in a range of bytes efficiently
    uint32_t countBitsInRange(size_t start_pos, size_t end_pos) const {
        if (start_pos >= end_pos) return 0;

        uint32_t count = 0;
        size_t pos = start_pos;

        // Handle partial first byte
        size_t start_byte = start_pos / 8;
        size_t start_bit = start_pos % 8;
        if (start_bit != 0 && pos < end_pos) {
            size_t bits_in_first_byte = std::min(8 - start_bit, end_pos - pos);
            for (size_t i = 0; i < bits_in_first_byte; i++) {
                if (getBit(pos + i)) count++;
            }
            pos += bits_in_first_byte;
        }

        // Handle complete bytes
        size_t current_byte = pos / 8;
        size_t end_byte = end_pos / 8;

        while (current_byte < end_byte && pos + 8 <= end_pos) {
            count += popcount(bit_array[current_byte]);
            pos += 8;
            current_byte++;
        }

        // Handle partial last byte
        while (pos < end_pos) {
            if (getBit(pos)) count++;
            pos++;
        }

        return count;
    }

public:
    MemoryEfficientZeroTracker(uint8_t* pBitArray, uint32_t arr_size) : array_size(arr_size) {
        // Allocate bit array - need (size + 7) / 8 bytes
        size_t bytes_needed = (array_size + 7) / 8;
        bit_array.resize(bytes_needed, 0);

        // Set bits for zero positions
        for (size_t i = 0; i < bytes_needed; i++) {
            bit_array[i] = pBitArray[i];
        }

        // Build prefix sum cache
        buildCache();
    }

    MemoryEfficientZeroTracker(uint32_t arr_size) : array_size(arr_size) {
        // Allocate bit array - need (size + 7) / 8 bytes
        size_t bytes_needed = (array_size + 7) / 8;
        bit_array.resize(bytes_needed, 0);
    }

    std::vector<uint8_t>& getBitArray() {
        return bit_array;
    }

    void setBitArray(const uint8_t* pBitArray, const uint32_t arr_size) {
        size_t bytes_needed = (arr_size + 7) / 8;
        bit_array.resize(bytes_needed, 0);
        for (size_t i = 0; i < bytes_needed; i++) {
            bit_array[i] = pBitArray[i];
        }

		buildCache();
    }

    // Alternative constructor that takes a callback to check if position is zero
    // Useful for very large arrays where you don't want to store the original
    template<typename ZeroCheckFunc>
    MemoryEfficientZeroTracker(size_t size, ZeroCheckFunc is_zero_func) : array_size(size) {
        size_t bytes_needed = (array_size + 7) / 8;
        bit_array.resize(bytes_needed, 0);

        for (size_t i = 0; i < array_size; i++) {
            if (is_zero_func(i)) {
                setBit(i, true);
            }
        }

        buildCache();
    }

    // Helper to set bit at position
    void setBit(size_t pos, bool value) {
        if (pos >= array_size) return;
        size_t byte_idx = pos / 8;
        size_t bit_idx = pos % 8;

        if (value) {
            bit_array[byte_idx] |= (1 << bit_idx);
        } else {
            bit_array[byte_idx] &= ~(1 << bit_idx);
        }
    }

    // Build prefix cache using efficient byte counting
    void buildCache() {
        size_t cache_size = (array_size + CACHE_INTERVAL - 1) / CACHE_INTERVAL;
        prefix_cache.resize(cache_size);

        uint32_t running_count = 0;
        for (size_t cache_idx = 0; cache_idx < cache_size; cache_idx++) {
            prefix_cache[cache_idx] = running_count;

            // Count zeros in the next CACHE_INTERVAL positions efficiently
            size_t start_pos = cache_idx * CACHE_INTERVAL;
            size_t end_pos = std::min(start_pos + CACHE_INTERVAL, array_size);

            running_count += countBitsInRange(start_pos, end_pos);
        }
    }

    // Get number of zeros before position pos (exclusive)
    uint32_t getZerosBeforePosition(size_t pos) const {
        if (pos == 0) return 0;
        if (pos > array_size) pos = array_size;

        // Find the cached position <= our target
        size_t cache_idx = (pos - 1) / CACHE_INTERVAL;
        size_t cache_pos = cache_idx * CACHE_INTERVAL;

        uint32_t count = (cache_idx < prefix_cache.size()) ? prefix_cache[cache_idx] : 0;

        // Count from cache position to our target using efficient method
        count += countBitsInRange(cache_pos, pos);

        return count;
    }

    // Get number of zeros in range [start, end)
    uint32_t getZerosInRange(size_t start, size_t end) const {
        if (start >= end) return 0;
        return getZerosBeforePosition(end) - getZerosBeforePosition(start);
    }

    // Get total number of zeros
    uint32_t getTotalZeros() const {
        return getZerosBeforePosition(array_size);
    }

    // Check if position contains zero
    bool isZeroAt(size_t pos) const {
        return getBit(pos);
    }

    // Update zero status at position (and invalidate cache if needed)
    void setZeroAt(size_t pos, bool is_zero) {
        if (pos >= array_size) return;

        bool was_zero = getBit(pos);
        if (was_zero != is_zero) {
            setBit(pos, is_zero);

            // Invalidate cache from this position onwards
            size_t cache_idx = pos / CACHE_INTERVAL;
            if (cache_idx < prefix_cache.size()) {
                // Rebuild cache from this point
                rebuildCacheFrom(cache_idx);
            }
        }
    }

    // Get memory usage in bytes
    size_t getMemoryUsage() const {
        size_t bit_array_bytes = bit_array.size() * sizeof(uint8_t);
        size_t cache_bytes = prefix_cache.size() * sizeof(uint32_t);
        return bit_array_bytes + cache_bytes + sizeof(*this);
    }

    // Get statistics about the bit array
    void printStatistics() const {
        std::cout << "=== Zero Tracker Statistics ===" << std::endl;
        std::cout << "Array size: " << array_size << " elements" << std::endl;
        std::cout << "Bit array size: " << bit_array.size() << " bytes" << std::endl;
        std::cout << "Cache size: " << prefix_cache.size() << " entries ("
                  << prefix_cache.size() * sizeof(uint32_t) << " bytes)" << std::endl;
        std::cout << "Cache interval: " << CACHE_INTERVAL << " positions" << std::endl;
        std::cout << "Total memory: " << getMemoryUsage() << " bytes" << std::endl;
        std::cout << "Memory per element: " << (double)getMemoryUsage() / array_size << " bytes" << std::endl;
        std::cout << "Total zeros: " << getTotalZeros() << std::endl;
        std::cout << "Zero density: " << (double)getTotalZeros() / array_size * 100 << "%" << std::endl;
    }

    // Get all zero positions (for smaller arrays or debugging)
    std::vector<size_t> getZeroPositions() const {
        std::vector<size_t> positions;
        positions.reserve(getTotalZeros()); // Pre-allocate if we know the count

        for (size_t i = 0; i < array_size; i++) {
            if (getBit(i)) {
                positions.push_back(i);
            }
        }
        return positions;
    }

    // Find next zero position after given position
    size_t findNextZero(size_t start_pos) const {
        for (size_t i = start_pos + 1; i < array_size; i++) {
            if (getBit(i)) return i;
        }
        return SIZE_MAX; // Not found
    }

    // Find previous zero position before given position
    size_t findPreviousZero(size_t start_pos) const {
        if (start_pos == 0) return SIZE_MAX;
        for (size_t i = start_pos - 1; i != SIZE_MAX; i--) {
            if (getBit(i)) return i;
        }
        return SIZE_MAX; // Not found
    }

    // Rebuild entire cache (call after many updates)
    void rebuildCache() {
        buildCache();
    }

    // Make POPCOUNT_LUT accessible for testing/demonstration
    static const std::array<uint8_t, 256>& getPopcountLUT() {
        return POPCOUNT_LUT;
    }

private:
    // Rebuild cache from given cache index onwards using efficient counting
    void rebuildCacheFrom(size_t start_cache_idx) {
        if (start_cache_idx >= prefix_cache.size()) return;

        // Get count up to start of this cache block
        uint32_t running_count = (start_cache_idx > 0) ? prefix_cache[start_cache_idx - 1] : 0;
        if (start_cache_idx > 0) {
            // Add count for the previous complete block
            size_t prev_start = (start_cache_idx - 1) * CACHE_INTERVAL;
            size_t prev_end = std::min(prev_start + CACHE_INTERVAL, array_size);
            running_count += countBitsInRange(prev_start, prev_end);
        }

        // Rebuild from start_cache_idx onwards
        for (size_t cache_idx = start_cache_idx; cache_idx < prefix_cache.size(); cache_idx++) {
            prefix_cache[cache_idx] = running_count;

            size_t start_pos = cache_idx * CACHE_INTERVAL;
            size_t end_pos = std::min(start_pos + CACHE_INTERVAL, array_size);

            running_count += countBitsInRange(start_pos, end_pos);
        }
    }
};

#endif //ZERO_TRACKER_HPP