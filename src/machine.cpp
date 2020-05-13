#include <psearch/machine.h>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



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
            Machine_instance->RUN_FILE(str, Machine_instance->KMP, Machine_instance->quantity_q);
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



Machine::Machine(int **KMP, int quantity_q, int thread_q){
    pthread_mutex_init(&file_names_mutex, NULL);
    pthread_mutex_init(&monitor, NULL);
    this->KMP = KMP;
    this->quantity_q = quantity_q;
    this->thread_q = thread_q;
    threads.resize(thread_q);
    pthread_cond_init(&waiters, NULL);
}
Machine::~Machine(){
    pthread_mutex_destroy(&file_names_mutex);
    pthread_mutex_destroy(&monitor);
    pthread_cond_destroy(&waiters);
    for(auto it : file_names)
        delete[]it;
}
void Machine::run_threads(){
    for(int i=0; i<thread_q; ++i){
        pthread_create(&threads[i], NULL, run_thread, (void *)this);
    }
}
void Machine::print_filenames(){
    pthread_mutex_lock(&file_names_mutex);
    pthread_mutex_lock(&monitor);
    for (auto it : file_names)
        printf("print_filenames:  %s\n", it);
    pthread_mutex_unlock(&monitor);
    pthread_mutex_unlock(&file_names_mutex);
}
void Machine::push_back(char* str){
    pthread_mutex_lock(&file_names_mutex);
    file_names.push_back(str);//you need to delete this str later
    pthread_cond_signal(&waiters);
    pthread_mutex_unlock(&file_names_mutex);
}
int Machine::pop_back(char ** str){
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
void Machine::end_input(){
    pthread_mutex_lock(&file_names_mutex);
    end_in = true;
    pthread_cond_broadcast(&waiters);
    pthread_mutex_unlock(&file_names_mutex);
    run_thread((void *)this);
    for(auto it : threads){
        pthread_join(it, NULL);
    }
}


int Machine::RUN_FILE(char* file_name, int **KMP, int quantity_q){
    int fd;
    if ((fd = open(file_name, O_RDONLY))<0){
        pthread_mutex_lock(&monitor);
        perror("RUN_FILE: open");
        fprintf(stderr, "filename: \"%s\"\n", file_name);
        pthread_mutex_unlock(&monitor);
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
            char* str_save = str;
            for (;*str!='\n' && str!=seof; ++str);++str;
            pthread_mutex_lock(&monitor);
            printf("%s :  string number %d, end symbol number %ld:\n", file_name, p, str_save - (char *) mem);
            write(1, (void*)out_string, str-out_string);
            pthread_mutex_unlock(&monitor);
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


void Machine::start_directory(char * dir_name, int length, bool current_directory_flag, int tab){
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        pthread_mutex_lock(&monitor);
        perror("!!!opendir");
        pthread_mutex_unlock(&monitor);
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
            push_back(new_dir_name);
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
            start_directory(new_dir_name, length+len1+1, current_directory_flag, tab+1);
            #endif
            delete[]new_dir_name;
        }
    }
    closedir(dir);
    return;
}

