#pragma once
#include "mapper.h"
#include "node.h"
#include "block.h"
#include <unordered_map>
#include <mutex>
#include <vector>
#include <future>
#include <optional>

namespace dictionary {
    class DictionaryController {
    public:
        DictionaryController(std::unique_ptr<Mapper>&& mapper, size_t workersCount);
        std::optional<Block> getNextBlock();
        void addWordPartForBlock(std::string_view block, size_t idx);
        void addWords(std::unordered_map<std::string, size_t>& words);
        std::string getWordPartForBlock(size_t idx);
        std::vector<Node> createResult() const;

    private:
        struct BlockIdLimits {
            size_t lower = 0;
            size_t upper = 0;
        };

    private:
        Block getBlockById(size_t id) const;
        void waitUntilNextBlockReady();

    private:
        std::unique_ptr<Mapper> m_mapper;
        size_t m_workersCount = 0;
        std::atomic_size_t m_currentBlockId = 0;
        std::atomic_size_t m_waitingThreadCount = 0;
        BlockIdLimits m_blockIdLimits;
        std::mutex m_waitMutex;
        std::condition_variable m_waitCv;

        std::unordered_map<std::string, size_t> m_words;
        std::unordered_map<size_t, std::promise<std::string>> m_wordsParts;
        std::mutex m_wordsPartsMutex;
        mutable std::mutex m_wordsMutex;
    };
}
