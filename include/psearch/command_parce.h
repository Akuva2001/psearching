#pragma once


#define COMMAND_STRING_PARCING_DEBUG

bool command_string_parcing(const int argc, char *argv[], 
    char **search_string_path, char ***directory_names, 
        bool &current_directory_flag, int &thread_count);