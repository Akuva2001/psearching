#include <psearch/KMP.h>
#include <stdio.h>


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
