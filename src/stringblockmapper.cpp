#include <stringblockmapper.h>
#include <utils.h>

namespace dictionary {
    StringBlockMapper::StringBlockMapper(std::string_view string, const size_t blockSize) :
        m_string(std::string(string)), m_blockSize(blockSize){
    }

    size_t StringBlockMapper::remap(const size_t blockId){
        m_currentBlockSize = std::min(getMapSize(), m_blockSize);
        return divideWithCeil(getMapSize(), getBlockSize());
    }

    const char* StringBlockMapper::getRawPtr() const{
        return m_string.c_str();
    }

    size_t StringBlockMapper::getSize() const {
        return m_string.size();
    }

    size_t StringBlockMapper::getMapSize() const {
        return m_string.size();
    }

    size_t StringBlockMapper::getPageSize() const {
        return -1;
    }

    size_t StringBlockMapper::getBlockSize() const {
        return m_currentBlockSize;
    }

    size_t StringBlockMapper::getBlocksCount() const {
        return divideWithCeil(getSize(), m_blockSize);
    }
}
