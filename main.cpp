// #include "controller.h"

#include <boost/program_options.hpp>

#include <iostream>
#include "Include/Spliter.hpp"
#include "Include/Map.hpp"
#include "Include/Reduce.hpp"

struct Context
{
    std::string path;
    uint16_t mnum;
    uint16_t rnum;
};

namespace po = boost::program_options;
Context parse_command_options(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help",                                       "Print this message")
        ("path",  po::value<std::string>()->required(), "File path")
        ("mnum", po::value<uint16_t>()->required(), "Map thread count")
        ("rnum", po::value<uint16_t>()->required(), "Reduce thread count");

    po::variables_map vm;
    try {
        po::store(parse_command_line(argc, argv, desc), vm);
        if (vm.count("help") != 0) {
            std::cout << desc << "\n";
            throw;
        }
        po::notify(vm);
    }
    catch (const po::error& error) {
        std::cerr << "Error while parsing command-line arguments: "
                << error.what() << "\nPlease use --help to see help message\n";
        throw;
    }

    std::string path = vm["path"].as<std::string>();
    uint16_t mnum = vm["mnum"].as<uint16_t>();
    uint16_t rnum = vm["rnum"].as<uint16_t>();

    return Context{path, mnum, rnum};
}

int main(int argc, char* argv[]) {
     try {
        auto context = parse_command_options(argc, argv);
        auto ans = split(context.path, context.mnum);
        Map map(context.mnum, context.path, std::move(ans));
        Reduce reduce(context.rnum);
        uint16_t prefix = 0;
        while(true) {
            try {
                auto mapRes = map.process(++prefix);
                if (reduce.process(std::move(mapRes))) {
                    break;
                }
            } catch(...) {
                std::cout << "Non prefix" << std::endl;
                return 1;
            }    
        }
        std::cout << "Prefix " << prefix << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
