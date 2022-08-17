#pragma once
#include "blockmapper.h"
#include "node.h"
#include "block.h"
#include "container.h"
#include <optional>

namespace dictionary {
    class Controller {
    public:
        Controller(std::unique_ptr<BlockMapper>&& mapper, size_t workersCount);
        std::optional<Block> getNextBlock();

        void addWordPartForBlock(std::string_view block, size_t blockId);
        void addWords(const std::unordered_map<std::string, size_t>& words);
        std::string getWordPartForBlock(size_t blockId);
        std::vector<Node> createDictionary() const;

    private:
        struct BlockIdLimits {
            size_t lower = 0;
            size_t upper = 0;
        };

    private:
        Block getBlockById(size_t id) const;
        void waitUntilNextBlockReady();

    private:
        Container m_container;
        std::unique_ptr<BlockMapper> m_mapper;
        size_t m_workersCount = 0;
        std::atomic_size_t m_currentBlockId = 0;
        std::atomic_size_t m_waitingThreadCount = 0;
        BlockIdLimits m_blockIdLimits;
        std::mutex m_waitMutex;
        std::condition_variable m_waitCv;
    };
}
