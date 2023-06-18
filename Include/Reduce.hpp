#ifndef REDUCE_HPP
#define REDUCE_HPP

#include <list>
#include <string>
#include <fstream>
#include <filesystem>
#include "Spliter.hpp"
#include "ThreadPool.hpp"

class Reduce {
using It = std::list<std::string>::iterator;
public:

    Reduce(uint8_t rnum): 
    _threadCount(rnum),
    _threadPool(rnum)
    { }

    bool process(std::list<std::string>&& chuncks) {
        _mapChanks = std::move(chuncks);
        std::vector<std::pair<It, It>> reduceStore; 
        size_t step = _mapChanks.size() / _threadCount;
        It prev = _mapChanks.begin();
        It next = _mapChanks.begin();

        for (uint16_t i = 0; i < _threadCount; ++i) {
            if (std::distance(prev, _mapChanks.end()) < step) {
                next = _mapChanks.end();
            } else {
                std::advance(next, step);
            }
            while (next != _mapChanks.end() && *std::next(next) == *next) {
                ++next;
            }
            if (next == _mapChanks.end()){
                auto chunck = std::make_pair(prev, next);
                reduceStore.push_back(chunck);
                break;
            }
            auto chunck = std::make_pair(prev, next);
            reduceStore.push_back(chunck);
            prev = std::next(next);
        }

        using ReduceResult = std::future<bool>; 
        std::vector<ReduceResult> result;

        for (uint8_t i = 0; i < _threadCount; ++i) {
            auto job = [this, chunck = reduceStore[i]]()
            {
                return this->tact(chunck.first, chunck.second);
            };
            result.push_back(_threadPool.enqueue(job));
        }

        bool out = true;
        for (auto& res : result) {
            out &= res.get();
        }
        return out;
    }

private:
    uint8_t _threadCount;
    ThreadPool _threadPool;
    std::list<std::string> _mapChanks;
    

    bool tact(It begin, It end) {
        for (auto it = begin; it != end; ++it) {
            if (*it == *std::next(it)) {
                return false;
            }
        }
        return true;
    }

    void prepare() {

    }

};

#endif