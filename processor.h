#ifndef H_PROCESSOR

#include <string>
#include <deque>
#include <set>
#include <map>
#include <vector>
#include <memory>

namespace homework{

    /**
     * Вспомогательная структура для рекурсивного обхода дерева файлов
     */
    struct Item{
        //  директория для обработки
        std::string dir;
        // текущая глубина директории
        size_t      deep;
    };


    /**
     * Результат сравнения файлов
     */
    struct ProcessedItem{
        // имя "главного" файла
        std::string                 filename;
        // файлы в группе обязательных
        std::set<std::string>       files;
        // кэш блоков для группы
        std::vector<std::string>     blocks;

        // размер файла
        size_t      get_file_size();

        // максимальное количество блоков в файле
        size_t      get_max_block_count();

        // чтение блока из файла или кеша
        std::string get_block(size_t index);

        // конвертация массива в строку с хэшированием
        std::string block_to_string(const unsigned char*);
    };

    class Processor{
        private:
            std::deque<Item>          tasks;
            std::map<std::string,ProcessedItem > processed_files;


            void process(Item&);
            void get_files_and_dirs(std::string& dir,std::vector<std::string>& files,std::vector<std::string>& dirs);
            void process_file(std::string& input);
            //void load_file_from_disk(const std::string& file_name, std::vector<std::string> & blocks);
            bool compare_files(ProcessedItem &lhv, ProcessedItem &rhv);
            
            bool check_masks(const std::string & filename);
            bool check_size(const std::string & filename);
            
        public:
            const std::map<std::string,ProcessedItem >& process(const std::set<std::string>&);
    };
}

#endif