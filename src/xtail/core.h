#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "stack.h"

#define MAX_BUF_LINES 80
#define BUF_LINE_SIZE 20

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
	
	while (linesLeft >= 0) {
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
		char* line = (char*) malloc(bufSize + (remainCount+1) * sizeof(char));
		if (remainCount) {
			// Remember to copy the chars from the previous read
			strncpy(line, charsRemain, remainCount);
		}
		int lineSize = remainCount;
		for (int i=charCount-1; i>=0 && linesLeft>=0; i--) {
			char c = buf[i];
			if (c == '\n') {
				// If a newline is found, commit the line and reset line buffer
				line[lineSize] = '\0';
				char* s = strrev(line);
				push(stack, s);
				lineSize = 0;
				linesLeft--;
			} else {
				// Otherwise, just copy the character
				line[lineSize++] = c;
			}
		}

		charsRemain = line;
		remainCount = lineSize;

		// Seek to the next position if needed
		if (!ofs) break;
		if (linesLeft>=0) {
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
		const char* line = pop(stack);
		if(lc) printf("\n");
		printf("%s", line);
		lc++;
	}

	return 0;
}

