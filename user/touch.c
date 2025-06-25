#include <args.h>
#include <lib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: touch <file>\n");
		return 1;
	}

	const char *target = argv[1];
	char abspath[MAXPATHLEN];
	relpath2abspath(abspath, target);

	char path[MAXPATHLEN];
	format_abspath(path, abspath);

	struct Stat st;
	if (stat(path, &st) == 0) {
		// 文件已存在，什么都不做
		return 0;
	}

	// 检查父目录是否存在
	char parent[MAXPATHLEN];
	strcpy(parent, path);
	char *last_slash = NULL;
	for (char *p = parent; *p; p++) {
		if (*p == '/') last_slash = p;
	}
	if (last_slash && last_slash != parent) {
		*last_slash = '\0';
		struct Stat pst;
		if (stat(parent, &pst) < 0 || !pst.st_isdir) {
			printf("touch: cannot touch '%s': No such file or directory\n", target);
			return 1;
		}
	}

	if (touch(path) < 0) {
		printf("touch: cannot touch '%s'\n", target);
		return 1;
	}

	return 0;
}
