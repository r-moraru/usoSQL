#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pager.h"


#define INFO_SIZE 1024
#define COLNAME_SIZE 100

// TODO: create read_page function

// creates a new table object
table_t *table_alloc(int columns) {
    table_t *temp = malloc(sizeof *temp);
    if (temp == NULL)
        return NULL;

    temp->columns = columns;

    temp->column_names = malloc(columns * sizeof *(temp->column_names));
    if (temp->column_names == NULL) {
        free(temp);
        return NULL;
    }

    for (int i = 0; i < columns; i++)
        temp->column_names[i] = malloc(COLNAME_SIZE * sizeof(char));

    temp->column_types = malloc(columns * sizeof *(temp->column_types));
    if (temp->column_types == NULL) {
        for (int i = 0; i < columns; i++)
            free(temp->column_names[i]);
        free(temp->column_names);

        free(temp);
        return NULL;
    }

    temp->column_sizes = malloc(columns * sizeof *(temp->column_sizes));
    if (temp->column_sizes == NULL) {
        for (int i = 0; i < columns; i++)
            free(temp->column_names[i]);
        free(temp->column_names);

        free(temp->column_types);
        free(temp);
        return NULL;
    }

    temp->row_size = 0;

    return temp;
}

void free_table(table_t *table) {
    for (int i = 0; i < table->columns; i++)
        free(table->column_names[i]);
    free(table->column_names);

    free(table->column_types);
    free(table->column_sizes);

    free(table);
}

table_t *init_table(char *table_name) {
    strcat(table_name, ".db");
    FILE *fin = fopen(table_name, "rb");
    if (fin == NULL) {
        printf("Table \"%s\" does not exist.\n", table_name);
        return NULL;
    }

    char info[INFO_SIZE]; // turn into dynamically allocated vector
    fread(info, sizeof *info, INFO_SIZE-1, fin);
    info[INFO_SIZE-1] = '\0';

    fclose(fin);

    int columns = 0;

    for (char *p = info; *p != '\n' && p-info < INFO_SIZE; p += 1)
        if (*p == ',') columns++;

    table_t *table = table_alloc(columns);
    table->info_size = strchr(info, '\n')-info+1;

    // test
    printf("Table %s (%d columns):\n", table_name, columns);

    int current_column = 0;
    char *p = info;
    while (current_column < columns) {
        sscanf(p, "%s %c", table->column_names[current_column],
                           &(table->column_types[current_column]));

        p += strlen(table->column_names[current_column])+2*sizeof(char);

        if (table->column_types[current_column] == 'c') {
            int arr_size;
            memcpy(&arr_size, p, sizeof(arr_size));
            table->column_sizes[current_column] = sizeof(char)*arr_size;
        } else if (table->column_types[current_column] == 'i') {
            table->column_sizes[current_column] = sizeof(int);
        } else {
            table->column_sizes[current_column] = sizeof(float);
        }

        p += sizeof(char);

        printf("Column %d: %s, type %c, size %d\n",
                  current_column,
                  table->column_names[current_column],
                  table->column_types[current_column],
                  table->column_sizes[current_column]);

        current_column++;
    }

    return table;
}
