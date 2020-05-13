#pragma once


//#define KMP_DEBUG


void prefix_function (char* s, int *pi, int str_len);

int KMP_help(int p, const char c, const char* search_string, int * pi);

void KMP_build(int** KMP, const char* search_string, int *pi, int quantity_q);
