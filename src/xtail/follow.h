#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define PRINT_BUF_SIZE 1024

enum PollMode {
	POLL_FD, POLL_NAME
};

static volatile int* watched_fds;
static volatile int num_fd = 0;

void close_watch() {
	for (int i=0; i<num_fd; i++) {
		close(watched_fds[i]);
	}
}

void close_watch_handler(int dummy) {
	close_watch();
	printf("\n");
	exit(0);
}

int count_items(char** items) {
	char* p = items[0];
	int count = 0;
	while(p) {
		p = items[++count];
	}
	return count;
}

void print_offset(int fd, off_t offset) {
	char* buf = (char*) malloc (PRINT_BUF_SIZE * sizeof(char));
	ssize_t bytesRead = pread(fd, buf, PRINT_BUF_SIZE * sizeof(char), offset);
	while (bytesRead > 0) {
		int charsRead = bytesRead / sizeof(char);
		write(STDOUT_FILENO, buf, bytesRead);
		offset += bytesRead;
		bytesRead = pread(fd, buf, PRINT_BUF_SIZE * sizeof(char), offset);
	}
}

int tail_fd_poll(char** paths, int sleep_s, int retry) {
	num_fd = count_items(paths);
	watched_fds = (int*) malloc (sizeof(int) * num_fd); 
	off_t *ofss = (off_t*) malloc (num_fd * sizeof(off_t));

	for (int i=0; i<num_fd; i++) {
		watched_fds[i] = open(paths[i], O_RDONLY);
		if (!retry && watched_fds[i] == -1) {
			perror("open");
			return errno;
		}
		if (watched_fds[i] != -1) {
			ofss[i] = lseek(watched_fds[i], 0, SEEK_END);
		}
	}

	while (1) {
		for (int i=0; i<num_fd; i++) {
			// Retry
			if (watched_fds[i] == -1) {
				watched_fds[i] = open(paths[i], O_RDONLY);
				if (watched_fds[i] != -1) {
					ofss[i] = lseek(watched_fds[i], 0, SEEK_END);
				} else {
					continue;
				}
			}

			// With valid fd
			off_t n_ofs = lseek(watched_fds[i], 0, SEEK_END);
			if (n_ofs == -1) {
				// TODO Maybe handle this differently
				continue;
			} else if (n_ofs != ofss[i]) {
				print_offset(watched_fds[i], ofss[i]);
				ofss[i] = n_ofs;
			}

			sleep(sleep_s);
		}
	}
}

int tail_name_poll(char** paths, int sleep_s, int retry) {
	num_fd = count_items(paths);
	watched_fds = (int*) malloc (sizeof(int) * num_fd);
}
