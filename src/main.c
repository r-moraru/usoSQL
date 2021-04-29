#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void help(){
    printf("Citeste fisierul README.md");
}

void create(char* path){
    char check[10], table[100];
    printf("Sunt la create!\n");
    printf("%s", path);
    scanf("%s %s", check, table);
    strcat(path, "//");
    strcat(path, table);
    strcat(path, ".db");
    if(strcmp(check, "TABLE") != 0){
        help();
    } else {
        FILE *db = fopen(path, "wb");
        char coloana[100], tip[100];
        while(tip[strlen(tip)-1] != ';'){
            scanf("%s", coloana);
            scanf("%s", tip);
            printf("%s %s\n", coloana, tip);
            fwrite(coloana, 1, strlen(coloana)+1, db);
            fwrite(tip, 1, 1, db);
            if(tip[0] == 'c'){
                int nr;
                sscanf(tip+5,"%d", &nr);
                fwrite(&nr, sizeof(nr), 1, db);
            }
            fwrite(",", 1, 1, db);
        }
        fclose(db);
    }
}

void insert(char* path){
    printf("Sunt la insert!\n");
    printf("%s", path);
}

void delete(char* path){
    printf("Sunt la delete!\n");
    printf("%s", path);
}

void update(char* path){
    printf("Sunt la update!\n");
    printf("%s", path);
}

void select(char* path){
    printf("Sunt la select!\n");
    printf("%s", path);
}

void drop(char* path){
    printf("Sunt la drop!\n");
    printf("%s", path);
    char check[100], name[100];
    scanf("%s %s", check, name);
    if(strcmp(check, "DATABASE") == 0){
        //de facut
    } else if(strcmp(check, "TABLE") == 0){
        strcat(path, "//");
        strcat(path, name);
        strcat(path, ".db");
        remove(path);
    }
}

int main(int argc, char* argv[]) {
    char sqlcmd[1000];
    int i;
    scanf("%s", sqlcmd); //citeste pana la caracterul ';'
    if(argc == 2) {
        char path[100];
        strcpy(path, argv[1]);
        int check = mkdir(path);

        // check if directory is created or not
        if (!check)
            printf("Database created\n");
        if (strcmp(sqlcmd, "CREATE") == 0) {
            create(path);
        } else if (strcmp(sqlcmd, "INSERT") == 0) {
            insert(path);
        } else if (strcmp(sqlcmd, "DELETE") == 0) {
            delete(path);
        } else if (strcmp(sqlcmd, "UPDATE") == 0) {
            update(path);
        } else if (strcmp(sqlcmd, "SELECT") == 0) {
            select(path);
        } else if (strcmp(sqlcmd, "DROP") == 0){
            drop(path);
        } else {
            help();
        }
    } else {
        help();
    }
}
