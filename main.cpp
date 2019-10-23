#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "config.h"
#include "processor.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

auto main(int argc, char *argv[]) -> int
{
    try
    {
        po::options_description desc{"Options"};
        desc.add_options()("help,h", "This screen")("input,i", po::value<std::vector<std::string>>()->multitoken(), "input directories")("exclude,e", po::value<std::vector<std::string>>()->multitoken(), "exclude directories directories")("deep,d", po::value<size_t>()->default_value(0), "how deep should we scan")("size,s", po::value<size_t>()->default_value(1), "minimum file size in bytes")("mask,m", po::value<std::vector<std::string>>()->multitoken(), "file masks")("block,b", po::value<size_t>()->default_value(128), "block size for comparison")("crc32", "Use crc32 algorithm to compare blocks")("md5", "Use md5 algorithm to compare blocks");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
            std::cout << desc << '\n';

        if (vm.count("input"))
        {
            for (auto s : vm["input"].as<std::vector<std::string>>())
            {
                if (fs::exists(s))
                {
                    fs::path p = fs::canonical(s);
                    homework::Config::instance().input_dirs().insert(p.string());
                }
            }
        }

        if (vm.count("exclude"))
        {
            for (auto s : vm["exclude"].as<std::vector<std::string>>())
            {
                if (fs::exists(s))
                {
                    fs::path p = fs::canonical(s);
                    homework::Config::instance().exclude_dirs().insert(p.string());
                }
            }
        }

        if (vm.count("mask"))
        {
            for (auto s : vm["mask"].as<std::vector<std::string>>())
                homework::Config::instance().masks().insert(s);
        }



        if (vm.count("deep"))
        {
            homework::Config::instance().deep() = vm["deep"].as<size_t>();
        }

        if (vm.count("size"))
        {
            homework::Config::instance().size() = vm["size"].as<size_t>();
        }

        if (vm.count("block"))
        {
            homework::Config::instance().block() = vm["block"].as<size_t>();
        }

        if (vm.count("crc32"))
        {
            homework::Config::instance().algorithm() = homework::Algorithm::crc32;
        }
        else if (vm.count("md5"))
        {
            homework::Config::instance().algorithm() = homework::Algorithm::md5;
        }

        if (homework::Config::instance().input_dirs().empty())
        {
            std::cout << "You should specify at list one input dir" << std::endl;
            return 1;
        }

        // ищем дубликаты и сразу выводим результат
        homework::Processor processor;
        for (auto bulk : processor.process(homework::Config::instance().input_dirs()))
        {
            // в результате сожержаться все данные, для дубликатов размер массива больше 1, для уникальных = 1
            if (bulk.second.files.size() > 1) 
            {
                for (auto str : bulk.second.files)
                {
                    std::cout << str << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
