#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

#include "pager/pager.h"
#include "queue/queue.h"

void help(){
    printf("Citeste fisierul README.md");
}

void create() {
    char check[10], table[100];
    printf("Sunt la create!\n");
    scanf("%s %s", check, table);

    strcat(table, ".db");
    if(strcmp(check, "TABLE") != 0){
        help();
    } else {
        // Check if table already exists. If it does, quit
        if( access(table, F_OK ) == 0 ) {
            printf("Deja exista %s", table);
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
    printf("Sunt la insert!\n");
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
        // check if NULL

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
    printf("Sunt la delete!\n");
    char check[10], table_name[100];
    scanf("%s %s", check, table_name);

    if (table_name[strlen(table_name)-1] == ';')
        table_name[strlen(table_name)-1] = '\0';

    strcat(table_name, ".db");

    if( access(table_name, F_OK ) != 0 ) {
        printf("Nu exista %s", table_name);
        exit(0);
    }

    FILE *fin = fopen(table_name, "r+b");
    table_t *tab = init_table(fin);

    fclose(fin);

    fin = fopen(table_name, "r+b");

    char column_name[100];

    scanf("%s", check);
    scanf("%s", column_name);
    scanf("%s", check);

    void *value;

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

    printf("Delete lol\n");
    delete_row(tab, fin, column_name, value);

    free(value);
    fclose(fin);
    free_table(tab);
}

void update(){
    printf("Sunt la update!\n");

    char check[10], table_name[100];
    scanf("%s %s", check, table_name);

    if (table_name[strlen(table_name)-1] == ';')
        table_name[strlen(table_name)-1] = '\0';

    strcat(table_name, ".db");

    if( access(table_name, F_OK ) != 0 ) {
        printf("Nu exista %s", table_name);
        exit(0);
    }

    FILE *fin = fopen(table_name, "r+b");
    table_t *tab = init_table(fin);

    fclose(fin);

    fin = fopen(table_name, "r+b");

    char temp_buffer[100];
    scanf("%s", check);

    // TODO: initiate column - value queues

    for (scanf("%s", temp_buffer);
            strcmp(temp_buffer, "WHERE") != 0;
            scanf("%s", temp_buffer)) {
        void *value;

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
                    value = malloc(tab->column_sizes[i]);
                    scanf("%s", (char *)value);
                }
            }
        }
    }

    fclose(fin);
    free_table(tab);
}

void select(){
    printf("Sunt la select!\n");

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
        printf("Nu exista %s", table);
        exit(0);
    }

    FILE *fin = fopen(table, "r+b");
    table_t *tab = init_table(fin);

    fclose(fin);

    fin = fopen(table, "r+b");

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
    printf("Sunt la drop!\n");
    char check[100], name[100];

    scanf("%s", check);

    if(strcmp(check, "DATABASE") == 0){
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if(strcmp(dir->d_name + (strlen(dir->d_name) - 3),".db") == 0){
                    remove(dir->d_name); //se sterg toate fisierele cu extensia .db din folder
                }
            }

            char cwd[PATH_MAX]; //cwd - current working directory
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                chdir(".."); //schimb cwd pentru a il putea sterge
                int r = rmdir(cwd); //sterg folderul bazei de date
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
    scanf("%s", sqlcmd); //citeste primul cuvant cheie

    if(argc == 2) {
        char path[100];
        strcpy(path, argv[1]);

        int check = mkdir(path);
        // check if database is created or not
        if (!check)
            printf("Database created\n");

        int ch=chdir(path); //aici se schimba working directory-ul pentru program
        if(ch<0) {
            printf("Something went wrong, chdir change of directory not successful\n");
            return 2;
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
            select();
        } else if (strcmp(sqlcmd, "DROP") == 0){
            drop();
        } else {
            help();
        }
    } else {
        help();
    }
}
