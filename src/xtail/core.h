#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUF_EXP_SIZE 32
#define BUF_INIT_SIZE 128

int tail_std_print(const char* path, int lines) {
	FILE* f = fopen(path, "rt");
	if (!f) {
		perror("open");
		return errno;
	}

	int size = BUF_INIT_SIZE, idx = 0;
	char* line = (char*) malloc (size * sizeof(char));
	char** lineBuf = (char**) malloc(lines * sizeof(char*));
	memset(lineBuf, 0, lines);
	int lineCount = 0;

	char c = fgetc(f);
	int trail_nl = 0;
	while (c != EOF) {
		if(idx == size) {
			size += BUF_EXP_SIZE;
			line = (char*) realloc(line, size);
		}

		line[idx++] = c;
		if(c == '\n') {
			line[idx] = '\0';
			// if (lineBuf[lineCount % lines])
			// 	free(lineBuf[lineCount % lines]);
			lineBuf[lineCount % lines] = line;
			lineCount++;
			size = BUF_INIT_SIZE, idx = 0;
			line = (char*) malloc (size * sizeof(char));
			trail_nl = 1;
		} else {
			trail_nl = 0;
		}

		c = fgetc(f);
	}

	// Flush everything left
	if (idx) {
		line[idx] = '\0';
		if (lineBuf[lineCount % lines])
			free(lineBuf[lineCount % lines]);
		lineBuf[lineCount % lines] = line;
		lineCount++;
	}
	
	for (int i=0; i<lines; i++) {
		int aci = (lineCount + i) % lines;
		if (lineBuf[aci]) {
			printf("%s", lineBuf[aci]);
			free(lineBuf[aci]);
		}
	}

	free(lineBuf);
	fclose(f);
	return 0;
}
