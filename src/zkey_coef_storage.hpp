#ifndef ZKEY_COEF_STORAGE_H
#define ZKEY_COEF_STORAGE_H

#include "coef_storage_interface.hpp"

/**
 * Implementation for traditional zkey format
 * Direct memory access to coefficient data
 */
template<typename Engine>
class ZKeyCoefStorage : public CoefStorageInterface<Engine> {
private:
    const void* coef_data_;
    uint32_t coef_count_;

public:
    /**
    * Constructor for zkey coefficient storage
     * @param coef_data Pointer to coefficient section data (after 4-byte count)
     * @param data_size Size of the coefficient data in bytes
     */
    ZKeyCoefStorage(const void* coef_data, uint64_t data_size) {

        if (!coef_data || data_size < 4) {
            throw std::invalid_argument("Invalid coefficient data");
        }

        // First 4 bytes contain the coefficient count
        const uint32_t* count_ptr = static_cast<const uint32_t*>(coef_data);
        coef_count_ = *count_ptr;

        // Coefficient data starts after the count
        coef_data_ = static_cast<const uint8_t*>(coef_data) + 4;

        // Verify data size
        uint64_t expected_size = 4 + (uint64_t)coef_count_ * sizeof(Coef<Engine>);
        if (data_size < expected_size) {
            throw std::invalid_argument("Insufficient data for coefficient count");
        }
    }

    Coef<Engine> get(uint32_t index) const override {
        if (index >= coef_count_) {
            throw std::out_of_range("Coefficient index out of range");
        }

        const Coef<Engine>* coefs = static_cast<const Coef<Engine>*>(coef_data_);

        return coefs[index];
    }

    /**
     * Get the total number of coefficients
     */
    uint32_t getCoefCount() const override {
        return coef_count_;
    }
};

#endif // ZKEY_COEF_STORAGE_H