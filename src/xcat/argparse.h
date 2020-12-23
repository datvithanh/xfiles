#include <argp.h>
#include <error.h>
#include <string.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "<datvithanh98@gmail.com>";
static char doc[] = "Re-implementation of GNU cat.";
static char args_doc[] = "[FILE...]";

static struct argp_option options[] = {
  {"show-ends", 'E', 0, 0, "display $ at end of each line"},
  {"show-tabs", 'T', 0, 0, "display TAB characters as ^I"},
  {"number", 'n', 0, 0, "number all output lines"},
  {0}
};

struct Arguments {
  char **files;
  int end, tab, number;
};

static error_t parse_opts(int key, char *arg, struct argp_state *state) {
  struct Arguments *arguments = state->input;

	switch (key) {
    case 'E':
      arguments->end = 1;
      break;
    case 'T':
      arguments->tab = 1;
      break;
    case 'n':
      arguments->number = 1;
      break;
    case ARGP_KEY_NO_ARGS:
			argp_usage(state);
		case ARGP_KEY_ARG:
			arguments->files = &state->argv[state->next - 1];
			state->next = state->argc;
			break;
    default:
			return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
