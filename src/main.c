#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

#ifdef __unix__
    #include <unistd.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/types.h>
#endif

#include "pager/pager.h"
#include "queue/queue.h"

void help(){
    printf("Open README.md");
}

void create() {
    char check[10], table[100];
    scanf("%s %s", check, table);

    strcat(table, ".db");
    if(strcmp(check, "TABLE") != 0){
        help();
    } else {
        // Check if table already exists. If it does, quit
        if( access(table, F_OK ) == 0 ) {
            printf("Table %s already exists.", table);
            exit(0);
        }

        FILE *db = fopen(table, "wb");
        char coloana[100], tip[100];

        while(tip[strlen(tip)-1] != ';'){
            scanf("%s", coloana);
            scanf("%s", tip);

            printf("%s %s\n", coloana, tip);

            fwrite(coloana, sizeof *coloana, strlen(coloana), db);
            fwrite(" ", 1, 1, db);
            fwrite(tip, sizeof *tip, 1, db);

            if(tip[0] == 'c'){
                int nr;
                sscanf(tip+5,"%d", &nr);
                nr++;
                fwrite(&nr, sizeof(nr), 1, db);
            }
            fwrite(",", 1, 1, db);
        }
        fwrite("\n", 1, 1, db);
        fclose(db);
    }
}

void insert() {
    char check[10], table[100];
    scanf("%s %s", check, table);
    strcat(table, ".db");

    if(strcmp(check, "INTO") != 0){
        help();
    } else {
        // Check if table doesn't exist. If it doesn't, quit
        if( access(table, F_OK ) != 0 ) {
            printf("Nu exista %s", table);
            exit(0);
        }

        FILE *fin = fopen(table, "r+b");
        table_t *tab = init_table(fin);
        fseek(fin, 0, SEEK_SET);

        printf("Table Data:\n");
        printf("  - # of columns: %d\n", tab->columns);
        printf("  - Info size: %zu\n", tab->info_size);
        printf("  - Row size: %zu\n", tab->row_size);
        printf("Columns:\n");
        for (int i = 0; i < tab->columns; i++) {
            printf("  * %s, type: %c, size: %zu\n",
                   tab->column_names[i],
                   tab->column_types[i],
                   tab->column_sizes[i]);
        }

        fclose(fin);
        fin = fopen(table, "r+b");

        char *row_buffer = malloc(sizeof *row_buffer * tab->row_size);
        memset(row_buffer, 0, tab->row_size);

        size_t iter = 0;
        for (int i = 0; i < tab->columns; i++) {
            if (tab->column_types[i] == 'i') {
                int temp;
                scanf("%d", &temp);

                memcpy(row_buffer+iter, &temp, sizeof temp);
            }
            else if (tab->column_types[i] == 'c') {
                char *temp;
                temp = malloc(tab->column_sizes[i]);
                memset(temp, 0, tab->column_sizes[i]);
                scanf("%s", temp);  // buffer overflow

                if (temp[strlen(temp)-1] == ';')
                    temp[strlen(temp)-1] = '\0';

                memcpy(row_buffer+iter, temp, tab->column_sizes[i]);
            }
            else if (tab->column_types[i] == 'f') {
                float temp;
                scanf("%f", &temp);

                memcpy(row_buffer+iter, &temp, sizeof temp);
            }
            iter += tab->column_sizes[i];
        }

        insert_row(row_buffer, tab, fin);
        fclose(fin);

        free_table(tab);
        free(row_buffer);
    }
}

void delete(){
    char check[10], table_name[100];
    int condition = 1;
    scanf("%s %s", check, table_name);

    if (table_name[strlen(table_name)-1] == ';') {
        table_name[strlen(table_name)-1] = '\0';
        condition = 0;
    }

    strcat(table_name, ".db");

    if( access(table_name, F_OK ) != 0 ) {
        printf("Table %s does not exist.", table_name);
        exit(0);
    }

    FILE *fin = fopen(table_name, "r+b");
    table_t *tab = init_table(fin);
    fseek(fin, 0, SEEK_SET);

    fclose(fin);
    fin = fopen(table_name, "r+b");

    char column_name[100];
    void *value = NULL;

    if (condition) {
        scanf("%s", check);
        scanf("%s", column_name);
        scanf("%s", check);


        for (int i = 0; i < tab->columns; i++) {
            if (strcmp(column_name, tab->column_names[i]) == 0) {
                if (tab->column_types[i] == 'i') {
                    value = malloc(sizeof(int));
                    scanf("%d", (int *)value);
                }
                else if (tab->column_types[i] == 'f') {
                    value = malloc(sizeof(float));
                    scanf("%f", (float *)value);
                }
                else {
                    value = malloc(tab->column_sizes[i]);
                    scanf("%s", (char *)value);
                    if (((char *)value)[strlen((char *)value)-1] == ';')
                        ((char *)value)[strlen((char *)value)-1] = '\0';
                }
            }
        }
    }

    delete_row(tab, fin, column_name, value);

    free(value);
    fclose(fin);
    free_table(tab);
}

void update(){
    char check[10], table_name[100];
    int condition = 1;
    scanf("%s", table_name);

    if (table_name[strlen(table_name)-1] == ';')
        table_name[strlen(table_name)-1] = '\0';

    strcat(table_name, ".db");

    if( access(table_name, F_OK ) != 0 ) {
        printf("Table %s does not exist.", table_name);
        exit(0);
    }

    FILE *fin = fopen(table_name, "r+b");
    table_t *tab = init_table(fin);

    fseek(fin, 0, SEEK_SET);
#ifndef __unix__
    fclose(fin);
    fin = fopen(table_name, "r+b");
#endif

    char temp_buffer[100];
    scanf("%s", check);

    void **values = malloc(tab->columns * sizeof(*values));
    for (int i = 0; i < tab->columns; i++)
        values[i] = NULL;

    // read column names until stdin reaches WHERE condition
    for (scanf("%s", temp_buffer);
            strcmp(temp_buffer, "WHERE") != 0;
            scanf("%s", temp_buffer)) {
        void *value;

        if (strcmp(temp_buffer, ";") == 0) {
            condition = 0;
            break;
        }

        for (int i = 0; i < tab->columns; i++) {
            if (strcmp(temp_buffer, tab->column_names[i]) == 0) {
                if (tab->column_types[i] == 'i') {
                    value = malloc(sizeof(int));
                    scanf("%d", (int *)value);
                }
                else if (tab->column_types[i] == 'f') {
                    value = malloc(sizeof(float));
                    scanf("%f", (float *)value);
                }
                else {
                    value = calloc(tab->column_sizes[i], 1);

                    scanf("%s", (char *)value);
                    if (((char *)value)[strlen((char *)value)-1] == ';') {
                        ((char *)value)[strlen((char *)value)-1] = '\0';
                        condition = 0;
                        values[i] = value;
                        break;
                    }
                }
                values[i] = value;
            }
        }
    }

    char column_name[100];
    void *value = NULL;

    if (condition) {
        scanf("%s", column_name);
        scanf("%s", check);

        for (int i = 0; i < tab->columns; i++) {
            if (strcmp(column_name, tab->column_names[i]) == 0) {
                if (tab->column_types[i] == 'i') {
                    value = malloc(sizeof(int));
                    scanf("%d", (int *)value);
                }
                else if (tab->column_types[i] == 'f') {
                    value = malloc(sizeof(float));
                    scanf("%f", (float *)value);
                }
                else {
                    value = malloc(tab->column_sizes[i]);
                    scanf("%s", (char *)value);
                }
            }
        }
    }

    update_rows(tab, fin, values, column_name, value);

    for(int i=0; i<tab->columns; i++)
        free(values[i]);

    free(values);

    fclose(fin);
    free_table(tab);
}

void select1(){

    queue_t *queue = init_queue();
    char column_name[100];
    int select_all = 0;

    for (scanf("%99s", column_name);
            strcmp(column_name, "FROM") != 0;
            scanf("%99s", column_name)) {
        if (strcmp(column_name, "*") == 0) {
            select_all = 1;
        } else {
            char *string = malloc(strlen(column_name) + 1);
            memcpy(string, column_name, strlen(column_name) + 1);

            insert_string(queue, string);
        }
    }

    char table[100];
    scanf("%99s", table);

    int flag_where = 1;

    if (table[strlen(table)-1] == ';') {
        table[strlen(table) - 1] = '\0';
        flag_where = 0;
    }

    strcat(table, ".db");

    if( access(table, F_OK ) != 0 ) {
        printf("Table %s does not exist.", table);
        exit(0);
    }

    FILE *fin = fopen(table, "r+b");
    table_t *tab = init_table(fin);
    fseek(fin, 0, SEEK_SET);
#ifdef __unix__
    fclose(fin);
    fin = fopen(table, "r+b");
#endif

    int *selected_cols = malloc(sizeof *selected_cols * tab->columns);
    if(select_all == 1) {
        for (int i = 0; i < tab->columns; i++)
            selected_cols[i] = 1;
    }
    else {
        for (int i = 0; i < tab->columns; i++)
            selected_cols[i] = 0;

        // find positions in table->column_names for each selected column
        while (queue->front != NULL) {
            char *col = remove_string(queue);

            for (int i = 0; i < tab->columns; i++) {
                if (strcmp(col, tab->column_names[i]) == 0) {
                    selected_cols[i] = 1;
                    break;
                }
            }

            free(col);
        }
    }

    char temp_buffer[100];
    
    if(flag_where == 1) {
        scanf("%s", temp_buffer);   // where
        scanf("%s", column_name);
        scanf("%s", temp_buffer);   // =

        void *value = NULL;

        for (int i = 0; i < tab->columns; i++) {
            if (strcmp(column_name, tab->column_names[i]) == 0) {
                if (tab->column_types[i] == 'i') {
                    value = malloc(sizeof(int));
                    scanf("%d", (int *) value);
                } else if (tab->column_types[i] == 'f') {
                    value = malloc(sizeof(float));
                    scanf("%f", (float *) value);
                } else {
                    value = malloc(tab->column_sizes[i]);
                    scanf("%s", (char *) value);

                    if (((char *)value)[strlen((char *)value)-1] == ';')
                        ((char *) value)[strlen((char *) value) - 1] = '\0';
                }
            }
        }

        select_rows(tab, fin, selected_cols, column_name, value);

        free(value);
    }
    else {
        select_rows(tab, fin, selected_cols, NULL, NULL);
    }
    
    free(selected_cols);
    fclose(fin);
    free_table(tab);
}

void drop(){
    char check[100], name[100];

    scanf("%s", check);

    if(strcmp(check, "DATABASE") == 0){
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if(strcmp(dir->d_name + (strlen(dir->d_name) - 3),".db") == 0){
                    remove(dir->d_name); // deletes all files with .db extension
                }
            }

            char cwd[PATH_MAX]; //cwd - current working directory
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                chdir("..");
                int r = rmdir(cwd); // delete database folder
                if(r == -1){
                    perror("rmdir() error");
                }
            } else {
                perror("getcwd() error");
            }
            closedir(d);
        }
    } else if(strcmp(check, "TABLE") == 0){
        scanf("%s", name);
        strcat(name, ".db");
        remove(name);
    }
}

int main(int argc, char* argv[]) {
    char sqlcmd[1000];
    scanf("%s", sqlcmd); // reads first keyword from stdin

    if(argc == 2) {
        char path[100];
        strcpy(path, argv[1]);
#ifdef __unix__
        int check = mkdir(path, 0777);
#else
	int check = mkdir(path, 0777);
#endif
        // check if database is created or not
        if (!check)
            printf("Database created: %s\n", path);

        int ch=chdir(path); // changing working directory
        if(ch<0) {
            printf("Something went wrong, chdir change of directory not successful\n");
            // return 2;
        }

        if (strcmp(sqlcmd, "CREATE") == 0) {
            create();
        } else if (strcmp(sqlcmd, "INSERT") == 0) {
            insert();
        } else if (strcmp(sqlcmd, "DELETE") == 0) {
            delete();
        } else if (strcmp(sqlcmd, "UPDATE") == 0) {
            update();
        } else if (strcmp(sqlcmd, "SELECT") == 0) {
            select1();
        } else if (strcmp(sqlcmd, "DROP") == 0){
            drop();
        } else {
            help();
        }
    } else {
        help();
    }
}
