#include <worker.h>
#include <cctype>

namespace dictionary {
    Worker::Worker(Controller& controller) :
        m_controller(controller) {
        m_accumulator.reserve(512);
    }

    void Worker::run() {
        while (const auto oBlock = m_controller.getNextBlock()) {
            const auto& block = oBlock.value();
            const auto offset = processFirstWord(block.block);
            processWords(block, offset);
            completeFirstWord(block);
        }
        sendWordsToController();
    }

    size_t Worker::processFirstWord(std::string_view block) {
        const auto offset = read(block, 0);
        if (!m_accumulator.empty()) {
            m_firstWordLastPart = m_accumulator;
        }
        else {
            m_firstWordLastPart.clear();
        }
        m_isSingleWordBlock = m_firstWordLastPart.size() == block.size();
        return offset;
    }

    void Worker::processWords(const Block& block, const size_t offset) {
         if (m_isSingleWordBlock) {
            return;
        }
        for (auto end = read(block.block, offset);
            end < block.block.size(); end = read(block.block, end + 1)) {
            addWord(m_accumulator);
        }
        if (block.isLastBlock) {
            addWord(m_accumulator);
        }
        else {
            addWordPart(m_accumulator, block.idx + 1);
        }
    }

    void Worker::completeFirstWord(const Block& block) {
        const auto addWordMethod = [this, &block](const std::string& word) {
            if (m_isSingleWordBlock && !block.isLastBlock) {
                return addWordPart(word, block.idx + 1);
            }
            if (!word.empty()) {
                return addWord(word);
            }
        };
        if (block.idx != 0) {
            const auto wordPart = m_controller.getWordPartForBlock(block.idx);
            addWordMethod(wordPart + m_firstWordLastPart);
        } else {
            addWordMethod(m_firstWordLastPart);
        }
    }

    size_t Worker::read(std::string_view block, const size_t offset) {
        if (!m_accumulator.empty()) {
            m_accumulator.clear();
        }
        auto end = offset;
        for (const auto c : block.substr(offset)) {
            if (std::isalpha(c)) {
                m_accumulator += static_cast<unsigned char>(std::tolower(c));
                ++end;
            }
            else {
                break;
            }
        }
        return end;
    }

    void Worker::addWord(const std::string& word) {
        if (!word.empty()){
            ++m_words[word];
        }
    }

    void Worker::addWordPart(const std::string& word, size_t idx) {
        m_controller.addWordPartForBlock(word, idx);
    }

    void Worker::sendWordsToController() {
        m_controller.addWords(m_words);
    }
}
