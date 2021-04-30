#ifndef PAGER_H_
#define PAGER_H_

#define PAGE_SIZE 4096

typedef struct {
    long int info_size;   // number of bytes used at the beginning of the file
    char **column_names; // array of strings
    int *column_sizes;
    // column_prefix_sum array for fast retrieval of column position in file?
    long int row_size;   // number of bytes required to store a row
} table_t;

// open & init table
int init_table(char *table_name);

// search for row

// insert new row

// delete row

#endif
