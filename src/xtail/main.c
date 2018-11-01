#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "argparse.h"
#include "core.h"

static struct argp parser = { options, parse_opts, args_doc, doc };

int main(int argc, char **argv) {
	struct Arguments arguments;
	arguments.files = (char**) malloc(sizeof(char*));
	arguments.files[0] = "-";
	arguments.follow = 0;
	arguments.lines = 10;
	arguments.quiet = 0;
	arguments.verbose = 0;
	arguments.retry = 0;

	argp_parse(&parser, argc, argv, 0, 0, &arguments);
	char* file = arguments.files[0];
	int count = 0;
	while (file) {
		file = arguments.files[++count];
	}

	for (int i=0; i<count; i++) {
		file = arguments.files[i];
		if ((count > 1 && !arguments.quiet) || arguments.verbose) {
			if (i) printf("\n");
			printf("==> %s <==\n", file);
		}

		if (arguments.retry) {
			while(tail_std_print(file, arguments.lines)) sleep(1);
		} else tail_std_print(file, arguments.lines);
	}

	return 0;
}
