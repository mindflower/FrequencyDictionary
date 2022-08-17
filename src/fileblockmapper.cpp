#include <fileblockmapper.h>
#include <utils.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

namespace dictionary {
    FileBlockMapper::FileBlockMapper(std::string_view fileName, const size_t blockSize) :
        m_fileName(fileName), m_blockSize(blockSize), m_pageSize(sysconf(_SC_PAGESIZE)) {
        m_descriptor = open(m_fileName.c_str(), O_RDONLY, 0);
        if(m_descriptor < 0) {
            throw std::runtime_error("open failed for file: " + m_fileName + "; error: " + strerror(errno));
        }

        struct stat st = {};
        if(fstat(m_descriptor, &st) < 0) {
            const auto err = strerror(errno);
            close(m_descriptor);
            throw std::runtime_error("fstat failed for file: " + m_fileName + "; error: " + err);
        }
        m_fileSize = static_cast<size_t>(st.st_size);
    }

    FileBlockMapper::~FileBlockMapper() {
        munmap(m_data, m_mapSize);
        close(m_descriptor);
        m_data = MAP_FAILED;
        m_descriptor = -1;
    }

    size_t FileBlockMapper::remap(const size_t blockId) {
        munmap(m_data, m_mapSize);
        m_data = MAP_FAILED;

        const auto blockOffset = blockId * m_blockSize;
        if (blockOffset > m_fileSize){
            throw std::runtime_error("block " + std::to_string(blockId) + " is of file bounds");
        }
        const auto pageId = blockOffset / m_pageSize;
        const auto pageOffset = pageId * m_pageSize;
        m_mapSize = std::min(GIGABYTE, m_fileSize - pageOffset);
        m_data = mmap(nullptr, m_mapSize, PROT_READ, MAP_PRIVATE, m_descriptor, pageOffset);
        if(m_data == MAP_FAILED) {
            throw std::runtime_error("mmap failed for file: " + m_fileName + "; error: " + strerror(errno));
        }

        m_currentBlockSize = std::min(m_mapSize, m_blockSize);
        return divideWithCeil(m_mapSize, m_currentBlockSize);
    }

    const char* FileBlockMapper::getRawPtr() const {
        return static_cast<char*>(m_data);
    }

    size_t FileBlockMapper::getSize() const {
        return m_fileSize;
    }

    size_t FileBlockMapper::getMapSize() const {
        return m_mapSize;
    }

    size_t FileBlockMapper::getPageSize() const {
        return m_pageSize;
    }

    size_t FileBlockMapper::getBlockSize() const {
        return m_currentBlockSize;
    }

    size_t FileBlockMapper::getBlocksCount() const {
        return divideWithCeil(m_fileSize, m_blockSize);
    }
}
