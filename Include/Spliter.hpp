#ifndef SPILITER_HPP
#define SPILITER_HPP

#include <vector>
#include <fstream>
#include <filesystem>
#include <string_view>

using ChunckFile = std::pair<size_t, size_t>;

// поменять на размер начало, размер

std::vector<ChunckFile> split(std::string_view filePath, uint16_t threadCount) {
    namespace fs = std::filesystem;
    using Path = fs::path;
    auto path = Path(filePath);
    if (!fs::exists(path)) {
        throw std::runtime_error("No file");
    }
    auto size = fs::file_size(path);
    auto chunck = size / threadCount;
    size_t prev = 0;
    size_t next = 0;
    std::ifstream file(filePath.data());
    std::vector<ChunckFile> ans;
    for (uint16_t i = 0; i < threadCount; ++i) {
        if (next < size - chunck) {
            next = prev + chunck;
        } else {
            next = size;
        }
        file.seekg(next);
        // идем до конца строки и проверяем на конец файла
        while (next < size && file.peek() != '\n') {
            next += 1;
            file.ignore(1);
        }
        ChunckFile fChunck = std::make_pair(prev, next);
        ans.push_back(fChunck);
        prev = next;
        if (prev != size) {
            prev += 1;
        }
    }
    return ans;
} 

#endif