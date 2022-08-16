#include <worker.h>
#include <cctype>
#include <sstream>

namespace dictionary {
    Worker::Worker(DictionaryController& controller, const size_t idx) :
        m_controller(controller), m_idx(idx) {
    }

    void Worker::run() {
        while (const auto oBlock = m_controller.getNextBlock()) {
            const auto& block = oBlock.value();
            const auto nextWordIdx = processFirstWord(block.block);
            processWords(block, nextWordIdx);
            completeFirstWord(block);
        }
        sendWordsToController();
    }

    size_t Worker::processFirstWord(std::string_view block) {
        m_firstWordLastPart.clear();
        m_isSingleWordBlock = false;
        if (!block.empty() && !std::isalpha(block[0])) {
            return 0;
        }

        std::stringstream ss;
        size_t idx = 0;
        for (const auto c : block) {
            if (std::isalpha(c)) {
                ss << static_cast<unsigned char>(std::tolower(c));
            } else {
                m_firstWordLastPart = ss.str();
                return idx;
            }
            ++idx;
        }
        m_isSingleWordBlock = true;
        m_firstWordLastPart = ss.str();
        return idx;
    }

    void Worker::processWords(const Block& block, const size_t startIdx) {
        std::stringstream key;
        for (const auto c : block.block.substr(startIdx)) {
            if (std::isalpha(c)) {
                key << static_cast<unsigned char>(std::tolower(c));
            } else {
                if (key.rdbuf()->in_avail()) {
                    addWord(key.str());
                    key.str(std::string{});
                }
            }
        }
        if (!m_isSingleWordBlock) {
            if (block.isLastBlock && key.rdbuf()->in_avail()) {
                addWord(key.str());
            }
            else {
                addWordPart(key.str(), block.idx + 1);
            }
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

    void Worker::addWord(const std::string& word) {
        ++m_words[word];
    }

    void Worker::addWordPart(const std::string& word, size_t idx) {
        m_controller.addWordPartForBlock(word, idx);
    }

    void Worker::sendWordsToController() {
        m_controller.addWords(m_words);
    }
}
