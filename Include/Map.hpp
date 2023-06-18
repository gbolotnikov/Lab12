#ifndef MAP_HPP
#define MAP_HPP

#include <list>
#include <string>
#include <fstream>
#include <filesystem>
#include "Spliter.hpp"
#include "ThreadPool.hpp"

class Map {
  
public:
    using Path = std::filesystem::path;


    Map(uint8_t mnum, Path path, std::vector<ChunckFile>&& chuncks): 
    _path(path),
    _threadCount(mnum),
    _threadPool(mnum),
    _flieChuncks(chuncks)
    { }

    std::list<std::string> process(uint8_t prefixCnt) {
        using MapResult = std::future<std::list<std::string>>; 
        std::vector<MapResult> result;
        for (uint8_t i = 0; i < _threadCount; ++i) {
            auto job = [this, i, prefixCnt]()
            {
                return this->tact(_flieChuncks[i], prefixCnt);
            };
            result.push_back(_threadPool.enqueue(job));
        }
        std::list<std::string> ans;
        bool wasExcept = false;
        for (auto& res : result) {
            try {
                ans.merge(res.get());
            } catch(...) {
                wasExcept = true;
            }
        }
        if (wasExcept) {
            throw std::exception();
        }
        return ans;
    }

private:
    Path _path;
    uint8_t _threadCount;
    ThreadPool _threadPool;
    std::vector<ChunckFile> _flieChuncks;

    std::list<std::string> tact(ChunckFile fChunck, size_t prefix) {
        std::string chunck = readFile(fChunck);
        std::istringstream stream(chunck);
        std::list<std::string> list;
        std::string line;
        while (std::getline(stream, line)) {
            if (prefix > line.size()){
                throw std::exception();
            }
            list.push_back(line.substr(0, prefix));
        }
        list.sort();
        return list;
    }

    std::string readFile(ChunckFile fChunck) {
        std::ifstream file(_path);
        file.seekg(fChunck.first);
        std::string str(fChunck.second - fChunck.first, 0);
        file.read(str.data(), fChunck.second - fChunck.first);
        return str;
    }  

};

#endif