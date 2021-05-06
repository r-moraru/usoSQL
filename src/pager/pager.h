#ifndef PAGER_H_
#define PAGER_H_

// TODO: page_size dinamic (se adapteaza la dimensiunile liniilor)
#define PAGE_SIZE 4096
#define MAX_INFO_SIZE 1024
#define COLNAME_SIZE 100

typedef struct {
    int columns;
    size_t info_size;   // number of bytes used at the beginning of the file
    char **column_names;  // array of strings
    char *column_types;
    size_t *column_sizes;
    // column_prefix_sum array for fast retrieval of column position in file?
    size_t row_size;    // number of bytes required to store a row
} table_t;

// open & init table
table_t *init_table(FILE *fin);
void free_table(table_t *table);

// TODO: search for rows that satisfy a condition
//       Use a callback for those rows

void insert_row(char *row, table_t *table, FILE *fin);

// TODO: delete row (function used as callback for search)

// TODO: update row (function used as callback for search)

#endif
