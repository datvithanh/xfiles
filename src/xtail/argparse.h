#include <argp.h>
#include <error.h>
#include <string.h>

#define OPT_RETRY 1

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "<phan.ngoclan58@gmail.com>";
static char doc[] = "Re-implementation of GNU tail.";
static char args_doc[] = "[FILE...]";

static struct argp_option options[] = {
	{"follow", 'f', "[=descriptor|name]", 0, "Output appended data as the file grows. Defaults to descriptor."},
	{"lines", 'n', "NUM", 0, "Output the last NUM lines. Defaults to 10"},
	{"sleep-interval", 's', "N", 0, "Sleep for approximately N seconds (default  1.0) between iterations"},
	{"quiet", 'q', 0, 0, "Never output headers"},
	{"verbose", 'v', 0, 0, "Always output headers"},
	{"retry", OPT_RETRY, 0, 0, "Keep trying to open a file if it's inaccessible"},
	{0}
};

struct Arguments {
	char **files;
	char* follow;
	int lines;
	int quiet, verbose, retry, sleep;
};

static error_t parse_opts(int key, char *arg, struct argp_state *state) {
	struct Arguments *arguments = state->input;

	switch (key) {
		case 'q':
			arguments->quiet = 1;
			break;
		case 's':
			if (!arg) return ARGP_ERR_UNKNOWN;
			arguments->sleep = atoi(arg);
			if (arguments->lines < 1) {
				fprintf(stderr, "%d is not a valid sleep interval", arguments->sleep);
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case 'f':
			arguments->follow = arg;
			if (strcmp(arguments->follow, "descriptor") || strcmp(arguments->follow, "name")) {
				arguments->follow = "descriptor";
				state->next--;
			}
			break;
		case 'v':
			arguments->verbose = 1;
			break;
		case 'n':
			if (!arg) return ARGP_ERR_UNKNOWN;
			arguments->lines = atoi(arg);
			if (arguments->lines < 0) {
				fprintf(stderr, "%d is not a valid number of lines", arguments->lines);
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case OPT_RETRY:
			arguments->retry = 1;
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
