#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "argparse.h"
#include "core.h"

static struct argp parser = { options, parse_opts, args_doc, doc };

int main(int argc, char **argv) {
	struct Arguments arguments;
	arguments.file = "-";
	arguments.prefix = "x";
	arguments.suffix_len = 2;
	arguments.bytes = -1;
	arguments.chunks = 2;

	argp_parse(&parser, argc, argv, 0, 0, &arguments);

	// Generate format string
	char* fmt_str = (char*) malloc(32 * sizeof(char));
	sprintf(fmt_str, "%%s%%0%dd", arguments.suffix_len);
	int fmt_len = strlen(arguments.prefix) + arguments.suffix_len;

	// Resolve chunks to chunk_size
	if (arguments.bytes < 0) {
		arguments.bytes = size_from_chunks(arguments.file, arguments.chunks);
		if (arguments.bytes <= 0) {
			exit(arguments.bytes);
		}
	}

	return split_file(arguments.file, arguments.bytes,
					  arguments.prefix, fmt_str, fmt_len);
}
