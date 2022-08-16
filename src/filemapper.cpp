#include <filemapper.h>
#include <utils.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

namespace dictionary {
    FileMapper::FileMapper(std::string_view fileName, const size_t blockSize) :
        m_fileName(fileName), m_blockSize(blockSize), m_pageSize(sysconf(_SC_PAGESIZE)) {
        m_descriptor = open(m_fileName.c_str(), O_RDONLY, 0);
        if(m_descriptor < 0) {
            throw std::runtime_error("open failed for file: " + m_fileName + "; error:" + strerror(errno));
        }

        struct stat st = {};
        if(fstat(m_descriptor, &st) < 0) {
            const auto err = strerror(errno);
            close(m_descriptor);
            throw std::runtime_error("fstat failed for file: " + m_fileName + "; error:" + err);
        }
        m_fileSize = static_cast<size_t>(st.st_size);
    }

    FileMapper::~FileMapper() {
        munmap(m_data, m_mapSize);
        close(m_descriptor);
        m_data = MAP_FAILED;
        m_descriptor = -1;
    }
    
    size_t FileMapper::remap(const size_t blockId) { 
        munmap(m_data, m_mapSize);
        m_data = MAP_FAILED;
        
        const auto blockOffset = blockId * m_blockSize;
        if (blockOffset > m_fileSize){
            throw std::runtime_error("block " + std::to_string(blockId) + " of of file bounds");
        }
        const auto pageId = blockOffset / m_pageSize;
        const auto pageOffset = pageId * m_pageSize;
        m_mapSize = std::min(GIGABYTE, m_fileSize - pageOffset);
        m_data = mmap(nullptr, m_mapSize, PROT_READ, MAP_PRIVATE, m_descriptor, pageOffset);
        if(m_data == MAP_FAILED) {
            throw std::runtime_error("mmap failed for file: " + m_fileName + "; error:" + strerror(errno));
        }

        m_currentBlockSize = std::min(m_mapSize, m_blockSize);
        return divideWithCeil(m_mapSize, m_currentBlockSize);
        
    }

    const char* FileMapper::getRawPtr() const{
        return static_cast<char*>(m_data);
    }

    size_t FileMapper::getSize() const {
        return m_fileSize;
    }

    size_t FileMapper::getMapSize() const {
        return m_mapSize;
    }

    size_t FileMapper::getPageSize() const {
        return m_pageSize;
    }

    size_t FileMapper::getBlockSize() const {
        return m_currentBlockSize;
    }

    size_t FileMapper::getBlocksCount() const {
        return divideWithCeil(m_fileSize, m_blockSize);
    }
}
