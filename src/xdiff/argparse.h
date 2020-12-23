#include <argp.h>
#include <error.h>
#include <string.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "<datvithanh98@gmail.com>";
static char doc[] = "Re-implementation of GNU diff.";
static char args_doc[] = "[FILE...]";

static struct argp_option options[] = {
  {"brief", 'b', 0, 0, "report only when files differ"},
  {"unified", 'u', 0, 0, "output unified file"},
  {0}
};

struct Arguments {
  char **files;
  int brief, unified;
};

static error_t parse_opts(int key, char *arg, struct argp_state *state) {
  struct Arguments *arguments = state->input;

	switch (key) {
    case 'b':
      arguments->brief = 1;
      break;
    case 'u':
      arguments->unified = 1;
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
