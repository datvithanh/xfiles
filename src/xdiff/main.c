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
  arguments.brief = 0;
  arguments.unified = 0;

  argp_parse(&parser, argc, argv, 0, 0, &arguments);

  char* file = arguments.files[0];
	int count = 0;
	while (file)
		file = arguments.files[++count];

  if (count != 2){
    printf("missing or redundant operands\n");
    return 0;
  }

  diff_std_print(arguments.files[0], arguments.files[1], arguments.brief, arguments.unified);
  return 0;
}