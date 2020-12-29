#include <argp.h>
#include <error.h>
#include <string.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "<datvithanh98@gmail.com>";
static char doc[] = "Inode information of a file";
static char args_doc[] = "[FILE...]";

static struct argp_option options[] = {
  {0}
};

struct Arguments {
  char **args;
};

static error_t parse_opts(int key, char *arg, struct argp_state *state) {
  struct Arguments *arguments = state->input;

	switch (key) {
    case ARGP_KEY_NO_ARGS:
			argp_usage(state);
		case ARGP_KEY_ARG:
			arguments->args = &state->argv[state->next - 1];
			state->next = state->argc;
			break;
    default:
			return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
