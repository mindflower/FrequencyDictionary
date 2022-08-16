#include <dictionary.h>

namespace dictionary {
    Dictionary::Dictionary(std::unique_ptr<Mapper>&& mapper, const size_t workersCount) :
        m_controller(std::move(mapper), workersCount), m_workersCount(workersCount) {
    }

    std::vector<Node> Dictionary::parse() {
        createWorkers();
        joinWorkers();
        return createResult();
    }

    void Dictionary::createWorkers() {
        for (size_t idx = 0; idx < m_workersCount; ++idx) {
            m_workers.emplace_back(m_controller, idx);
        }

        for (const auto& worker : m_workers) {
            m_workerThreads.emplace_back(&Worker::run, worker);
        }
    }

    void Dictionary::joinWorkers() {
        for (auto& workerThread : m_workerThreads) {
            if (workerThread.joinable()) {
                workerThread.join();
            }
        }
    }

    std::vector<Node> Dictionary::createResult() const {
        return m_controller.createResult();
    }
}
