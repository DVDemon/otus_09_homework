#ifndef H_CONFIG

#include <set>
#include <string>
#include <ostream>

namespace homework{

    enum class Algorithm{
        crc32=0,
        md5=1
    };

    class Config{
        private:
            static Config _instance;
            Config();

            std::set<std::string> _input_dirs;
            std::set<std::string> _exclude_dirs;
            std::set<std::string> _masks;

            size_t _deep;
            size_t _size;
            size_t _block;

            Algorithm _alg;

        public:
            static Config& instance();

            std::set<std::string> & input_dirs();
            std::set<std::string> & exclude_dirs();
            std::set<std::string> & masks();

            size_t & deep();
            size_t & size();
            size_t & block();

            Algorithm & algorithm();

    };



}

std::ostream& operator<<(std::ostream &, homework::Config&);
#endif