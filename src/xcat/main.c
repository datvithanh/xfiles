#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "argparse.h"
#include "core.h"

static struct argp parser = {options, parse_opts, args_doc, doc};

int main(int argc, char **argv) {
  struct Arguments arguments;

  arguments.files = (char**) malloc(sizeof(char*));
  arguments.end = 0;
  arguments.tab = 0;
  arguments.number = 0;

  argp_parse(&parser, argc, argv, 0, 0, &arguments);

  char* file = arguments.files[0];
	int count = 0;
	while (file) {
		file = arguments.files[++count];
	}

  int idx = 0;
  int lineCount = 0;
  for(int i = 0; i < count; ++i)
    lineCount = cat_std_print(arguments.files[i], lineCount, arguments.end, arguments.tab, arguments.number);
  return 0;
}