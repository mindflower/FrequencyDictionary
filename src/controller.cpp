#include <controller.h>

namespace dictionary {
    Controller::Controller(std::unique_ptr<BlockMapper>&& mapper, const size_t workersCount) :
        m_mapper(std::move(mapper)), m_workersCount(workersCount) {
        m_blockIdLimits.upper = m_mapper->remap(0);
    }

    std::optional<Block> Controller::getNextBlock() {
        const auto currentBlockId = m_currentBlockId++;
        if (currentBlockId < m_mapper->getBlocksCount()) {
            if (currentBlockId >= m_blockIdLimits.upper) {
                waitUntilNextBlockReady();
            }
            return getBlockById(currentBlockId);
        }
        return std::nullopt;
    }

    Block Controller::getBlockById(const size_t id) const {
        const auto blockSize = m_mapper->getBlockSize();
        const auto pageSize = m_mapper->getPageSize();
        const auto blockOffset = blockSize * id;
        const auto pageOffset = blockOffset % pageSize;
        const auto pageId = blockOffset / pageSize;
        const auto pageFullSize = pageId * pageSize;
        const auto ptrOffset = pageFullSize + pageOffset;

        if (ptrOffset <= m_mapper->getMapSize()) {
            const auto remain = std::min(blockSize, m_mapper->getSize() - ptrOffset);
            const auto isLastBlock = ptrOffset + remain >= m_mapper->getSize();
            return {{m_mapper->getRawPtr() + ptrOffset, remain}, id, isLastBlock};
        }
        throw std::runtime_error("input file offset out of bounds");
    }

    void Controller::waitUntilNextBlockReady() {
        if (++m_waitingThreadCount == m_workersCount) {
            m_blockIdLimits.lower = m_blockIdLimits.upper;
            m_blockIdLimits.upper += m_mapper->remap(m_blockIdLimits.upper);
            m_waitingThreadCount = 0;
            m_waitCv.notify_all();
            return;
        }
        std::unique_lock lock{m_waitMutex};
        m_waitCv.wait(lock);
    }

    void Controller::addWords(const std::unordered_map<std::string, size_t>& words) {
        m_container.addWords(words);
    }

    void Controller::addWordPartForBlock(std::string_view block, const size_t blockId) {
        m_container.addWordPartForBlock(block, blockId);
    }

    std::string Controller::getWordPartForBlock(const size_t blockId) {
        return m_container.getWordPartForBlock(blockId);
    }

    std::vector<Node> Controller::createDictionary() const {
        return m_container.createDictionary();
    }
}
