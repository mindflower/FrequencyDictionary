#pragma once
#include "block.h"
#include "dictionarycontroller.h"

namespace dictionary {
    class Worker {
    public:
        Worker(DictionaryController& controller, size_t idx);
        void run();

    private:
        size_t processFirstWord(std::string_view block);
        void processWords(const Block& block, size_t startIdx);
        void completeFirstWord(const Block& block);
        void addWord(const std::string& word);
        void addWordPart(const std::string& word, size_t idx);
        void sendWordsToController();

    private:
        DictionaryController& m_controller;
        size_t m_idx = 0;
        std::string m_firstWordLastPart;
        bool m_isSingleWordBlock = false;
        std::unordered_map<std::string, size_t> m_words;
    };
}
