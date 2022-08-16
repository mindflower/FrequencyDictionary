#include <stringmapper.h>
#include <utils.h>

namespace dictionary {
    StringMapper::StringMapper(std::string_view string, const size_t blockSize) :
        m_string(std::string(string)), m_blockSize(blockSize){
    }
    
    size_t StringMapper::remap(const size_t blockId){
        m_currentBlockSize = std::min(getMapSize(), m_blockSize);
        return divideWithCeil(getMapSize(), getBlockSize());
    }

    const char* StringMapper::getRawPtr() const{
        return m_string.c_str();
    }

    size_t StringMapper::getSize() const {
        return m_string.size();
    }

    size_t StringMapper::getMapSize() const {
        return m_string.size();
    }

    size_t StringMapper::getPageSize() const {
        return -1;
    }

    size_t StringMapper::getBlockSize() const {
        return m_currentBlockSize;
    }

    size_t StringMapper::getBlocksCount() const {
        return divideWithCeil(getSize(), m_blockSize);
    }
}
