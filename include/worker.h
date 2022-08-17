#pragma once
#include "block.h"
#include "controller.h"

namespace dictionary {
    class Worker {
    public:
        Worker(Controller& controller);
        void run();

    private:
        size_t processFirstWord(std::string_view block);
        void processWords(const Block& block, size_t offset);
        void completeFirstWord(const Block& block);
        size_t read(std::string_view block, size_t offset);
        void addWord(const std::string& word);
        void addWordPart(const std::string& word, size_t idx);
        void sendWordsToController();

    private:
        Controller& m_controller;
        std::string m_firstWordLastPart;
        bool m_isSingleWordBlock = false;
        std::unordered_map<std::string, size_t> m_words;
        std::string m_accumulator;
    };
}
