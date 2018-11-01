#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "stack.h"

#define MAX_BUF_LINES 40
#define BUF_LINE_SIZE 20
#define BUF_EXP_SIZE 32
#define BUF_INIT_SIZE 128

int get_fs_blocksize() {
	struct stat buf;
	int st = stat(".", &buf);
	if (st) {
		perror("stat");
	}

	return buf.st_blksize;
}

char* strrev(char* str) {
	int count = 0;
	while (str[count] != '\0')
    	count++;
	
	char* r = (char*) malloc (sizeof(char) * (count + 1));
	int end = count - 1;
	for (int i=0; i<count; i++) {
		r[i] = str[end];
		end--;
	}
	r[count] = '\0';
	return r;
}

int tail_seek_print(const char* path, int lines) {
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return errno;
	}

	// Initialize buffer
	int initLines = MAX_BUF_LINES > lines ? lines : MAX_BUF_LINES;
	int bufSize = sizeof(char) * BUF_LINE_SIZE * initLines;
	char *buf = (char*)malloc(bufSize);

	// Seek to last estimated line block
	// LOG("Seek to end");
	off_t ofs = lseek(fd, (off_t) -bufSize, SEEK_END);
	if (ofs == -1) {
		perror("lseek");
		return errno;
	}

	// Start printing
	int linesLeft = lines;
	char* charsRemain;
	int remainCount = 0;
	LineStack* stack = new_stack();
	
	while (linesLeft > 0) {
		printf("<<%ld>>\n", ofs);
		// Read into buffer
		ssize_t bytesRead = read(fd, buf, (size_t) bufSize);
		if (bytesRead == -1) {
			perror("read");
			return errno;
		} else if (!bytesRead) {
			break;
		}

		// Find lines
		int charCount = ((int) bytesRead) / sizeof(char);
		int lineSize = remainCount + 1;
		char* line = (char*) malloc(bufSize + (remainCount+1) * sizeof(char));
		if (remainCount) {
			// Remember to copy the chars from the previous read
			// charsRemain[remainCount] = '\0';
			// printf("[[%s]]\n", charsRemain);
			strcpy(line, charsRemain);
		}
		for (int i=charCount-1; i>=0 && linesLeft>0; i--) {
			char c = buf[i];
			if (c == '\n') {
				// If a newline is found, commit the line and reset line buffer
				char* s = strrev(line);
				push(stack, s);
				lineSize = 0;
				if (linesLeft != lines || strcmp(s, ""))
					linesLeft--;
			} else {
				// Otherwise, just copy the character
				line[lineSize++] = c;
				line[lineSize] = '\0';
			}
		}

		line[lineSize] = '\0';		
		if (charsRemain) {
			free(charsRemain);
		}
		remainCount = lineSize;
		if (remainCount) {
			charsRemain = (char*) malloc((remainCount+1) * sizeof(char));
			strcpy(charsRemain, line);
		}

		// Seek to the next position if needed
		if (ofs <= 0) break;
		if (linesLeft > 0) {
			// off_t new_pos = ((ofs - bufSize) >= 0) ? (ofs - bufSize) : 1;
			// printf("%ld\n", new_pos);
			ofs = lseek(fd, ofs - bufSize, SEEK_SET);
		}
	}

	// Flush the remaining buffer
	if (remainCount) {
		charsRemain[remainCount] = '\0';
		charsRemain = strrev(charsRemain);
		push(stack, charsRemain);
	}

	// Pop the stack and print
	int lc = 0;
	while(!is_empty(stack)) {
		char* line = pop(stack);
		if(lc) printf("\n");
		printf("%s", line);
		free(line);
		lc++;
	}

	close(fd);
	return 0;
}

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
	// printf("----\n");

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
		// if (i) printf("\n");
		if (lineBuf[aci]) {
			printf("%s", lineBuf[aci]);
			free(lineBuf[aci]);
		}
	}
	// if (trail_nl) printf("\n");

	free(lineBuf);
	fclose(f);
	return 0;
}
