#include "processor.h"
#include "config.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <exception>
#include <iostream>
#include <fstream>

namespace fs = boost::filesystem;

#define UNUSED(value) (void)value
#define DEBUG 

#ifdef DEBUG

#define TRACE(value) std::cout << value << std::endl
#else
#define TRACE(value)
#endif

/**
 * Основной цикл обработки. Используем стэк на deque для хранения очереди задач. Аналог обхода дерева директорий "в ширину".
 */
const std::map<std::string,homework::ProcessedItem >& homework::Processor::process(const std::set<std::string>& input){

    for(auto s: input)   tasks.emplace_back(Item{s,0});
    
    
    while(!tasks.empty()){
        Item item = tasks[0];
        tasks.pop_front();
        process(item);
    }

    return processed_files;
}

/**
 * Обработка директории состоит из двух частей. Директории помещаем в очередь задач, а файлы сравниваем с уже загруженными.
 */
void homework::Processor::process(homework::Item& item){
    std::vector<std::string> files;
    std::vector<std::string> sub_dirs;

    get_files_and_dirs(item.dir,files,sub_dirs);  
    if(item.deep<Config::instance().deep()){
        for(auto s: sub_dirs) {
            if(homework::Config::instance().exclude_dirs().find(s)==std::end(homework::Config::instance().exclude_dirs()))
                tasks.push_back(Item{s,item.deep+1});
        }
    }

    for(auto s: files) process_file(s);

}

/**
 * Загружаем список директорий и файлов
 */
void homework::Processor::get_files_and_dirs(std::string& dir,std::vector<std::string>& files,std::vector<std::string>& dirs){
if (fs::exists(dir))
    {
      if (fs::is_regular_file(dir)) throw std::logic_error("Input is not a directory");
      if (fs::is_directory(dir)){
        for (fs::directory_entry& x : fs::directory_iterator(dir)){
                if(fs::is_directory(x.path())) dirs.push_back(x.path().string());
                if(fs::is_regular_file(x.path())) {
                    std::string filename = x.path().string();
                    if(check_masks(filename))
                    if(check_size(filename))
                        files.push_back(filename);
                }
        }       
      }    
    } else throw std::logic_error("Input is not a directory");
}


/**
 * Сравниваем файлы с теми что уже обработали
 */
void homework::Processor::process_file(std::string& file){
    bool inserted = false;
    ProcessedItem current_item {file,std::set<std::string>(),std::vector<std::string>()};
    for(auto it = processed_files.begin();it!=processed_files.end();++it){    
        if(compare_files(current_item,it->second)){
            it->second.files.insert(file);
            inserted = true;
            break;
        }
    }

    if(!inserted){
        processed_files.insert(std::make_pair(file,current_item));
        processed_files[file].files.insert(file);
    }

}

/**
 * Поблочное сравнение файлов. Блоки грузятся лениво
 */
bool homework::Processor::compare_files(ProcessedItem &lhv, ProcessedItem &rhv){
    bool result{true};
    if(lhv.get_file_size()!=rhv.get_file_size()) result = false;
     else {
         for(size_t i=0;result&&(i<lhv.get_max_block_count());i++){           
               result = (lhv.get_block(i) == rhv.get_block(i));
        }
     }

    return result;
}

/**
 * Если размер файла меньше параметра - то не проверяем
 */
 bool homework::Processor::check_size(const std::string & filename){
     return fs::file_size(filename)>=Config::instance().size();
 }

/**
 * Фильтруем файлы
 */
 bool homework::Processor::check_masks(const std::string & filename){
     for(std::string mask : Config::instance().masks()){
         std::string filter_str{"^"};
         filter_str+=mask;
         boost::replace_all(filter_str,".","[.]");
         boost::replace_all(filter_str,"*",".*");
         boost::replace_all(filter_str,"?",".");
         filter_str+="$";
         const boost::regex filter(filter_str);
         const fs::path file_path(filename);
         boost::smatch what;

         if(boost::regex_search(file_path.filename().string(),what,filter,boost::match_extra)) return true;
     }
     return false;
 }

/**
 * Считываем файл с диска
 */
size_t      homework::ProcessedItem::get_file_size(){
    return fs::file_size(filename);
}

/**
 * Вычисление максимального размера файла в блоках
 */
size_t      homework::ProcessedItem::get_max_block_count(){
    return (get_file_size()+(Config::instance().block()-1))/Config::instance().block();
}

/**
 * Умный механизм загрузки блоков с диска
 */
std::string homework::ProcessedItem::get_block(size_t index){
    // Если блок есть в кэше - то его возвращаем
    if(index<blocks.size()) {
        return blocks[index];
    }

    // если блока нет - то его считываем
    std::ifstream in_file(filename);
     if(in_file.is_open()){
        size_t position = blocks.size()*Config::instance().block();
        size_t target   = (index+1)*Config::instance().block();

        // пропускаем считанные блоки
        in_file.ignore(position);

        unsigned char *block = nullptr; 
        unsigned char byte{};
        size_t index_in_block{};

        while ((in_file >> byte) && (position < target)) {
            if(index_in_block >= Config::instance().block()) {
                std::string val = block_to_string(block);
                blocks.push_back(val);
                index_in_block = 0;
                block = nullptr;
            }
            if(index_in_block == 0) block = new unsigned char[Config::instance().block()];
            block[index_in_block] =  byte;
            index_in_block ++;         
            position ++;
        }

        //если блок не полный, то добавляем его
        if(index_in_block>0){
            while(index_in_block<Config::instance().block()) block[index_in_block++]=0;
            std::string val = block_to_string(block);
            blocks.push_back(val);
        }

        in_file.close();
     }

     if(index<blocks.size()) return blocks[index];

     // если вылезли за границу файла - то генерируем исключение
     throw std::logic_error("Requested block out of array bounds");
}

/**
 * Преобразуем массив с данными в хэш в виде строки
 */
std::string homework::ProcessedItem::block_to_string(const unsigned char* block){
    std::string result;

    switch(Config::instance().algorithm()){
        case Algorithm::md5:
            {
                boost::uuids::detail::md5 hash;
                boost::uuids::detail::md5::digest_type digest;
                hash.process_bytes(block, Config::instance().block());
                hash.get_digest(digest);
                const auto charDigest = reinterpret_cast<const char *>(&digest);
                boost::algorithm::hex(charDigest, charDigest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(result));
                break;
            }
        case Algorithm::crc32:
            {
                boost::crc_32_type crc;
                crc.process_bytes(block, Config::instance().block());
                result = std::to_string(crc.checksum());
                break;
            }
    }
    return result;
}