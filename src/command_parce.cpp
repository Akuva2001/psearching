#include <psearch/command_parce.h>
#include <string.h>
#include <stdio.h>
#include <iostream>


bool command_string_parcing(const int argc, char *argv[], 
    char **search_string_path, char ***directory_names, 
        bool &current_directory_flag, int &thread_count){
    
    const char hello[] = 
    "\nHello, welcome to a parallel search programm. Let's start!\n\n"
    "You can run it with that arguments\n"
    "./psearch /path_to_searched_string -t8 -[r/n] /path /to /directories\n"
    "-t means count of threads."
    "\n-r means recursive pass.\n"
    "-n means not recursive pass\n"
    "Example: ./psearch ../Pictures/string.txt -t8 -r ../P* ../O*\n\n";

    if (argc <= 4){
        printf(hello);
        return false;
    }
    #ifdef COMMAND_STRING_PARCING_DEBUG
    for(int i=0; i<argc; ++i)
        std::cout<<i<<' '<<argv[i]<<'\n';
    #endif
    *search_string_path = argv[1];
    thread_count = 0;
    if(argv[2][0]=='-' && argv[2][1]=='t'){
        for(int j=2; argv[2][j]!=0; j++){
            thread_count*=10;
            thread_count+=argv[2][j]-'0';
        }
    } else {
        printf(hello);
        return false;
    }
    if(strcmp(argv[3], "-n") == 0){
        current_directory_flag = true;
    } else if (strcmp(argv[3], "-r") == 0) {
        current_directory_flag = false;
    } else {
        printf(hello);
        return false;
    }
    *directory_names = &argv[4];
    #ifdef COMMAND_STRING_PARCING_DEBUG
    std::cout<<"search_string_path: "<<*search_string_path<<'\n';
    current_directory_flag?std::cout<<"current_directory_flag: true\n":std::cout<<"current_directory_flag: false\n";
    std::cout<<"thread_count: "<<thread_count<<'\n';
    #endif
    return true;
}