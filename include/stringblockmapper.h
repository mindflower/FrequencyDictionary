#pragma once
#include "blockmapper.h"
#include <string>

namespace dictionary
{
    //Helper class for unit testing
    class StringBlockMapper : public BlockMapper {
    public:
        StringBlockMapper(std::string_view string, size_t blockSize = 4);
        size_t remap(size_t blockId) override;
        const char* getRawPtr() const override;
        size_t getSize() const override;
        size_t getMapSize() const override;
        size_t getPageSize() const override;
        size_t getBlockSize() const override;
        size_t getBlocksCount() const override;

    private:
        std::string m_string;
        size_t m_blockSize = 0;
        size_t m_currentBlockSize = 0;
    };
}
