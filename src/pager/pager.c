#include <stdio.h>
#include <stdlib.h>
#include "pager.h"

#define INFO_SIZE 1024

// TODO: create read_page function

// creates a new table object
table_t *table_alloc(int columns) {
    table_t *temp = malloc(sizeof *temp);
    if (temp == NULL)
        return NULL;

    temp->column_names = malloc(columns * sizeof *(temp->column_names));
    if (temp->column_names == NULL) {
        free(temp);
        return NULL;
    }

    temp->column_sizes = malloc(columns * sizeof *(temp->column_sizes));
    if (temp->column_sizes == NULL) {
        free(temp->column_names);
        free(temp);
        return NULL;
    }

    return temp;
}

table_t *init_table(char *table_name) {
    FILE *fin = fopen(table_name, "rb");
    if (fin == NULL) {
        printf("Table \"%s\" does not exist.\n", table_name);
        return NULL;
    }

    int info_len = INFO_SIZE;
    char *info = malloc(INFO_SIZE * sizeof *info);
    if (info == NULL) {
        fclose(fin);
        return NULL;
    }

    // read until \n
    for (fread(info+info_len-INFO_SIZE, sizeof *info, INFO_SIZE, fin);
          strchr(info, "\n") == NULL;
          fread(info+info_len-INFO_SIZE, sizeof *info, INFO_SIZE, fin)) {

        char *temp = realloc(info, info_len + INFO_SIZE);
        if (temp == NULL) {
            free(info);
            fclose(fin);
            return NULL;
        }
        info = temp;
        info_len += INFO_SIZE;
    }

    int columns = 0;

    for (char *p = info; *p != '\n'; p += 1)
        if (*p == ',') columns++;

    table_t *table = table_alloc(columns);

    for (char *p = info; p != NULL; p = strchr(p, ',')+1) {
        
    }

    free(info);
    fclose(fin);
    return ;
}
