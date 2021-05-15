#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "pager.h"

// TODO: check for buffer overflows

/*
 * Creates a new table object
 */
table_t *table_alloc(int columns) {
    table_t *temp = malloc(sizeof *temp);
    if (temp == NULL)
        return NULL;

    temp->columns = columns;
    temp->row_size = 0;

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

/*
 * Frees dynamically allocated memory under table struct
 */
void free_table(table_t *table) {
    for (int i = 0; i < table->columns; i++)
        free(table->column_names[i]);
    free(table->column_names);

    free(table->column_types);
    free(table->column_sizes);

    free(table);
}
/*
 * Reads data from table file and returns table struct after initializing it
 */
table_t *init_table(FILE *fin) {
    char info[MAX_INFO_SIZE]; // turn into dynamically allocated vector
    fread(info, sizeof *info, MAX_INFO_SIZE - 1, fin);
    info[MAX_INFO_SIZE - 1] = '\0';

    fclose(fin);

    int columns = 0;

    char *p;
    for (p = info; *p != '\n' && p-info < MAX_INFO_SIZE; p += 1)
        if (*p == ',') columns++;

    table_t *table = table_alloc(columns);
    table->info_size = p-info+1;
    // printf("table info size: %zu\n", table->info_size);

    // test
    // printf("Table (%d columns):\n", columns);

    int current_column = 0;
    p = info;
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
        table->row_size += table->column_sizes[current_column];

        /*
        printf("Column %d: %s, type %c, size %zu\n",
                  current_column,
                  table->column_names[current_column],
                  table->column_types[current_column],
                  table->column_sizes[current_column]);
        */

        current_column++;
    }

    printf("init_table done\n");
    return table;
}

/*
 * Helper function that reads into a page buffer from a table file
 */
int get_page(char *page_buffer, FILE *table, size_t offset) {
    fseek(table, offset, SEEK_SET);
    size_t temp = fread(page_buffer, sizeof(*page_buffer), PAGE_SIZE, table);

    if (temp != PAGE_SIZE) return 0;
    return 1;
}

/*
 * Function that writes an empty page at the end of the table file
 */
void append_page(FILE *fin, table_t *table) {
    fseek(fin, 0, SEEK_END);
    char new_page[PAGE_SIZE];
    memset(new_page, 0, PAGE_SIZE);

    *(int *)new_page = sizeof (int);
    printf("%d\n", *(int *)new_page);

    size_t temp = fwrite(new_page, sizeof *new_page, PAGE_SIZE, fin);
    //printf("Append succesfully wrote %zu chars\n", temp);
}

/*
 * Inserts new row into page
 */
void insert_row_in_page(char *row, table_t *table, size_t offset, FILE *fin) {
    char page_buffer[PAGE_SIZE];
    memset(page_buffer, 0, PAGE_SIZE);
    int id = (*(int *)row);

    get_page(page_buffer, fin, offset);
    int page_pos = *(int *)page_buffer;
    //printf("inserting row at position %d in page\n", page_pos);
    *(int *)page_buffer = page_pos+(int)table->row_size;

    memcpy(page_buffer+page_pos,
           row, table->row_size * sizeof *row);

    fseek(fin, offset, SEEK_SET);
    fwrite(page_buffer, sizeof *page_buffer, PAGE_SIZE, fin);
}


void insert_row(char *row, table_t *table, FILE *fin) {
    size_t offset = table->info_size;
    char page_buffer[PAGE_SIZE];
    memset(page_buffer, 0, PAGE_SIZE);

    while (1) {
        if (!get_page(page_buffer, fin, offset)) {
            append_page(fin, table);
            insert_row_in_page(row, table, offset, fin);
            break;
        }

        int page_pos = *(int *)page_buffer;
        if (page_pos+table->row_size < PAGE_SIZE) {
            insert_row_in_page(row, table, offset, fin);
            break;
        }
        offset += PAGE_SIZE;
    }
}


cursor_t *init_cursor(void) {
    cursor_t *cursor = malloc(sizeof *cursor);
    if (cursor == NULL)
        return NULL;

    cursor->EOT = 0;
    cursor->page_num = 0;
    cursor->row_num = 0;

    return cursor;
}

// FIND OCCURRENCE
void find_next_occurrence(FILE *fin, table_t *table, cursor_t *cursor,  char *column_name, void *value) {
    long int offset = table->info_size;
    offset += PAGE_SIZE * cursor->page_num;

    char page_buffer[PAGE_SIZE];
    memset(page_buffer, 0, PAGE_SIZE);

    char type;
    size_t column_size;
    size_t col_pos = 0;

    for (int i = 0; i < table->columns; i++) {
        if (strcmp(column_name, table->column_names[i]) == 0) {
            type = table->column_types[i];
            column_size = table->column_sizes[i];
            break;
        }
        col_pos += table->column_sizes[i];
    }

    int page_num = 1;
    while (1) {
        if (!get_page(page_buffer, fin, offset)) {
            cursor->EOT = 1;
            break;
        }

        int max_pos = *(int *)page_buffer;
        for (size_t row_pos = sizeof(int) + cursor->row_num * table->row_size;
                    row_pos < max_pos;
                    row_pos += table->row_size) {

            if (type == 'i') {
                int val;
                memcpy(&val, page_buffer+row_pos+col_pos, sizeof val);

                if (val == *(int *)value)
                    return;
            }
            else if (type == 'f') {
                float val;
                memcpy(&val, page_buffer+row_pos+col_pos, sizeof val);

                if (val == *(float *)value)
                    return;
            }
            else {
                char *val = malloc((sizeof *val) * (column_size+1));
                memcpy(val, page_buffer + row_pos + col_pos, column_size);
                val[column_size] = '\0';

                if (strcmp(val, (char *)value) == 0)
                    return;

                free(val);
            }

            cursor->row_num++;
        }

        cursor->page_num++;
        cursor->row_num = 0;
        offset += PAGE_SIZE;
    }
}


void print_row(FILE *fin, table_t *table, const int *print_rows, cursor_t *cursor) {
    long int offset = table->info_size;
    offset += PAGE_SIZE * cursor->page_num;

    char page_buffer[PAGE_SIZE];
    memset(page_buffer, 0, PAGE_SIZE);

    get_page(page_buffer, fin, offset);

    size_t row_pos = sizeof(int) + cursor->row_num * table->row_size;

    size_t col_pos = 0;
    for (int i = 0; i < table->columns; i++) {
        if (!print_rows[i]) {
            col_pos += table->column_sizes[i];
            continue;
        }

        if (table->column_types[i] == 'i') {
            int val;
            memcpy(&val, page_buffer+row_pos+col_pos, sizeof val);

            printf("%d, ", val);
        }
        else if (table->column_types[i] == 'f') {
            float val;
            memcpy(&val, page_buffer+row_pos+col_pos, sizeof val);

            printf("%f, ", val);
        }
        else {
            char *val = malloc((sizeof *val) * (table->column_sizes[i]+1));
            memcpy(val, page_buffer + row_pos + col_pos, table->column_sizes[i]);
            val[table->column_sizes[i]] = '\0';

            printf("%s, ", val);

            free(val);
        }

        col_pos += table->column_sizes[i];
    }

    printf("\n");
}


void move_cursor_left(table_t *table, cursor_t *cursor) {
    size_t row_pos = sizeof(int) + table->row_size*cursor->row_num;
    if (row_pos + table->row_size >= PAGE_SIZE) {
        cursor->page_num++;
        cursor->row_num = 0;
        return;
    }
    cursor->row_num++;
}

/*
 * print_row[i] = 1 if i-th row is selected, 0 otherwise
 */
void select_rows(table_t *table, FILE *fin, int *print_rows, char *col, void *val) {
    cursor_t *cursor = init_cursor();

    for (find_next_occurrence(fin, table, cursor, col, val);
            cursor->EOT == 0;
            find_next_occurrence(fin, table, cursor, col, val)) {
        print_row(fin, table, print_rows, cursor);

        move_cursor_left(table, cursor);
    }

    free(cursor);
}

void delete_from_page(table_t *table, FILE *fin, cursor_t *cursor) {
    char page_buffer[PAGE_SIZE];
    size_t offset = table->info_size;

    offset += cursor->page_num * PAGE_SIZE;

    get_page(page_buffer, fin, offset);

    int last_row;
    memcpy(&last_row, page_buffer, sizeof last_row);

    printf("ROW SIZE: %d\n", table->row_size);

    offset = sizeof(int) + cursor->row_num * table->row_size;
    printf("Starting from %d, moving rows up until i reach %d\n", offset, last_row);

    while (offset < last_row) {
        printf("checking at %d\n", offset+2*table->row_size);
        if (offset+2*table->row_size > PAGE_SIZE)
            memset(page_buffer+offset, 0, PAGE_SIZE-offset);

        memcpy(page_buffer+offset, page_buffer+offset+table->row_size, table->row_size);

        offset += table->row_size;
    }

    offset = table->info_size + cursor->page_num*PAGE_SIZE;

    last_row -= table->row_size;
    memcpy(page_buffer, &last_row, sizeof last_row);

    fseek(fin, offset, SEEK_SET);
    fwrite(page_buffer, sizeof *page_buffer, PAGE_SIZE, fin);
}

void delete_row(table_t *table, FILE *fin, char *col, void *val) {
    cursor_t *cursor = init_cursor();

    for (find_next_occurrence(fin, table, cursor, col, val);
            cursor->EOT == 0;
            find_next_occurrence(fin, table, cursor, col, val)) {
        printf("Found a row to delete\n");
        delete_from_page(table, fin, cursor);
    }

    free(cursor);
}