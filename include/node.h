#pragma once
#include <string>

namespace dictionary {
    struct Node {
        std::string word;
        size_t frequency = 0;
    };
}
