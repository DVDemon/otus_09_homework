#include "config.h"

homework::Config homework::Config::_instance;

homework::Config::Config() : _deep(0),_size(1),_block(128),_alg(Algorithm::crc32){

}

std::ostream& operator<<(std::ostream & os, homework::Config& cfg){
    os << "block:" <<cfg.block() << std::endl;
    os << "size:" <<cfg.size() << std::endl;
    os << "deep:" <<cfg.deep() << std::endl;
    for(auto s: cfg.input_dirs())
        os << " input dir:" << s << std::endl;

    for(auto s: cfg.exclude_dirs())
        os << " exclude dir:" << s << std::endl;

    for(auto s: cfg.masks())
        os << " mask:" << s << std::endl;

    switch (cfg.algorithm())
    {
    case homework::Algorithm::crc32:
        os << "crc32 algorithm" << std::endl;
        break;
    case homework::Algorithm::md5:
        os << "md5 algorithm" << std::endl;
        break;
    };

    return os;
}

homework::Config &homework::Config::instance(){
    return _instance;
}

std::set<std::string> & homework::Config::input_dirs(){
    return _input_dirs;
}

std::set<std::string> & homework::Config::exclude_dirs(){
    return _exclude_dirs;
}

std::set<std::string> & homework::Config::masks(){
    return _masks;
}

size_t & homework::Config::deep(){
    return _deep;
}

size_t & homework::Config::size(){
    return _size;
}

size_t & homework::Config::block(){
    return _block;
}

homework::Algorithm & homework::Config::algorithm(){
    return _alg;
}
