#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define PURPLE  "\x1B[35m"
#define RESET "\x1B[0m"

const char DEFAULT_PATH[1000] = "/";

int my_atoi(char *str, int len);
char ** str_arr_sort(char **a, int n, int r);
char *get_permission(struct stat st_buf);
int is_legal_path(char *path);
void print_tree(int print_all, int just_print_directory, int is_absolute_path, int tab, int file_limit, int print_permissions, int lexicographical_order, int current_depth, int max_depth, char *path);
int is_dir(char *base_path, char *new_path);
int is_hidden(char *entry);
char *file_name(char *path);
char **double_size(char **a, int current_size);
char can_read(struct stat st);

long long int num_directories = 0, num_files = 0;

int main(int argc, char *argv[]){
    //Tham so -a, quyet dinh co in tat ca file hay khong, mac dinh la 0 (khong in tat ca)
    int print_all = 0;
    
    //Tham so -d, chi liet ke thu muc, mac dinh la 0 (in ca thu muc va file)
    int just_print_directory = 0;

    //Tham so -f, in day du duong dan cho moi file, mac dinh la 0 (khong in day du duong dan)
    int is_absolute_path = 0;

    //Tham so -i, khong in thut dau dong, mac dinh la 1 (co in thut dau dong)
    int tab = 3;

    //Tham so --filelimit, quyet dinh in cac thu muc co duoi bao nhieu entry
    int file_limit = INT_MAX;

    //Tham so -p, in permissions cho file/folder, mac dinh la 0 (khong in)
    int print_permissions = 0;

    //Tham so -r, in theo thu tu chu cai nguoc, mac dinh la 0 (in theo thu tu tu dien)
    int lexicographical_order = 0;

    //Tham so -L, do sau hien thi toi da cua cay thu muc
    int max_depth = INT_MAX;

    //Duong dan den file/folder
    char path[1000];
    strcpy(path, DEFAULT_PATH);

    int param_count = 0;

    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-a")){
            print_all = 1;
        } else if(!strcmp(argv[i], "-d")){
            just_print_directory = 1;
        } else if(!strcmp(argv[i], "-f")){
            is_absolute_path = 1;
        } else if(!strcmp(argv[i], "-i")){
            tab = 0;
        } else if(!strcmp(argv[i], "--filelimit")){
            i++;
            file_limit = my_atoi(argv[i], strlen(argv[i]));
            if(file_limit == -1){
                printf("Usage: --filelimit num_of_file, with num_of_file is non-negative number\n");
                exit(1);
            }
        } else if(!strcmp(argv[i], "-p")){
            print_permissions = 1;
        } else if(!strcmp(argv[i], "-r")){
            lexicographical_order = 1;
        } else if(!strcmp(argv[i], "-L")){
            i++;
            max_depth = my_atoi(argv[i], strlen(argv[i]));
            if(max_depth == -1){
                printf("Usage: -L max_depth, with max_depth is non-negative number\n");
                exit(1);
            }
        } else {
            int check = is_legal_path(argv[i]);
            if(check == -1){
                printf("Invalid argument\n");
                exit(1);
            } else if(check == 0){
                printf("Error opening dir\n");
                exit(1);
            } else {
                strcpy(path, argv[i]);
                break;
            }
        }      
    }
    
    // printf("Parameters: \n");
    // printf("-a: %d\n-d: %d\n-f: %d\n-i: %d\n--filelimit: %d\n-p: %d\n-r: %d\n-L: %d\n", print_all, just_print_directory, is_absolute_path, tab, file_limit, print_permissions, lexicographical_order, max_depth);
    // printf("Path: %s\n", path);

    print_tree(print_all, just_print_directory, is_absolute_path, tab, file_limit, print_permissions, lexicographical_order, 0, max_depth, path);
    printf("\n%llu directories, %llu files\n", num_directories, num_files);
}


// Cac tham so se su dung:
//     -a : in mọi files (mặc định không in file ẩn) -> ok
//     -d : chỉ liệt kê thư mục  -> ok
//     -f : in đầy đủ đường dẫn cho mỗi file  -> ok
//     -i : không in thụt đầu dòng  -> ok
//     --filelimit num: không in các thư mục vượt quá num entry -> ok
//     -p : in các bảo vệ (quyền) cho mỗi tệp 
//     -r : in đầu ra theo thứ tự chữ cái ngược -> ok
//     -L : độ sâu hiển thị tối đa của cây thư mục  -> ok

// Xử lí tham số:
    // print_all == 1: đưa các entry có dấu . ở đầu vào list 
    // just_print_directory == 1: chỉ đưa thư mục vào list 
    // is_absolute_path == 1: in ra đường dẫn đầy đủ 
    // tab = 0: không in thụt đầu dòng 
    // filelimit: đếm số file rồi mới in 
    // print_permissions == 1: in quyền trước rồi đến tên file 
    // lexicographical_order == 1: sắp xếp ngược 
    // max_depth: xem độ sau hiện tại có bằng độ sau max hay không 

                                    
void print_tree(int print_all, int just_print_directory, int is_absolute_path, int tab, int file_limit, int print_permissions, int lexicographical_order, int current_depth, int max_depth, char *path){
    if(current_depth > max_depth){
        return;
    }
    if(!is_legal_path(path)){
        return;
    }
    
    char **entry_list;
    int entry_list_size = 1000;
    entry_list = (char **) malloc(entry_list_size*sizeof(char *));
    for(int i = 0; i < entry_list_size; i++){
        entry_list[i] = (char *) malloc(1000*sizeof(char));
    }
    int num_entry = 0;
    // DIR *dir;
    // if(can_opened(path)){
    //     dir = opendir(path);
    // } else {
    //     return ;
    // }
    DIR *dir = opendir(path);
    struct dirent *entry;
    while((entry = readdir(dir)) !=  NULL){
        if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            if(print_all){
                if(just_print_directory){
                    if(is_dir(path, entry->d_name)){
                        strcpy(entry_list[num_entry], entry->d_name);
                        num_entry++;
                    }
                } else {
                    strcpy(entry_list[num_entry], entry->d_name);
                    num_entry++;
                }
            } else {
                if(!is_hidden(entry->d_name)){
                    if(just_print_directory){
                        if(is_dir(path, entry->d_name)){
                            strcpy(entry_list[num_entry], entry->d_name);
                            num_entry++;
                        }
                    } else {
                        strcpy(entry_list[num_entry], entry->d_name);
                        num_entry++;
                    }
                }
            }
        }

        if(num_entry+1 >= entry_list_size){
            printf(RED "Doubling size of entry\n" RESET);
            entry_list = double_size(entry_list, entry_list_size);
            entry_list_size *= 2;
        }
    }
    closedir(dir);
    if(num_entry > file_limit){
        return;
    }
    entry_list = str_arr_sort(entry_list, num_entry, lexicographical_order);
    if(current_depth != 0){
        for(int i = 0; i < current_depth-1; i++){
            printf("|");
            for(int j = 0; j < tab; j++){
                printf(" ");
            }
        }
        printf("|");
        for(int j = 0; j < tab; j++){
            printf("-");
        }
        if(is_absolute_path){
            if(print_permissions){
                struct stat st;
                stat(path, &st);
                printf("%s  ", get_permission(st));
            }
            printf(BLUE "%s\n" RESET, path);   
        } else {
            if(print_permissions){
                struct stat st;
                stat(path, &st);
                printf("%s  ", get_permission(st));
            }
            printf(BLUE "%s\n" RESET, file_name(path));   
        }
    } else {
        if(is_absolute_path){
            printf(BLUE "%s\n" RESET, file_name(path));   
        } else {
            printf(BLUE "%s\n" RESET, path);   
        }
    }
    

    for(int i = 0; i < num_entry; i++){
        // if(is_dir(path, entry_list[i])){
        //     printf(BLUE "%s\n" RESET, entry_list[i]);
        // } else {
        //     puts(entry_list[i]);
        // }
        char new_path[1000];
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, entry_list[i]);
        // printf(RED "%s\n" RESET, new_path);
        if(is_dir(path, entry_list[i])){
            num_directories++;
            print_tree(print_all, just_print_directory, is_absolute_path, tab, file_limit, print_permissions, lexicographical_order, current_depth+1, max_depth, new_path);
        } else {
            if(current_depth+1 > max_depth){
                return;
            } else {
                num_files++;
                for(int i = 0; i < current_depth; i++){
                    printf("|");
                    for(int j = 0; j < tab; j++){
                        printf(" ");
                    }
                }
                printf("|");
                for(int j = 0; j < tab; j++){
                    printf("-");
                }
                if(is_absolute_path){
                    if(print_permissions){
                        struct stat st;
                        stat(new_path, &st);
                        printf("%s  ", get_permission(st));
                    }
                    printf("%s\n", new_path);  
                } else {
                    if(print_permissions){
                        struct stat st;
                        stat(new_path, &st);
                        printf("%s  ", get_permission(st));
                    }
                    printf("%s\n", entry_list[i]);
                }
            }
        }
    }
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

char ** str_arr_sort(char **a, int n, int r){
    char **b = (char **) malloc(n * sizeof(char **));
    for(int i = 0; i < n; i++){
        b[i] = (char *) malloc(1000 * sizeof(char));
        strcpy(b[i], a[i]);
    }
    for(int i = 0; i < n-1; i++){
        for(int j = i+1;j < n; j++){
            if(r == 0){
                if(strcmp(b[i], b[j]) > 0){
                    char tmp[1000];
                    strcpy(tmp, b[i]);
                    strcpy(b[i], b[j]);
                    strcpy(b[j], tmp);
                }
            } else {
                if(strcmp(b[i], b[j]) < 0){
                    char tmp[1000];
                    strcpy(tmp, b[i]);
                    strcpy(b[i], b[j]);
                    strcpy(b[j], tmp);
                }
            }
        }
    }
    return b;
}

char ** str_arr_cpy(char **a, int size){
    char **b = (char **) malloc(2*size*sizeof(char *));
    for(int i = 0; i < size; i++){
        b[i] = (char *) malloc(1000*sizeof(char));
        strcpy(b[i], a[i]);
    }
    return b;
}

int my_atoi(char *str, int len){
    for(int i = 0; i < len; i++){
        if(str[i] < 48 || str[i] > 57){
            return -1;
        }
    }
    return atoi(str);
}

int is_legal_path(char *path){
    if(path[0] == '-'){
        return -1;
    } else {
        struct stat st;
        // DIR *dir;
        // dir = opendir(path);
        // if(dir){
        //     return 1;
        // } else if(ENOENT == errno){
        //     return 0;
        // } 
        if(stat(path, &st) == 0 && can_read(st) && S_ISDIR(st.st_mode)){
            return 1;
        } else {
            return 0;
        }
    }
    return 1;
}

int is_dir(char *base_path, char *new_path){
    struct stat st;
    char all_path[1000];
    strcpy(all_path, base_path);
    strcat(all_path, "/");
    strcat(all_path, new_path);
    int status = stat(all_path, &st);
    if(S_ISDIR(st.st_mode) && can_read(st)){
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

char **double_size(char **a, int current_size){
    char **b = (char **) malloc(current_size*2*sizeof(char *));
    for(int i = 0; i < current_size*2; i++){
        b[i] = (char *) malloc(1000*sizeof(char));
        if(i < current_size){
            strcpy(b[i], a[i]);
        }
    }
    return b;
}

char can_read(struct stat st){
    char *p = get_permission(st);
    uid_t uid = getuid();
    if(uid == st.st_uid || p[8] == 'r'){
        return 1;
    } 
    return 0;
}