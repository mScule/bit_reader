#include "str.h"
#include <stdlib.h>

int str_length(char *str) {

    int i = 0;

    while(str[i] != '\0')
        i++;
    return i;
}

int str_equals(char *str1, char *str2) {

    int
        str1_len = str_length(str1),
        str2_len = str_length(str2);

    if(str1_len == str2_len) {

        for(int i = 0; i < str1_len; i++)
            if(str1[i] != str2[i])
                return 0;
        return 1;
    }

    else
        return 0;
}

char *str_new(char *str) {

    int str_len = str_length(str);

    char *new = (char*) malloc(sizeof(char) * str_len);

    for(int i = 0; i < str_len; i++)
        new[i] = str[i];

    new[str_len] = '\0';

    return new;
}

char *str_combine(char *str1, char *str2) {

    int
        str1_len = str_length(str1),
        str2_len = str_length(str2),
        new_len  = str1_len + str2_len + 1;

    char *new = (char*) malloc(sizeof(char) * new_len);

    int i1 = 0, i2;

    for(i2 = 0; i2 < str1_len; i2++) {
        new[i1] = str1[i2];
        i1++;
    }

    for(i2 = 0; i2 < str2_len; i2++) {
        new[i1] = str2[i2];
        i1++;
    }

    new[new_len - 1] = '\0';

    free(str1);

    return new;
}
