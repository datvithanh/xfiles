#include <stdlib.h>
#include <stdio.h>

#include "argparse.h"

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
	
	return 0;
}
