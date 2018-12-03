#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>

#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define PURPLE  "\x1B[35m"
#define RESET "\x1B[0m"

int is_dir(char *base_path, char *new_path){
    struct stat st;
    char all_path[1000];
    strcpy(all_path, base_path);
    strcat(all_path, "/");
    strcat(all_path, new_path);
    int status = stat(all_path, &st);
    if(S_ISDIR(st.st_mode)){
        return 1;
    } else if (S_ISREG(st.st_mode)){
        return 0;
    }
}

int is_hidden(char *entry){
    return entry[0] == '.';
}

char *file_name(char *path){
    char *a = (char *) malloc(255);
    int l = strlen(path);
    int id = 0;
    for(int i = strlen(path)-1; i >= 0; i--){
        if(path[i] == '/'){
            id = i;
            break;
        }
    }
    strcpy(a, path+id+1);
    return a;
}
char *get_permission(struct stat st_buf){
    char *permission = (char *) malloc(13 * sizeof(char));
    permission[0] = '[';
    permission[1] = S_ISDIR(st_buf.st_mode) ? 'd' : '-';
    permission[2] =  (st_buf.st_mode & S_IRUSR) ? 'r' : '-';
    permission[3] = (st_buf.st_mode & S_IWUSR) ? 'w' : '-';
    permission[4] = (st_buf.st_mode & S_IXUSR) ? 'x' : '-';
    permission[5] = (st_buf.st_mode & S_IRGRP) ? 'r' : '-';
    permission[6] = (st_buf.st_mode & S_IWGRP) ? 'w' : '-';
    permission[7] = (st_buf.st_mode & S_IXGRP) ? 'x' : '-';
    permission[8] = (st_buf.st_mode & S_IROTH) ? 'r' : '-';
    permission[9] = (st_buf.st_mode & S_IWOTH) ? 'w' : '-';
    permission[10] = (st_buf.st_mode & S_IXOTH) ? 'x' : '-';
    permission[11] = ']';
    permission[12] = '\0';
    return permission;
}
char can_opened(char *path){
    // drwx------
    struct stat st;
    stat(path, &st);
    char *p = get_permission(st);
    puts(p);
    if(p[2] == 'r' && p[8] == 'r'){
        return 1;
    }
    return 0;
}

void main(){
    // char a[100] = ".test";
    char b[100] = "test3";
    // printf("%d\n", is_dir(a, b));
    // int t = is_dir(a, b);
    // if(t){
    //     printf("folder\n");
    // } else {
    //     printf("file\n");
    // }
    // struct stat st;
    // stat(a, &st);
    // if(S_ISDIR(st.st_mode)){
    //     printf("dir");
    // } else if (S_ISREG(st.st_mode)){
    //     printf("file");
    // }
    // printf("This is " RED "red"  " and this is " BLUE "blue" RESET "\n");
    DIR *dir;
    dir = opendir("/boot/efi");
    printf("%d", can_opened("/boot/efi"));
}