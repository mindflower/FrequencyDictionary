#pragma once
#include "blockmapper.h"
#include <sys/mman.h>
#include <string>

namespace dictionary {
    class FileBlockMapper : public BlockMapper {
    public:
        FileBlockMapper(std::string_view fileName, size_t blockSize = 1048576);
        ~FileBlockMapper();
        virtual size_t remap(size_t blockId);
        virtual const char* getRawPtr() const;
        virtual size_t getSize() const;
        virtual size_t getMapSize() const;
        virtual size_t getPageSize() const;
        virtual size_t getBlockSize() const;
        virtual size_t getBlocksCount() const;

    private:
        std::string m_fileName;
        int m_descriptor = -1;
        void* m_data = MAP_FAILED;
        size_t m_fileSize = 0;
        size_t m_blockSize = 0;
        size_t m_pageSize = 0;
        size_t m_mapSize = 0;
        size_t m_currentBlockSize = 0;
    };
}
