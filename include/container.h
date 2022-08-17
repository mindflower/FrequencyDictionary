#pragma once
#include "node.h"
#include <unordered_map>
#include <vector>
#include <future>
#include <mutex>

namespace dictionary {
    class Container {
    public:
        void addWords(const std::unordered_map<std::string, size_t>& words);
        void addWordPartForBlock(std::string_view wordPart, size_t blockId);
        std::string getWordPartForBlock(size_t blockId);
        std::vector<Node> createDictionary() const;

    private:
        std::unordered_map<std::string, size_t> m_words;
        std::unordered_map<size_t, std::promise<std::string>> m_wordsParts;
        std::mutex m_wordsPartsMutex;
        mutable std::mutex m_wordsMutex;
    };
}
