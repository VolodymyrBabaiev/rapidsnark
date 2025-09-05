#ifndef VB_KEY_OPT_COEF_INTERFACE_HPP
#define VB_KEY_OPT_COEF_INTERFACE_HPP

#include <string>
#include <array>
#include <nlohmann/json.hpp>

#pragma pack(push, 1)
   template <typename Engine>
   struct Coef {
       u_int32_t m;
       u_int32_t c;
       u_int32_t s;
       typename Engine::FrElement coef;
   };
#pragma pack(pop)

/**
 * Abstract base class for coefficient access
 * Provides common interface for both zkey and zkop formats
 */
template<typename Engine>
class CoefStorageInterface {
public:
    virtual ~CoefStorageInterface() = default;

    /**
     * Get coefficient by index
     * @param index The coefficient index (0 to getCoefCount()-1)
     * @return CoefData structure containing m, c, s, and coefficient
     */
    virtual Coef<Engine> get(uint32_t index) const = 0;

    /**
     * Get the total number of coefficients
     */
    virtual uint32_t getCoefCount() const = 0;

    /**
     * Get coefficient count (alternative method name for compatibility)
     */
    uint32_t size() const { return getCoefCount(); }

    /**
     * Check if index is valid
     */
    virtual bool isValidIndex(uint32_t index) const {
        return index < getCoefCount();
    }
};

#endif //VB_KEY_OPT_COEF_INTERFACE_HPP