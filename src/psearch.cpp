#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <psearch/KMP.h>
#include <psearch/machine.h>
#include <psearch/command_parce.h>





int main(const int argc, char *argv[]){
    char *search_string = NULL, *directory_name = NULL;
    bool name_directory_flag = false, current_directory_flag = false;
    int thread_count = 1;
    bool hand_input;
    int code = command_string_parcing(argc, argv, &search_string, &directory_name, name_directory_flag, current_directory_flag, thread_count, hand_input);
    if (code == false){
        return -1;
    }
    if (thread_count == 0){
        printf("ha-ha, stupid\n");
        return 0;
    }
    if (!name_directory_flag){
        directory_name = new char[3];
        strcpy(directory_name, ".");
    }
    
    const int quantity_q = strlen(search_string)+1;
    int *pi = new int[quantity_q - 1];
    prefix_function(search_string, pi, quantity_q - 1);
    int **KMP = new int* [quantity_q];
    for (int i=0; i<quantity_q; ++i)
        KMP[i] = new int[256];
    KMP_build(KMP, search_string, pi, quantity_q);
    
    Machine Machine_instance(KMP, quantity_q, thread_count - 1);
    if (thread_count>1)
        Machine_instance.run_threads();
    Machine_instance.start_directory(directory_name, strlen(directory_name), current_directory_flag, 0);
    Machine_instance.end_input();

    for (int i=0; i<quantity_q; ++i)
        delete[]KMP[i];
    delete[]KMP;
    delete[]pi;
    if (!name_directory_flag)
        delete[]directory_name;
    if (hand_input == true){
        delete[] search_string;
    }
    return 0;
}
