#ifndef VB_KEY_OPT_ZKOP_COEF_PROVIDER1_HPP
#define VB_KEY_OPT_ZKOP_COEF_PROVIDER1_HPP

#include "coef_storage_interface.hpp"

/**
 * Implementation for traditional zkey format
 * Direct memory access to coefficient data
 */
template<typename Engine>
class ZKopCoefStorage : public CoefStorageInterface<Engine> {
private:
    struct UniqueCoef {
        std::array<uint8_t, 32> coef_data;
        uint32_t cumulative_count;

        UniqueCoef() : cumulative_count(0) {
 		   coef_data.fill(0);
		}
        UniqueCoef(const uint8_t* data, uint32_t cum_count) : cumulative_count(cum_count) {
    		std::memcpy(coef_data.data(), data, 32);
		}
    };

    const void* coef_data_;
    uint32_t coef_count_;
    std::vector<UniqueCoef> unique_coefs_;
    const uint8_t* instances_data_;  // Pointer to coefficient instances in memory
    uint32_t unique_coef_count_;

	// Binary search for the first cumulative_count > coef_index
	uint32_t findUniqueCoefIndex(uint32_t coef_index) const {
    	auto it = std::upper_bound(unique_coefs_.begin(), unique_coefs_.end(), coef_index,
        	[](uint32_t index, const UniqueCoef& unique_coef) {
            	return index < unique_coef.cumulative_count;
        	});

    	if (it == unique_coefs_.end()) {
        	throw std::out_of_range("Coefficient index out of range");
    	}

    	return std::distance(unique_coefs_.begin(), it);
	}

public:

    /**
    * Constructor for zkey coefficient storage
     * @param coef_data Pointer to coefficient section data
     * @param data_size Size of the coefficient data in bytes
     */
    ZKopCoefStorage(const void* coef_data, uint64_t data_size) {
    	if (!coef_data || data_size < 8) {
        	throw std::invalid_argument("Invalid data pointer or size too small");
    	}

	    const uint8_t* data = static_cast<const uint8_t*>(coef_data);
	    uint64_t pos = 0;

    	// Read total coefficient count
    	if (pos + 4 > data_size) {
        	throw std::runtime_error("Unexpected end of data while reading total coef count");
    	}
    	coef_count_ = *reinterpret_cast<const uint32_t*>(data + pos);
    	pos += 4;

    	// Read unique coefficient count
    	if (pos + 4 > data_size) {
        	throw std::runtime_error("Unexpected end of data while reading unique coef count");
    	}
    	unique_coef_count_ = *reinterpret_cast<const uint32_t*>(data + pos);
    	pos += 4;

    	// Reserve space for unique coefficients
    	unique_coefs_.reserve(unique_coef_count_);

    	// Read unique coefficients and their cumulative counts
    	for (uint32_t i = 0; i < unique_coef_count_; ++i) {
        	// Read 32-byte coefficient data
        	if (pos + 32 > data_size) {
            	throw std::runtime_error("Unexpected end of data while reading unique coef");
        	}

        	const uint8_t* coef_data = data + pos;
        	pos += 32;

        	// Read cumulative count
        	if (pos + 4 > data_size) {
            	throw std::runtime_error("Unexpected end of data while reading cumulative count");
        	}
        	uint32_t cum_count = *reinterpret_cast<const uint32_t*>(data + pos);
        	pos += 4;

        	unique_coefs_.emplace_back(coef_data, cum_count);
    	}

    	// Set pointer to coefficient instances data
    	if (pos + coef_count_ * 8 > data_size) {
        	throw std::runtime_error("Insufficient data for coefficient instances");
    	}
    	instances_data_ = data + pos;

    	// Validate cumulative counts are sorted (required for binary search)
    	for (uint32_t i = 1; i < unique_coef_count_; ++i) {
        	if (unique_coefs_[i-1].cumulative_count >= unique_coefs_[i].cumulative_count) {
            	throw std::runtime_error("Invalid cumulative counts: not strictly increasing");
        	}
    	}

    	// Validate last cumulative count matches total
    	if (!unique_coefs_.empty() && unique_coefs_.back().cumulative_count != coef_count_) {
        	throw std::runtime_error("Last cumulative count doesn't match total coefficient count");
    	}
    }

    Coef<Engine> get(uint32_t index) const override {
 	   if (index >= coef_count_) {
    	    throw std::out_of_range("Coefficient index out of range");
    	}

    	Coef<Engine> coef;
    	// Get packed_c and s from memory directly
    	const uint32_t* instance_ptr = reinterpret_cast<const uint32_t*>(instances_data_ + index * 8);
    	uint32_t packed_c = instance_ptr[0];
    	coef.s = instance_ptr[1];

    	// Extract m and c from packed_c
    	coef.m = (packed_c & 0x80000000) ? 1 : 0;
    	coef.c = packed_c & 0x7FFFFFFF;

    	// Binary search to find which unique coefficient this instance refers to
    	uint32_t unique_index = findUniqueCoefIndex(index);

    	memcpy(&coef.coef, unique_coefs_[unique_index].coef_data.data(), 32);

    	return coef;
    }

    /**
     * Get the total number of coefficients
     */
    uint32_t getCoefCount() const override {
        return coef_count_;
    }

    /**
     * Get the number of unique coefficients
     */
	uint32_t getUniqueCoefCount() const {
    	return unique_coef_count_;
	}
};

#endif //VB_KEY_OPT_ZKOP_COEF_PROVIDER1_HPP