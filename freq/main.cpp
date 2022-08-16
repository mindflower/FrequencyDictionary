#include <dictionary.h>
#include <filemapper.h>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            std::cout<<"usage: " << argv[0] << " <input> <output>" << std::endl;
            return 0;
        }

        std::cout << "start dictionary generating..." << std::endl;
        const auto start = std::chrono::high_resolution_clock::now();
        
        auto inputMapper = std::make_unique<dictionary::FileMapper>(argv[1]);
        const auto dict = dictionary::Dictionary{std::move(inputMapper)}.parse();
        
        std::cout << "dictionary generated. out to file..." << std::endl;
        std::ofstream ss{argv[2]};
        for (const auto& node : dict) {
            ss << node.frequency << ' ' << node.word << '\n';
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;
        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "done. time passed: " << sec << ':' << ms << " sec" << std::endl;

    } catch (const std::exception& ex) {
        std::cout<<"exception occured:" << ex.what() << std::endl;
    }
}
