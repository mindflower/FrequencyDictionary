#include <utils.h>
#include <cmath>

namespace dictionary {
    size_t divideWithCeil(const size_t lhd, const size_t rhd) {
        const auto result = static_cast<double>(lhd) / rhd;
        return static_cast<size_t>(std::ceil(result));
    }
}
