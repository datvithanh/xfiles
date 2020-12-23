#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "argparse.h"
#include "core.h"

static struct argp parser = {options, parse_opts, args_doc, doc};

int main(int argc, char **argv) {
  struct Arguments arguments;

  arguments.args = (char**) malloc(sizeof(char*));

  argp_parse(&parser, argc, argv, 0, 0, &arguments);

  char* file = arguments.args[0];
	int count = 0;
	while (file) {
		file = arguments.args[++count];
	} 

  if (count != 2){
    printf("missing or redundant operands\n");
    return 0;
  }

  grep_std_print(arguments.args[0], arguments.args[1]);
  return 0;
}