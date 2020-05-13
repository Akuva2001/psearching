#include <psearch/command_parce.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

bool command_string_parcing(const int argc, char *argv[], 
    char **search_string, char **directory_name, bool &name_directory_flag,
        bool &current_directory_flag, int &thread_count, bool &hand_input){
    
    if (argc <= 1){
        for (;;){
            printf("Hello, welcome to a parallel search programm. Let's start!\n");
            printf("You can run it with that arguments\n ./psearch searched_string [-t5] [/path_to_dir]\n-t means count of threads, 1 by default.\n");
            printf("Or you can type arguments down. Quit?\n[yes/no]>>>");
            char answer[100];
            for (;;){ 
                scanf("%s", answer);
                if (strcmp(answer, "yes") == 0){
                    return -1;
                }
                else if (strcmp(answer, "no") == 0){
                    break;
                }
                else
                    printf("[yes/no]>>>");
            }
            printf("type search string and press enter\n>>>");
            *search_string = new char[1024];
            scanf("%s", *search_string);
            printf("type streams nunber\n>>>");
            scanf("%d", &thread_count);
            printf("type directory path\n>>>");
            name_directory_flag = false;
            *directory_name = new char[1024];
            scanf("%s", *directory_name);
            printf("Do you want to enable recursive direction pass?\n[yes/no]>>>");
            for (;;){ 
                scanf("%s", answer);
                if (strcmp(answer, "yes") == 0){
                    current_directory_flag = false;
                    break;
                }
                else if (strcmp(answer, "no") == 0){
                    current_directory_flag = true;
                    break;
                }
                else
                    printf("[yes/no]>>>");
            }
            printf("Confirm settings?\n \"yes\" to confirm, \"no\" to try again, \"quit\" to quit programm\n[yes/no/quit]>>>");
            for (;;){ 
                scanf("%s", answer);
                if (strcmp(answer, "yes") == 0){
                    hand_input = true;
                    #ifdef COMMAND_STRING_PARCING_DEBUG
                    std::cout<<"search_string: "<<*search_string<<'\n';
                    name_directory_flag? std::cout<<"directory_name: "<<*directory_name<<'\n':std::cout<<"current directory\n";
                    current_directory_flag?std::cout<<"current_directory_flag: true\n":std::cout<<"current_directory_flag: false\n";
                    std::cout<<"thread_count: "<<thread_count<<'\n';
                    #endif
                    
                    return true;
                }
                else if (strcmp(answer, "no") == 0){
                    break;
                }
                else if (strcmp(answer, "quit") == 0){
                    return false;
                }
                else
                    printf("[yes/no/qit]>>>");
            }
        }


    }

    #ifdef COMMAND_STRING_PARCING_DEBUG
    for(int i=0; i<argc; ++i)
        std::cout<<i<<' '<<argv[i]<<'\n';
    #endif
    int p=0;
    for (int i=1; i<argc; ++i){
        if(strcmp(argv[i], "-n") == 0){
            current_directory_flag = true;
        }
        else if(argv[i][0]=='-' && argv[i][1]=='t'){
            thread_count = 0;
            for(int j=2; argv[i][j]!=0; j++){
                thread_count*=10;
                thread_count+=argv[i][j]-'0';
            }
        }
        else if (p==0){
            *search_string = argv[i];
            ++p;
        }
        else if (p==1){
            *directory_name = argv[i];
            name_directory_flag = true;
            ++p;
        }
        else{
            fprintf(stderr, "\nError during parcing command_string\n");
            return false;
        }
    }
    #ifdef COMMAND_STRING_PARCING_DEBUG
    std::cout<<"search_string: "<<*search_string<<'\n';
    name_directory_flag? std::cout<<"directory_name: "<<*directory_name<<'\n':std::cout<<"current directory\n";
    current_directory_flag?std::cout<<"current_directory_flag: true\n":std::cout<<"current_directory_flag: false\n";
    std::cout<<"thread_count: "<<thread_count<<'\n';
    #endif
    return true;
}
