#include "container.h"
#include <algorithm>
#include <execution>

namespace dictionary {
    void Container::addWords(const std::unordered_map<std::string, size_t>& words) {
        std::unique_lock lock{m_wordsMutex};
        for (const auto& [word, freq] : words) {
            m_words[word] += freq;
        }
    }

    void Container::addWordPartForBlock(std::string_view block, const size_t idx) {
        std::unique_lock lock{m_wordsPartsMutex};
        m_wordsParts[idx].set_value(std::string{block});
    }

    std::string Container::getWordPartForBlock(const size_t idx) {
        std::unique_lock lock{m_wordsPartsMutex};
        auto future = m_wordsParts[idx].get_future();
        lock.unlock();
        return future.get();
    }

    std::vector<Node> Container::createDictionary() const {
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
