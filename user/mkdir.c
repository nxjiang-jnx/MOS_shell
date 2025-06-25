#include <args.h>
#include <lib.h>

int main(int argc, char **argv) {
	int recursive = 0;
	const char *dir;

	if (argc == 3 && strcmp(argv[1], "-p") == 0) {
		recursive = 1;
		dir = argv[2];
	} else if (argc == 2) {
		dir = argv[1];
	} else {
		printf("Usage: mkdir [-p] <dir>\n");
		exit(1);
		return 1;
	}

	char abspath[MAXPATHLEN];
	relpath2abspath(abspath, dir);
	char path[MAXPATHLEN];
	format_abspath(path, abspath);

	if (recursive) {
		// 递归创建目录
		char tmp[MAXPATHLEN];
		strcpy(tmp, path);
		char *p = tmp + 1;

		while (*p) {
			while (*p && *p != '/') p++;
			char old = *p;
			*p = '\0';

			struct Stat st;
			if (stat(tmp, &st) < 0) {
				if (mkdir(tmp) < 0) {
					printf("mkdir: cannot create directory '%s'\n", tmp);
					exit(1);
					return 1;
				}
			} else if (!st.st_isdir) {
				printf("mkdir: cannot create directory '%s': Not a directory\n", tmp);
				exit(1);
				return 1;
			}
			*p = old;
			if (*p) p++;
		}
		return 0;
	} else {
		// 普通 mkdir
		struct Stat st;
		if (stat(path, &st) == 0) {
			printf("mkdir: cannot create directory '%s': File exists\n", dir);
			exit(1);
			return 1;
		}

		// 检查父目录是否存在
		char parent[MAXPATHLEN];
		strcpy(parent, path);
		char *last_slash = NULL;
		for (char *p = parent; *p; p++) {
			if (*p == '/') {
				last_slash = p;
			}
		}
		if (last_slash && last_slash != parent) {
			*last_slash = '\0';
			struct Stat pst;
			if (stat(parent, &pst) < 0 || !pst.st_isdir) {
				printf("mkdir: cannot create directory '%s': No such file or directory\n", dir);
				exit(1);
				return 1;
			}
		}

		if (mkdir(path) < 0) {
			printf("mkdir: cannot create directory '%s'\n", dir);
			exit(1);
			return 1;
		}

		return 0;
	}
}
