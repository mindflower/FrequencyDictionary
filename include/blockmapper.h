#pragma once
#include <cstddef>

namespace dictionary
{
    class BlockMapper {
    public:
        virtual ~BlockMapper() = default;
        virtual size_t remap(size_t blockId) = 0;
        virtual const char* getRawPtr() const = 0;
        virtual size_t getSize() const = 0;
        virtual size_t getMapSize() const = 0;
        virtual size_t getPageSize() const = 0;
        virtual size_t getBlockSize() const = 0;
        virtual size_t getBlocksCount() const = 0;
    };
}
