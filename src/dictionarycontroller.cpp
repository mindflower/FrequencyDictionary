#include <dictionarycontroller.h>
#include <algorithm>
#include <execution>
#include <iterator>

namespace dictionary {
    DictionaryController::DictionaryController(std::unique_ptr<Mapper>&& mapper, const size_t workersCount) : 
        m_mapper(std::move(mapper)), m_workersCount(workersCount) {
        m_blockIdLimits.upper = m_mapper->remap(0);
    }

    std::optional<Block> DictionaryController::getNextBlock() {
        const auto currentBlockId = m_currentBlockId++;
        if (currentBlockId < m_mapper->getBlocksCount()) {
            if (currentBlockId >= m_blockIdLimits.upper) {
                waitUntilNextBlockReady();
            }
            return getBlockById(currentBlockId);
        }
        return std::nullopt;
    }

    Block DictionaryController::getBlockById(const size_t id) const {
        const auto blockSize = m_mapper->getBlockSize();
        const auto pageSize = m_mapper->getPageSize();
        const auto blockOffset = blockSize * id;
        const auto pageOffset = blockOffset % pageSize;
        const auto pageId = blockOffset / pageSize;
        const auto pageFullSize = pageId * pageSize;
        const auto ptrOffset = pageFullSize + pageOffset;
        if (ptrOffset <= m_mapper->getMapSize()) {
            const auto isLastBlock = ptrOffset + (std::min(blockSize, m_mapper->getSize() - (ptrOffset))) >= m_mapper->getSize();
            return {{m_mapper->getRawPtr() + ptrOffset, std::min(blockSize, m_mapper->getSize() - (ptrOffset))}, id, isLastBlock};
        }
        throw std::runtime_error("input file offset out of bounds");
    }

    void DictionaryController::waitUntilNextBlockReady() {
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

    void DictionaryController::addWords(std::unordered_map<std::string, size_t>& words) {
        std::unique_lock lock{m_wordsMutex};
        for (const auto& [word, freq] : words) {
            m_words[word] += freq;
        }
    }

    void DictionaryController::addWordPartForBlock(std::string_view block, const size_t idx) {
        std::unique_lock lock{m_wordsPartsMutex};
        m_wordsParts[idx].set_value(std::string{block});
    }

    std::string DictionaryController::getWordPartForBlock(const size_t idx) {
        std::unique_lock lock{m_wordsPartsMutex};
        auto future = m_wordsParts[idx].get_future();
        lock.unlock();
        return future.get();
    }

    std::vector<Node> DictionaryController::createResult() const {
        std::unique_lock lock{m_wordsMutex};
        std::vector<Node> result;
        result.reserve(m_words.size());
        std::transform(
            m_words.cbegin(), 
            m_words.cend(),
            std::back_inserter(result),
            [](const auto& word){return Node{word.first, word.second};} 
        );
        std::sort(std::execution::par, result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.frequency == rhs.frequency) {
                return lhs.word < rhs.word;
            }
            return lhs.frequency > rhs.frequency;
        });
        return result;
    }
}
