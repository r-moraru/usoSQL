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

typedef struct {
    int EOT;
    long int page_num;
    long int row_num;
} cursor_t;

// open & init table
table_t *init_table(FILE *fin);
void free_table(table_t *table);

// TODO: search for rows that satisfy a condition
// INIT CURSOR
//cursor_t *init_cursor(void);

// Pentru inceput, functia de cautare accepta doar o pereche coloana-valoare
//void find_next_occurrence(FILE *fin, table_t *table, cursor_t *cursor, char *column_name, void *value);

void insert_row(char *row, table_t *table, FILE *fin);

void select_rows(table_t *table, FILE *fin, int *print_row, char *col, void *val);

// TODO: delete row (function used as callback for search)
void delete_row(table_t *table, FILE *fin, char *col, void *val);

// TODO: update row (function used as callback for search)

#endif
