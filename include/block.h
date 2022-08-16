#pragma once
#include <string_view>

namespace dictionary {
    struct Block {
        std::string_view block;
        size_t idx = 0;
        bool isLastBlock = false;
    };
}