#include <args.h>
#include <lib.h>

int main(int argc, char **argv) {
	int recursive = 0, force = 0;
	int target_index = 1;

	if (argc < 2) {
		printf("Usage: rm [-r] [-f] <file or dir>\n");
		return 1;
	}

	if (argv[1][0] == '-') {
		if (strchr(argv[1], 'r')) recursive = 1;
		if (strchr(argv[1], 'f')) force = 1;
		target_index = 2;
	}

	if (argc <= target_index) {
		printf("Usage: rm [-r] [-f] <file or dir>\n");
		return 1;
	}

	const char *target = argv[target_index];

	char abspath[MAXPATHLEN];
	relpath2abspath(abspath, target);
	char path[MAXPATHLEN];
	format_abspath(path, abspath);

	struct Stat st;
	if (stat(path, &st) < 0) {
		if (!force) {
			printf("rm: cannot remove '%s': No such file or directory\n", target);
			return 1;
		}
		return 0;
	}

	if (!recursive && st.st_isdir) {
		printf("rm: cannot remove '%s': Is a directory\n", target);
		return 1;
	}

	if (remove(path) < 0) {
		if (!force) {
			printf("rm: cannot remove '%s'\n", target);
			return 1;
		}
	}

	return 0;
}
