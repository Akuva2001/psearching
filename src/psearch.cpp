#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <psearch/KMP.h>
#include <psearch/machine.h>
#include <psearch/command_parce.h>
#include <dirent.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>






int main(const int argc, char *argv[]){
    char *search_string_path = NULL, *directory_name = NULL;
    bool name_directory_flag = false, current_directory_flag = false;
    int thread_count = 1;
    bool hand_input;
    char ** directory_names;
    int code = command_string_parcing(argc, argv, &search_string_path, &directory_names, current_directory_flag, thread_count);
    if (code == false){
        return -1;
    }
    if (thread_count == 0){
        printf("ha-ha, stupid\n");
        return 0;
    }

    //extract search string
    int fd;
    if ((fd = open(search_string_path, O_RDONLY))<0){
        perror("open search string: open");
        return -1;
    }
    struct stat stat_buf;
    int status = stat(search_string_path, &stat_buf);
    int size = stat_buf.st_size;
    void* mem = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    char *search_string = (char *) mem;
    //write(1, search_string, size);
    //return 0;

    
    const int quantity_q = size+1;
    int *pi = new int[quantity_q - 1];
    prefix_function(search_string, pi, quantity_q - 1);
    int **KMP = new int* [quantity_q];
    for (int i=0; i<quantity_q; ++i)
        KMP[i] = new int[256];
    KMP_build(KMP, search_string, pi, quantity_q);
    
    Machine Machine_instance(KMP, quantity_q, thread_count - 1);
    if (thread_count>1)
        Machine_instance.run_threads();
    for (int i=0; directory_names[i] != NULL; ++i) {
        Machine_instance.start_directory(directory_names[i], strlen(directory_names[i]), current_directory_flag, 0);
    }
    Machine_instance.end_input();

    for (int i=0; i<quantity_q; ++i)
        delete[]KMP[i];
    delete[]KMP;
    delete[]pi;
    if (!name_directory_flag)
        delete[]directory_name;
    if (hand_input == true){
        delete[] search_string_path;
    }

    munmap(mem, size);
    close(fd);
    return 0;
}
