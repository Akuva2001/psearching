# psearching

Эта программа производит параллельный поиск в указанных директориях образца, к которому имеет доступ из файла. Для поиска используется КМП автомат. Выходом программы является список путей и файлов, в которых обнаружились совпадения, также номер строки и абсолютный номер в файле байта конца совпадающей последовательности, а также строку, на которую пришелся конец шаблона. Строки актуальнее для текстовых файлов, прочие файлы их могут не содержать и выводятся достаточно неприемлимым образом.

Программа достаточно устойчива, но может падать, если файлы в процессе работы постоянно изменяются. Так, если запустить поиск по кэшу Google Chrome, то велика вероятность поймать Segmentation fault, т.к. кэш файлы постоянно переписываются, создаются и изменяются и удаляются в процессе чтения.

Используются pthread. Для ускорения доступа к файлам используется mmap. Для ускорения операций вывода используется write.

На пустом входе Вы столкнетесь с приветствием

Hello, welcome to a parallel search programm. Let's start!

You can run it with that arguments
./psearch /path_to_searched_string -t8 -[r/n] /path /to /directories
-t means count of threads.
-r means recursive pass.
-n means not recursive pass
Example: ./psearch ../Pictures/string.txt -t8 -r ../P* ../O*


Имеется возможность рекурсивного обхода директорий и просмотра файлов только в указанных папаках

Запустите make для сборки проекта.