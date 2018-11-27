#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>
#include <sys/types.h>
#include <errno.h>

const int MAX_BUF_SIZE = 64 * 1024;

off_t size_from_chunks(const char* src_path, int chunks) {
	int src_fd = open(src_path, O_RDONLY);
	if (errno) {
		perror("open");
		return errno;
	}

	struct stat *st = (struct stat*) malloc(sizeof(struct stat));
	fstat(src_fd, st);
	if (errno) {
		perror("stat");
		return errno;
	}

	off_t src_size = st->st_size;
	close(src_fd);
	return (off_t) ceil((double)src_size / (double)chunks);
}

int split_file(const char* src_path, int chunk_size, const char* prefix, const char* fmt_str, int fmt_len) {
	int src_fd = open(src_path, O_RDONLY);
	if (errno) {
		perror("open");
		return errno;
	}

	size_t buf_size = (chunk_size > MAX_BUF_SIZE) ? MAX_BUF_SIZE : chunk_size;
	void *buf = malloc(buf_size);
	int count = 0;
	ssize_t read_bytes = 0;

	do {
		char* fname = (char*) malloc(fmt_len * sizeof(char));
		sprintf(fname, fmt_str, prefix, count);
		
		int dest_fd = creat(fname, 0666);
		if (errno) {
			perror("creat");
			return errno;
		}

		ssize_t remaining = chunk_size;
		ssize_t total_written = 0;
		do {
			ssize_t abuf_size = (buf_size > remaining) ? remaining : buf_size;
			read_bytes = read(src_fd, buf, abuf_size);
			if (errno) {
				perror("read");
				return errno;
			}
			ssize_t written_bytes = write(dest_fd, buf, read_bytes);
			if (errno) {
				perror("write");
				return errno;
			}
			remaining -= written_bytes;
			total_written += written_bytes;
		} while (remaining > 0 && read_bytes > 0);

		count++;
		close(dest_fd);
		if (!total_written) {
			remove(fname);
		}
	} while (read_bytes > 0);

	close(src_fd);
	return 0;
}
