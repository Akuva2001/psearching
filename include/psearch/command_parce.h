#pragma once


//#define COMMAND_STRING_PARCING_DEBUG

bool command_string_parcing(const int argc, char *argv[], 
    char **search_string, char **directory_name, bool &name_directory_flag,
        bool &current_directory_flag, int &thread_count, bool &hand_input);