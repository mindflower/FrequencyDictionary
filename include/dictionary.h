#pragma once
#include "dictionarycontroller.h"
#include "worker.h"
#include <thread>

namespace dictionary {
    class Dictionary {
    public:
        Dictionary(std::unique_ptr<Mapper>&& stream, size_t workersCount = 8);
        std::vector<Node> parse();

    private:
        void createWorkers();
        void joinWorkers();
        std::vector<Node> createResult() const;

    private:
        DictionaryController m_controller;
        size_t m_workersCount = 1;
        std::vector<Worker> m_workers;
        std::vector<std::thread> m_workerThreads;
    };
}
