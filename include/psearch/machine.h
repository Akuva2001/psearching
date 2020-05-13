#pragma once
#include <vector>
#include <pthread.h>


//#define DIRECTORY_DEBUG
//#define RUN_THREAD_DEBUG

enum TH_CODES{
    OK = 0,
    WAIT = -1,
    FINISH = -2,
};

void* run_thread(void* adress);


class Machine{
public:
    std::vector<char*> file_names;
    std::vector<pthread_t> threads;
    pthread_cond_t waiters;
    pthread_mutex_t file_names_mutex;
    pthread_mutex_t monitor;
    bool end_in = false;
    int **KMP;
    int quantity_q;
    int thread_q;


    Machine(int **KMP, int quantity_q, int thread_q);
    ~Machine();
    void run_threads();
    void print_filenames();
    void push_back(char* str);
    int pop_back(char ** str);
    void start_directory(char * dir_name, int length, bool current_directory_flag, int tab);
    void end_input();
    int RUN_FILE(char* file_name, int **KMP, int quantity_q);
};