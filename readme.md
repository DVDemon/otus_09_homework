# Bayan

Программа для поиска дубликатов файлов по списку директорий. Программа ищет файлы с одинаковым содержимым и выводит их в виде групп на экран. Каждое имя файла расположено на отдельной строке. Группы разделены пустой строкой.



# Запуск программы
```sh
bayan options
```

# Возможные опции

```sh
 -h [ --help ]             This screen
 -i [ --input ] arg        input directories
 -e [ --exclude ] arg      exclude directories directories
 -d [ --deep ] arg (=0)    how deep should we scan
 -s [ --size ] arg (=1)    minimum file size in bytes
 -m [ --mask ] arg         file masks
 -b [ --block ] arg (=128) block size for comparison
 --crc32                   Use crc32 algorithm to compare blocks
 --md5                     Use md5 algorithm to compare blocks
```

По умолчанию минимальный размер файла для сравнения равен 1 байту, файлы ищутся только в указанных директориях, размер блока равен 128 байтам, а алгоритм хэширования (для сравнения) - crc32

> Программа хранит хэши для прочитанных файлов в памяти, что, теоретически накладывает ограничения на количество сравниваемых файлов. 

