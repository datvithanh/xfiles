#include <argp.h>
#include <error.h>
#include <string.h>

#define OPT_RETRY 1

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "<phan.ngoclan58@gmail.com>";
static char doc[] = "Re-implementation of GNU split.";
static char args_doc[] = "[FILE [PREFIX]]";

static struct argp_option options[] = {
	{"bytes", 'b', "SIZE", 0, "Put SIZE bytes per output file"},
	{"number", 'n', "CHUNKS", 0, "Generate CHUNKS output files"},
	{"suffix-length", 'a', "N", 0, "Generate suffixes of length N (default 2)"},
	{0}
};

struct Arguments {
	char *file;
	char *prefix;
	int chunks;
	int bytes;
	int suffix_len;
};

int parse_size(const char* size);

static error_t parse_opts(int key, char *arg, struct argp_state *state) {
	struct Arguments *arguments = state->input;

	switch (key) {
		case 'b':
			if (!arg) return ARGP_ERR_UNKNOWN;
			arguments->bytes = parse_size(arg);
			break;
		case 'n':
			if (!arg) return ARGP_ERR_UNKNOWN;
			arguments->chunks = atoi(arg);
			break;
		case 'a':
			if (!arg) return ARGP_ERR_UNKNOWN;
			arguments->suffix_len = atoi(arg);
			break;
		case ARGP_KEY_NO_ARGS:
			argp_usage(state);
		case ARGP_KEY_ARG: 
			arguments->file = state->argv[state->next - 1];
			if (state->next == state->argc - 1) {
				arguments->prefix = state->argv[state->next];
			}
			state->next = state->argc;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int parse_size(const char* size) {
	return atoi(size);
}
