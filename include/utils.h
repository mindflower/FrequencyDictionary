#pragma once
#include <cstddef>

namespace dictionary {
    constexpr size_t GIGABYTE = 1024 * 1024 * 1024;
    size_t divideWithCeil(size_t lhd, size_t rhd);
}
