#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUF_EXP_SIZE 512
#define BUF_INIT_SIZE 1024

int tail_std_print(const char* path, int lines) {
    FILE* f = fopen(path, "rt");
    if (!f) {
        perror("open");
        return errno;
    }

    int size = BUF_INIT_SIZE, idx = 0;
    char* line = (char*) malloc (size * sizeof(char));
    char** lineBuf = (char**) malloc(lines * sizeof(char*));
    int* strSizes = (int*) malloc(lines * sizeof(int));
    memset(lineBuf, 0, lines);
    int lineCount = 0;

    char c = fgetc(f);
    while (c != EOF) {
        if(idx == size) {
            size += BUF_EXP_SIZE;
            line = (char*) realloc(line, size);
        }

        line[idx++] = c;
        if(c == '\n') {
            lineBuf[lineCount % lines] = line;
            strSizes[lineCount % lines] = idx;
            lineCount++;
            size = BUF_INIT_SIZE, idx = 0;
            line = (char*) malloc (size * sizeof(char));
        }

        c = fgetc(f);
    }

    // Flush everything left
    if (idx) {
        if (lineBuf[lineCount % lines])
            free(lineBuf[lineCount % lines]);
        lineBuf[lineCount % lines] = line;
        strSizes[lineCount % lines] = idx;
        lineCount++;
    }
    
    for (int i=0; i<lines; i++) {
        int aci = (lineCount + i) % lines;
        if (lineBuf[aci]) {
            write(STDOUT_FILENO, lineBuf[aci], strSizes[aci]);
            free(lineBuf[aci]);
        }
    }

    free(lineBuf);
    fclose(f);
    return 0;
}
