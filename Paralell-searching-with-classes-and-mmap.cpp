#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
using namespace std;


//#define COMMAND_STRING_PARCING_DEBUG
//#define KMP_DEBUG
//#define DIRECTORY_DEBUG
//#define RUN_THREAD_DEBUG

class Machine;

void start_directory(char * dir_name, int length, bool current_directory_flag, Machine *Machine_instance, int tab, pthread_mutex_t *monitor);

void* run_thread(void*);


int RUN_FILE(char* file_name, int **KMP, int quantity_q, pthread_mutex_t *monitor){
    int fd;
    if ((fd = open(file_name, O_RDONLY))<0){
        pthread_mutex_lock(monitor);
        perror("RUN_FILE: open");
        pthread_mutex_unlock(monitor);
        return -1;
    }
    struct stat stat_buf;
    int status = stat(file_name, &stat_buf);
    int size = stat_buf.st_size;
    void* mem = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    char *str = (char *) mem, *seof = str+size;
    char* out_string = str;
    int p = 0;
    int q = 0;
    for (; str!=seof; ++str){
        q = KMP[q][(int)(unsigned char)*str];
        if (q == quantity_q - 1){
            for (;*str!='\n' && str!=seof; ++str);++str;
            pthread_mutex_lock(monitor);
            printf("%s :  string number %d:\n", file_name, p);
            write(1, (void*)out_string, str-out_string);
            pthread_mutex_unlock(monitor);
            munmap((void*)str, size);
            close(fd);
            return p;
        }
        if (*str=='\n'){
            ++p;
            out_string = str+1;
        }
    }
    munmap(mem, size);
    close(fd);
    return -1;
}


enum TH_CODES{
    OK = 0,
    WAIT = -1,
    FINISH = -2,
};


class Machine{
public:
    vector<char*> file_names;
    vector<pthread_t> threads;
    pthread_cond_t waiters;
    pthread_mutex_t file_names_mutex;
    pthread_mutex_t monitor;
    bool end_in = false;
    int **KMP;
    int quantity_q;
    int thread_q;


    Machine(int **KMP, int quantity_q, int thread_q){
        pthread_mutex_init(&file_names_mutex, NULL);
        pthread_mutex_init(&monitor, NULL);
        this->KMP = KMP;
        this->quantity_q = quantity_q;
        this->thread_q = thread_q;
        threads.resize(thread_q);
        pthread_cond_init(&waiters, NULL);
    }
    ~Machine(){
        pthread_mutex_destroy(&file_names_mutex);
        pthread_mutex_destroy(&monitor);
        pthread_cond_destroy(&waiters);
        for(auto it : file_names)
            delete[]it;
    }
    void run_threads(){
        for(int i=0; i<thread_q; ++i){
            pthread_create(&threads[i], NULL, run_thread, (void *)this);
        }
    }
    void print_filenames(){
        pthread_mutex_lock(&file_names_mutex);
        pthread_mutex_lock(&monitor);
        for (auto it : file_names)
            printf("print_filenames:  %s\n", it);
        pthread_mutex_unlock(&monitor);
        pthread_mutex_unlock(&file_names_mutex);
    }
    void push_back(char* str){
        pthread_mutex_lock(&file_names_mutex);
        file_names.push_back(str);//you need to delete this str later
        pthread_cond_signal(&waiters);
        pthread_mutex_unlock(&file_names_mutex);
    }
    int pop_back(char ** str){
        int code = OK;
        pthread_mutex_lock(&file_names_mutex);
        if (!file_names.empty()){
            *str = file_names.back();
            file_names.pop_back();
        }
        else if (!end_in)
            code = WAIT;
        else
            code = FINISH;
        pthread_mutex_unlock(&file_names_mutex);
        return code;
    }
    void end_input(){
        pthread_mutex_lock(&file_names_mutex);
        end_in = true;
        pthread_cond_broadcast(&waiters);
        pthread_mutex_unlock(&file_names_mutex);
        run_thread((void *)this);
        for(auto it : threads){
            pthread_join(it, NULL);
        }
    }
};



void* run_thread(void* adress){
    Machine* Machine_instance = (Machine*)adress;
    for(;;){
        char *str;
        int code = Machine_instance->pop_back(&str);
        if(code == OK){
            #ifdef RUN_THREAD_DEBUG
            pthread_mutex_lock(&(Machine_instance->monitor));
            printf("run_thread: filename:  %s\n", str);
            pthread_mutex_unlock(&(Machine_instance->monitor));
            #endif
            RUN_FILE(str, Machine_instance->KMP, Machine_instance->quantity_q, &(Machine_instance->monitor));
            delete[]str;
            continue;
        }
        if(code == WAIT){
            pthread_mutex_lock(&(Machine_instance->file_names_mutex));
            pthread_cond_wait(&(Machine_instance->waiters), &(Machine_instance->file_names_mutex));
            pthread_mutex_unlock(&(Machine_instance->file_names_mutex));
            continue;
        }
        if(code == FINISH){
            break;
        }
    }
    return NULL;
}


bool command_string_parcing(const int argc, char *argv[], 
    char **search_string, char **directory_name, bool &name_directory_flag,
        bool &current_directory_flag, int &thread_count){
    
    #ifdef COMMAND_STRING_PARCING_DEBUG
    for(int i=0; i<argc; ++i)
        cout<<i<<' '<<argv[i]<<'\n';
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
            printf("\nError during parcing command_string\n");
            return false;
        }
    }
    #ifdef COMMAND_STRING_PARCING_DEBUG
    cout<<"search_string: "<<*search_string<<'\n';
    name_directory_flag? cout<<"directory_name: "<<*directory_name<<'\n':cout<<"current directory\n";
    current_directory_flag?cout<<"current_directory_flag: true\n":cout<<"current_directory_flag: false\n";
    cout<<"thread_count: "<<thread_count<<'\n';
    #endif
    return true;
}

void prefix_function (char* s, int *pi, int str_len) {
    pi[0] = 0;
	for (int i=1; s[i]!=0; ++i) {
		int j = pi[i-1];
		while (j > 0 && s[i] != s[j])
			j = pi[j-1];
		if (s[i] == s[j])  ++j;
		pi[i] = j;
    }
    #ifdef KMP_DEBUG
    cout<<"\n";
    for(int i=0; i<str_len+1; ++i)
        cout<<i<<' ';
    cout<<"\n  ";
    for (int i=0; s[i]!=0; ++i) 
        cout<<s[i]<<' ';
    cout<<"\n  ";
    for(int i = 0; i < str_len; ++i)
        cout<<pi[i]<<' ';
    cout<<'\n';
    #endif
}

//Для каждой найденной строки с образцом нужно вывести имя файла, в
//котором она была найдена, её номер и её саму. Не выводите одну и ту же
//строку несколько раз

int KMP_help(int p, const char c, const char* search_string, int * pi){
    if (search_string[p]==c)
        return p+1;
    if (p==0)
        return 0;
    return KMP_help(pi[p-1], c, search_string, pi);
}

void KMP_build(int** KMP, const char* search_string, int *pi, int quantity_q){
    for(int i=0; i<quantity_q; ++i)
        for(int j=0; j<256; ++j)
            KMP[i][j] = KMP_help(i, (const char)j, search_string, pi);
    #ifdef KMP_DEBUG
    printf("\n\n   ");
    for (char i='A'; i<='z'; ++i)
        printf("%c ", i);
    cout<<'\n';
    for(int i=0; i<quantity_q; ++i){
        printf("%d  ", i);
        for(char j='A'; j<='z'; ++j)
            printf("%d ", KMP[i][(int)j]);
        cout<<'\n';
    }
    cout<<'\n';
    #endif
}

void start_directory(char * dir_name, int length, bool current_directory_flag, Machine *Machine_instance, int tab, pthread_mutex_t *monitor){
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        pthread_mutex_lock(monitor);
        perror("!!!opendir");
        pthread_mutex_unlock(monitor);
        return;
    }
    for (auto rd = readdir(dir); rd != NULL; rd = readdir(dir)) {
        if (rd->d_type == DT_REG){
            #ifdef DIRECTORY_DEBUG
            pthread_mutex_lock(monitor);
            printf("!!!");
            for(int i=0;i<tab;++i) printf("  ");
            printf("  %s\n", rd->d_name);
            pthread_mutex_unlock(monitor);
            #endif
            int len1 = strlen(rd->d_name);
            char *new_dir_name = new char[length + len1 + 2];
            strcpy(new_dir_name, dir_name);
            new_dir_name[length]='/';
            new_dir_name[length+1]=0;
            strcat(new_dir_name, rd->d_name);
            Machine_instance->push_back(new_dir_name);
        }
        else if (rd->d_type == DT_DIR){
            
            #ifdef DIRECTORY_DEBUG
            if (strcmp(rd->d_name, ".") == 0 || strcmp(rd->d_name, "..") == 0)
                continue;
            #else
            if (current_directory_flag || strcmp(rd->d_name, ".") == 0 || strcmp(rd->d_name, "..") == 0)
                continue;
            #endif

            int len1 = strlen(rd->d_name);
            char *new_dir_name = new char[length + len1 + 2];
            strcpy(new_dir_name, dir_name);
            new_dir_name[length]='/';
            new_dir_name[length+1]=0;
            strcat(new_dir_name, rd->d_name);
            #ifdef DIRECTORY_DEBUG
            pthread_mutex_lock(monitor);
            printf("!!!");
            for(int i=0;i<tab;++i) printf("  ");
            printf("%s :\n", new_dir_name);
            pthread_mutex_unlock(monitor);
            if (!current_directory_flag){
                start_directory(new_dir_name, length+len1+1, current_directory_flag, Machine_instance, tab+1, monitor);
            }
            #else
            start_directory(new_dir_name, length+len1+1, current_directory_flag, Machine_instance, tab+1, monitor);
            #endif
            delete[]new_dir_name;
        }
    }
    closedir(dir);
    return;
}


int main(const int argc, char *argv[]){
    char *search_string = NULL, *directory_name = NULL;
    bool name_directory_flag = false, current_directory_flag = false;
    int thread_count = 1;
    command_string_parcing(argc, argv, &search_string, &directory_name, name_directory_flag, current_directory_flag, thread_count);
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
    start_directory(directory_name, strlen(directory_name), current_directory_flag, &Machine_instance, 0, &(Machine_instance.monitor));
    Machine_instance.end_input();

    for (int i=0; i<quantity_q; ++i)
        delete[]KMP[i];
    delete[]KMP;
    delete[]pi;
    if (!name_directory_flag)
        delete[]directory_name;
    return 0;
}
