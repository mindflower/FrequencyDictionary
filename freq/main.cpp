#include <dictionary.h>
#include <fileblockmapper.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            std::cout<<"usage: " << argv[0] << " <input> <output>" << std::endl;
            return 0;
        }

        std::cout << "start dictionary generating..." << std::endl;
        const auto start = std::chrono::high_resolution_clock::now();

        auto inputMapper = std::make_unique<dictionary::FileBlockMapper>(argv[1]);
        const auto dict = dictionary::Dictionary{std::move(inputMapper)}.parse();

        //Here also can be used file mapping but this is not really improve performance too much
        std::cout << "dictionary generated. out to file..." << std::endl;
        std::stringstream ss;
        for (const auto& node : dict) {
            ss << node.frequency << ' ' << node.word << '\n';
        }
        std::ofstream os{argv[2]};
        std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>{},
            std::ostreambuf_iterator<char>(os));

        const auto end = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;
        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "done. time passed: " << sec << ':' <<
            std::setfill('0') << std::setw(3) << ms << " sec" << std::endl;

    } catch (const std::exception& ex) {
        std::cout<<"exception occured: " << ex.what() << std::endl;
    }
}
