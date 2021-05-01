#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

#include "pager/pager.h"

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
                fwrite(&nr, sizeof(nr), 1, db);
            }
            fwrite(",", 1, 1, db);
        }
        fwrite("\n", 1, 1, db);
        fclose(db);
    }
}

void insert(){
    printf("Sunt la insert!\n");
}

void delete(){
    printf("Sunt la delete!\n");
}

void update(){
    printf("Sunt la update!\n");
}

void select(){
    printf("Sunt la select!\n");
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
