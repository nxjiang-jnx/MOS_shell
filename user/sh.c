#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

#define HISTFILE "/.mos_history"
#define HISTFILESIZE 20

int exitcode = 0;
int excu = 1;  // 默认执行

EnvValue localVar[100];
int shellId;

/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	// 特判 "&&"
	if (*s == '&' && *(s + 1) == '&') {
		*p1 = s;
		s[0] = 0;
		s += 2;
		*p2 = s;
		return 76;
	}

	// 特判 "||"
	if (*s == '|' && *(s + 1) == '|') {
		*p1 = s;
		s[0] = 0;
		s += 2;
		*p2 = s;
		return 248;
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe, int *leftenv) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit(-1);
			}
			argv[argc++] = t;
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit(-1);
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (1/3) */
			char abspath[MAXPATHLEN];
			relpath2abspath(abspath, t);
			char path[MAXPATHLEN];
			format_abspath(path, abspath);

			fd = open(path, O_RDONLY);
			if (fd < 0) {
				debugf("failed to open '%s'\n", t);
				exit(-1);
			}
			dup(fd, 0);
			close(fd);
			break;
		case '>':
			// 追加重定向 >>
			char *next;
			int peek = gettoken(0, &next);  // 预取下一个 token

			if (peek == '>') {
				if (gettoken(0, &t) != 'w') {
					debugf("syntax error: >> not followed by word\n");
					exit(-1);
				}
				char abspath[MAXPATHLEN];
				char path[MAXPATHLEN];
				relpath2abspath(abspath, t);
				format_abspath(path, abspath);

				fd = open(path, O_WRONLY | O_CREAT | O_APPEND);
				if (fd < 0) {
					debugf("failed to open '%s'\n", t);
					exit(-1);
				}
				dup(fd, 1);
				close(fd);
				break;
			}

			// 普通 >
			if (peek != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit(-1);
			}
			t = next;
			// Open 't' for writing, create it if not exist and trunc it if exist, dup
			// it onto fd 1, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (2/3) */

			relpath2abspath(abspath, t);
			format_abspath(path, abspath);

			fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
			if (fd < 0) {
				debugf("failed to open '%s'\n", t);
				exit(-1);
			}
			dup(fd, 1);
			close(fd);
			break;
		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */

			r = pipe(p);
			if (r != 0) {
				debugf("pipe: %d\n", r);
				exit(-1);
			}
			r = fork();
			if (r < 0) {
				debugf("fork: %d\n", r);
				exit(-1);
			}
			*rightpipe = r;
			if (r == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe, leftenv);
			} else {
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				return argc;
			}

			break;
		case ';': 
			argv[argc] = NULL; // 当前指令切断，避免右边指令进入本次执行

			*leftenv = fork();
			if (*leftenv < 0) {
				debugf("fork: error\n");
				exit(-1);
			} else if (*leftenv == 0) {
				return argc; // 子进程执行当前指令
			} else {
				// 父进程继续解析下一个命令（右边的）
				return parsecmd(argv + argc + 1, rightpipe, leftenv);
			}
			break;
		case 76:
			// &&
		case 248:
			// ||
			// 保存当前操作符，用于之后判断是否执行
			char op = (char)(c == 76 ? '&' : '|');

			// fork 当前累积指令
			r = fork();
			if (r == 0) {
				// 子进程判断是否执行
				if (excu) return argc;
				else exit(0); // 不执行，返回 0
			} else {
				// 父进程等待并接受子进程返回值
				argc = 0; // 清空当前参数（准备下一个命令）
				syscall_ipc_recv(0);
				wait(r);
				int ret = env->env_ipc_value;

				// 根据 op 和 ret 来更新 excu
				if (op == '&') {
					excu = (ret == 0);  // &&：上一条成功则执行下一条
				} else {
					excu = (ret != 0); // ||：上一条失败才执行下一条
				}
			}
			break;
		}
	}

	return argc;
}

// 环境变量
void showVar() {
	int i;
	int r;
	char name[MAXENVLEN] = {0};
	char val[MAXENVLEN] = {0};

	// 全局
	for (i = 0; i < 100; i++) {
		r = syscall_get_set_globalVar(name, val, 6, shellId, i);
		if (r == 0) {
			printf("%s=%s\n", name, val);
		}
	}

	// 局部
	for (i = 0; i < 100; i++) {
		if (localVar[i].mode != 0) {
			printf("%s=%s\n", localVar[i].name, localVar[i].value);
		}
	}
}

int get_set_localVar(char* name, char* val, int mode, int rwMode) {
    //mode 0 is check whether has the localVar
    //mode 1 is set the var by name
    //mode 2 is delete var
    //mode 3 is get the name's val 
    //mode 4 is create a new var of name
    //mode 5 is delete var without permission
    int i;
    int flag = 0;
    if (mode == 0) {
    	// check whether has the localVar of id
    	// 0 means has
    	//-1 means has not
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
           		flag = 1;
            	break;
        	}
    	}
        if (flag == 1) {
            return 0;
        } else {
            return -1;
        }
    } else if (mode == 1) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
        		if (localVar[i].mode == 1) {
        			printf("%s is read only\n", name);
        			return -1;
				}
        		strcpy(localVar[i].value, val);
        		localVar[i].mode = rwMode > 0 ? 1 : 2;
        		return 0;	
        	}
    	}	
	} else if (mode == 2) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
               	if (localVar[i].mode == 1) {
        			printf("%s is read only\n", name);
        			return -2;
				}
        		localVar[i].mode = 0;
        		return 0;	
        	}
    	}
	} else if (mode == 3) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
        		strcpy(val, localVar[i].value);
        		return 0;	
        	}
    	}			
	} else if (mode == 4) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode == 0) {
        		localVar[i].mode =  rwMode > 0 ? 1 : 2;
        		strcpy(localVar[i].value, val);
        		strcpy(localVar[i].name, name);
        		return 0;
        	}
    	}
	} else if (mode == 5) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
        		localVar[i].mode = 0;
        		return 0;	
        	}
    	}
	}
	return -1;
}

void declare(char* nameAndVal, int mode) {
	u_int x = mode & 0x1;
	u_int READONLY = mode & 0x2;
	char name[MAXENVLEN] = {0};
	char val[MAXENVLEN] = {0};
	int i, j;
	int r;
	
	if (mode == -1) {
		showVar();
		return;
	}
	
	
	for (i = 0; nameAndVal[i] != '=' && nameAndVal[i] != 0; i++){
		name[i] = nameAndVal[i];
	}
	if (nameAndVal[i]) {
		for (j = 0, i++; nameAndVal[i]; i++, j++){
			val[j] = nameAndVal[i];
		}
	}
	
	if (x) {
		r = get_set_localVar(name, val, 0, 0);
		if (r == 0) { 
			// 存在当前 shell 下环境变量
			r = syscall_get_set_globalVar(name, val, 0, shellId, 0);
			if (r == 0) {
				// 存在全局环境变量
				r = syscall_get_set_globalVar(name, val, 1, shellId, READONLY);
				if (r) return;
				r = get_set_localVar(name, val, 5, 0);
				if (r) return;
			} else { 
				// 不存在全局环境变量
				r = syscall_get_set_globalVar(name, val, 4, shellId, READONLY);
				if (r) return;
				r = get_set_localVar(name, val, 5, 0);
				if (r) return;
			}
		} else {
			r = syscall_get_set_globalVar(name, val, 0, shellId, 0);
			if (r == 0) {
				r = syscall_get_set_globalVar(name, val, 1, shellId, READONLY);
				if (r) return;
				r = get_set_localVar(name, val, 5, 0);
			} else {
				r = syscall_get_set_globalVar(name, val, 4, shellId, READONLY);
				if (r) return;
			}
		}
	} else {
		r = syscall_get_set_globalVar(name, val, 0, shellId, 0);
		if (r == 0) {
			// 存在全局环境变量
			r = syscall_get_set_globalVar(name, val, 1, shellId, READONLY);
			if (r) return;
		} else {
			r = get_set_localVar(name, val, 0, READONLY);
			if (r == 0) { 
				// 不存在全局，存在局部
				r = get_set_localVar(name, val, 1, READONLY);
				if (r) return;
			} else {
				// 均不存在
				r = get_set_localVar(name, val, 4, READONLY);
				if (r) return;
			}
		}
	}
	
}

void unset(char* name) {
	int r;
	int i;
	r = syscall_get_set_globalVar(name, 0, 2, shellId, 0);
	if (r == 0) {
		return;
	} else if (r == -2) {
		printf(1, " unset: %s: cannot unset: readonly variable", name);
		return;
	}
	r = get_set_localVar(name, 0, 2, 0);
	if (r == -2) {
		printf(1, " unset: %s: cannot unset: readonly variable", name);
		return;
	} else if (r == 0) {
		return;
	}
}

void getShellId() {
    int fd, n;
    int r;
    int i;
    char tmp[12] = {0};
	if ((fd = open("/shellId", O_RDWR)) < 0)
    	user_panic("open %s: error in getShellId\n", fd);
    if ((r = seek(fd, 0)) < 0) {
        user_panic("error in seek getShellId\n");
    }
    n = read(fd, tmp, (long)sizeof tmp);
    int ans = 0;
    for (i = 0; tmp[i]; i++) {
    	ans = ans * 10 + tmp[i] - '0';
	}
	close(fd);
	shellId = ans;
}

void addShellIdToFile(int type) {
	int fd, n;
    int r;
    int i, j;
    int ans = shellId + type;
    char tmp[12] = {0};
    i = 0;
	ans = ans < 0 ? 0 : ans;
    while (ans) {
    	tmp[i++] = ans % 10 + '0';
    	ans /= 10;
	}
	for (j = i - 1, i = 0; i < j; i++, j--){
		char c = tmp[i];
		tmp[i] = tmp[j];
		tmp[j] = c;
	}
	if ((fd = open("/shellId", O_RDWR)) < 0)
    	user_panic("open %s: error in addShellIdToFile\n", fd);
    if ((r = seek(fd, 0)) < 0) {
        user_panic("error in seek addShellIdToFile\n");
    }
    r = write(fd, tmp, strlen(tmp));
	if (r < 0) {
		user_panic("error in writeback in addShellIdToFile\n");
	} 
	close(fd);
}

static inline int is_var_char(char c) {
    return (c == '_' || (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

void replaceEnvVar(const char *src, char *dst) {
    const char *s = src;
    char *d       = dst;
    char  name[MAXENVLEN] = {0};   /* 暂存变量名 */
    char  val[MAXENVLEN]  = {0};   /* 暂存变量值 */

    while (*s && (d - dst) < MAXENVLEN - 1) {
        if (*s != '$') {
            /* 普通字符直接复制 */
            *d++ = *s++;
            continue;
        }

        /* 现在 *s == '$' */
        s++;                       /* 跳过 '$' */
        if (!is_var_char(*s)) {    /* '$' 后不是合法首字符 ⇒ 按字面量处理 */
            *d++ = '$';
            continue;
        }

        /* 提取变量名 */
        int ni = 0;
        while (*s && is_var_char(*s) && ni < MAXENVLEN - 1) {
            name[ni++] = *s++;
        }
        name[ni] = '\0';

        /* 查找变量值：先全局后局部 */
        int r = syscall_get_set_globalVar(name, val, 3, shellId, 0);
        if (r != 0) {  /* 全局不存在再查局部 */
            r = get_set_localVar(name, val, 3, 0);
        }

        if (r == 0) {  /* 找到变量，复制其值 */
            for (char *v = val; *v && (d - dst) < MAXENVLEN - 1; v++) {
                *d++ = *v;
            }
        }
        /* 若变量不存在，则直接忽略，相当于替换为空串。
           如需保留原文本，可改为在这里把 "$" 和变量名再写回 d。*/
    }

    *d = '\0';  /* 确保结尾 */
}

// history
int hsty_num;
int hsty_now;
char tmpbuf[MAXPATHLEN];	// 用于保存当前输入行

int _readline(int fd, char *buf, int n) {
	int i = 0;
	char ch;

	while (i < n - 1) {  // 保留1个字节用于末尾 '\0'
		if (readn(fd, &ch, 1) != 1)
			break;

		if (ch == '\n') {
			break;
		}

		// 过滤 \r
		if (ch == '\r')
			continue;

		buf[i++] = ch;
	}

	buf[i] = '\0';
	return i;
}

void loadcmd_from_buf(int *p_cursor, char *dst, char *from) {
	int buf_len = strlen(dst);
	int cursor = *p_cursor;

	for (int i = 0; i < buf_len; i++)
		printf(" ");
	for (int i = 0; i < buf_len; i++)
		printf("\b");

	memset(dst, 0, 1024);
	strcpy(dst, from);

	printf("%s", dst);
	*p_cursor = strlen(dst);
}

void loadcmd(int *p_cursor, char *buf, int no) {
	int fd;
	if ((fd = open(HISTFILE, O_CREAT | O_RDONLY)) < 0) {
		user_panic("open /.mos_history, %d", fd);
	}

	char tmp[1024];
	for (int i = 0; i <= no; i++) {
		_readline(fd, tmp, 1024);
	}
	close(fd);

	loadcmd_from_buf(p_cursor, buf, tmp);
}

void init_history_state() {
	int fd;
	hsty_num = 0;
	if ((fd = open(HISTFILE, O_RDONLY)) >= 0) {
		char line[1024];
		while (hsty_num < HISTFILESIZE && _readline(fd, line, sizeof(line)) > 0) {
			hsty_num++;
		}
		close(fd);
	}
	hsty_now = hsty_num;
}

void savecmd(char *s) {
	int fd;
	static char lines[HISTFILESIZE][1024];
	static int count = 0;

	// 初始化读取已有历史记录
	if (count == 0) {
		if ((fd = open(HISTFILE, O_CREAT | O_RDONLY)) < 0) {
			user_panic("open history for read, %d", fd);
		}
		int len = 0;
		char ch;
		while (count < HISTFILESIZE && read(fd, &ch, 1) == 1) {
			if (ch == '\n') {
				lines[count][len] = '\0';
				count++;
				len = 0;
			} else {
				if (len < sizeof(lines[0]) - 1)
					lines[count][len++] = ch;
			}
		}
		close(fd);
	}

	// 超出则丢弃最早一条
	if (count == HISTFILESIZE) {
		for (int i = 1; i < HISTFILESIZE; i++) {
			for (int j = 0; j < sizeof(lines[0]); j++) {
				lines[i - 1][j] = lines[i][j];
			}
		}
		count--;
	}

	// 手动复制字符串 s 到 lines[count]
	int i = 0;
	while (s[i] && i < sizeof(lines[count]) - 1) {
		lines[count][i] = s[i];
		i++;
	}
	lines[count][i] = '\0';
	count++;

	// 重写整个历史文件
	if ((fd = open(HISTFILE, O_CREAT | O_WRONLY)) < 0) {
		user_panic("open history for write, %d", fd);
	}
	ftruncate(fd, 0);
	for (int i = 0; i < count; i++) {
		write(fd, lines[i], strlen(lines[i]));
		write(fd, "\n", 1);
	}
	close(fd);

	hsty_num = count;
	hsty_now = hsty_num;
}

// ls
int flag[256];
void ls(char *path, char *prefix) {
	int r;
	struct Stat st;

	if ((r = stat(path, &st)) < 0) {
		user_panic("stat %s: %d", path, r);
	}
	if (st.st_isdir && !flag['d']) {
		lsdir(path, prefix);
	} else {
		ls1(0, st.st_isdir, st.st_size, path);
	}
}

void lsdir(char *path, char *prefix) {
	int fd, n;
	struct File f;

	if ((fd = open(path, O_RDONLY)) < 0) {
		user_panic("open %s: %d", path, fd);
	}
	while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
		if (f.f_name[0]) {
			ls1(prefix, f.f_type == FTYPE_DIR, f.f_size, f.f_name);
		}
	}
	if (n > 0) {
		user_panic("short read in directory %s", path);
	}
	if (n < 0) {
		user_panic("error reading directory %s: %d", path, n);
	}
}

void ls1(char *prefix, u_int isdir, u_int size, char *name) {
	char *sep;

	if (flag['l']) {
		printf("%11d %c ", size, isdir ? 'd' : '-');
	}
	if (prefix) {
		if (prefix[0] && prefix[strlen(prefix) - 1] != '/') {
			sep = "/";
		} else {
			sep = "";
		}
		printf("%s%s", prefix, sep);
	}
	printf("%s", name);
	if (flag['F'] && isdir) {
		printf("/");
	}
	printf(" ");
}

void runcmd(char *s) {
	gettoken(s, 0);

	char *argv[MAXARGS];
	int rightpipe = 0;
	int leftenv = 0;
	int argc = parsecmd(argv, &rightpipe, &leftenv);
	if (leftenv) {
		wait(leftenv);
	}

	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	char target[MAXARGS][MAXENVLEN] = {0};

	for (int i = 0; i < argc; i++) {
		replaceEnvVar(argv[i], target[i]);
		argv[i] = target[i];
	}

	// 内建命令
	if (!excu) return;
	if (strcmp(argv[0], "pwd") == 0) {
		if (argc != 1) {
			printf("pwd: expected 0 arguments; got %d\n", argc - 1);
			exitcode = 2;
			exit(2);
		} else {
			char path[MAXPATHLEN];
			getcwd(path);
			printf("%s\n", path);
			exitcode = 0;
			exit(0);
		}
		return;
	} else if (strcmp(argv[0], "cd") == 0) {
		if (argc > 2) {
			printf("Too many args for cd command\n");
			exitcode = 1;
			exit(1);
			return;
		}

		const char *target = (argc == 1) ? "/" : argv[1];
		char abspath[MAXPATHLEN];
		relpath2abspath(abspath, target);

		char formatedpath[MAXPATHLEN];
		format_abspath(formatedpath, abspath);

		struct Stat stat_buf;
		if (stat(formatedpath, &stat_buf) < 0) {
			printf("cd: The directory '%s' does not exist\n", target);
			exitcode = 1;
			exit(1);
			return;
		}
		if (!stat_buf.st_isdir) {
			printf("cd: '%s' is not a directory\n", target);
			exitcode = 1;
			exit(1);
			return;
		}

		syscall_set_env_curpath(env->env_parent_id, formatedpath);
		exitcode = 0;
		exit(0);
		return;
	} else if (strcmp(argv[0], "mkdir") == 0 || strcmp(argv[0], "/mkdir") == 0) {
		int recursive = 0;
		const char *dir;

		if (argc == 3 && strcmp(argv[1], "-p") == 0) {
			recursive = 1;
			dir = argv[2];
		} else {
			dir = argv[1];
		}

		char abspath[MAXPATHLEN];
		relpath2abspath(abspath, dir);
		char path[MAXPATHLEN];
		format_abspath(path, abspath);

		if (recursive == 1) {
			// 创建递归目录 mkdir -p

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
						exitcode = 1;
						exit(1);
						return;
					}
				} else if (!st.st_isdir) {
					printf("mkdir: cannot create directory '%s': Not a directory\n", tmp);
					exitcode = 1;
					exit(1);
					return;
				}
				*p = old;
				if (*p) p++;
			}
			exitcode = 0;
			exit(0);
			return;
		} else {
			// mkdir 普通模式
			struct Stat st;
			if (stat(path, &st) == 0) {
				printf("mkdir: cannot create directory '%s': File exists\n", dir);
				exitcode = 1;
				exit(1);
				return;
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
					exitcode = 1;
					exit(1);
					return;
				}
			}

			if (mkdir(path) < 0) {
				printf("mkdir: cannot create directory '%s'\n", dir);
				exitcode = 1;
				exit(1);
				return;
			}

			exitcode = 0;
			exit(0);
			return;
		}
	} else if (strcmp(argv[0], "touch") == 0 || strcmp(argv[0], "/touch") == 0) {
		if (argc != 2) {
			printf("Usage: touch <file>\n");
			exitcode = 1;
			exit(exitcode);
			return;
		}

		const char *target = argv[1];
		char abspath[MAXPATHLEN];
		relpath2abspath(abspath, target);

		char path[MAXPATHLEN];
		format_abspath(path, abspath);

		struct Stat st;
		if (stat(path, &st) == 0) {
			// 文件已存在，什么都不做
			exitcode = 0;
			exit(exitcode);
			return;
		}

		// 检查父目录是否存在
		char parent[MAXPATHLEN];
		strcpy(parent, path);

		char *last_slash = NULL;
		for (char *p = parent; *p; p++) {
			if (*p == '/'){ last_slash = p;}
		}

		if (last_slash && last_slash != parent) {
			*last_slash = '\0';
			struct Stat pst;
			if (stat(parent, &pst) < 0 || !pst.st_isdir) {
				printf("touch: cannot touch '%s': No such file or directory\n", target);
				exitcode = 1;
				exit(exitcode);
				return;
			}
		}

		if (touch(path) < 0) {
			printf("touch: cannot touch '%s'\n", target);
			exitcode = 1;
			exit(exitcode);
			return;
		}

		exitcode = 0;
		exit(exitcode);
		return;
	} else if (strcmp(argv[0], "rm") == 0 || strcmp(argv[0], "/rm") == 0) {
		int recursive = 0, force = 0;
		int target_index = 1;

		// 处理选项
		if (argc >= 2 && argv[1][0] == '-') {
			if (strchr(argv[1], 'r')) recursive = 1;
			if (strchr(argv[1], 'f')) force = 1;
			target_index = 2;
		}

		if (argc <= target_index) {
			printf("Usage: rm [-r] [-f] <file or dir>\n");
			exitcode = 1;
			exit(exitcode);
			return;
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
				exitcode = 1;
			} else {
				exitcode = 0;
			}
			exit(exitcode);
			return;
		}

		if (!recursive && st.st_isdir) {
			printf("rm: cannot remove '%s': Is a directory\n", target);
			exitcode = 1;
			exit(exitcode);
			return;
		}

		// 调用底层删除接口
		if (remove(path) < 0) {
			if (!force) {
				printf("rm: cannot remove '%s'\n", target);
				exitcode = 1;
			} else {
				exitcode = 0;
			}
			exit(exitcode);
			return;
		}

		exitcode = 0;
		exit(exitcode);
		return;
	} else if (strcmp(argv[0], "exit") == 0) {
		exitcode = 0;
		exit(0);
	} else if (strcmp(argv[0], "declare") == 0) {
		int mode = 0;
		int argi = 1;

		// 解析 -x -r 选项
		while (argi < argc && argv[argi][0] == '-') {
			if (strchr(argv[argi], 'x')) mode |= 1;
			if (strchr(argv[argi], 'r')) mode |= 2;
			argi++;
		}

		// 无参数，打印变量
		if (argi == argc) {
			declare(0, -1);
			exitcode = 0;
			exit(exitcode);
			return;
		}

		// 设置变量
		for (; argi < argc; argi++) {
			declare(argv[argi], mode);
		}
		exitcode = 0;
		exit(exitcode);
		return;
	} else if (strcmp(argv[0], "unset") == 0) {
		if (argc < 2) {
			printf("unset: missing variable name\n");
			exitcode = 1;
			exit(exitcode);
			return;
		}
		for (int i = 1; i < argc; i++) {
			unset(argv[i]);
		}
		exitcode = 0;
		exit(exitcode);
		return;
	} else if (strcmp(argv[0], "sh") == 0 || strcmp(argv[0], "sh.b") == 0 || strcmp(argv[0], "/sh.b") == 0) {
		addShellIdToFile(1);
	} else if (strcmp(argv[0], "history") == 0) {
		int fd = open(HISTFILE, O_RDONLY);
		if (fd < 0) {
			printf("No history.\n");
			exit(-1);
			return;
		}
		char line[1024];
		while (read(fd, line, sizeof(line)) > 0) {
			printf("%s", line);
		}
		close(fd);
		exit(0);
		return;
	} else if (strcmp(argv[0], "ls") == 0 || strcmp(argv[0], "/ls") == 0) {
		int i;

		char **pargv = argv + 1;  // 跳过程序名
		int pargc = argc - 1;

			while (pargc > 0 && pargv[0][0] == '-' && pargv[0][1]) {
			char *opt = &pargv[0][1];
			if (opt[0] == '-' && opt[1] == '\0') {
				pargc--;
				pargv++;
				break;
			}
			while (*opt) {
				char optc = *opt++;
				switch (optc) {
					case 'd':
					case 'F':
					case 'l':
						flag[(u_char)optc]++;
						break;
					default:
						usage();
				}
			}
			pargc--;
			pargv++;
		}

		if (pargc == 0) {
			ls("/", "");
		} else {
			for (i = 0; i < pargc; i++) {
				ls(pargv[i], pargv[i]);
			}
		}
		printf("\n");
		exit(0);
	}

	// 处理无 .b 后缀命令
	char altcmd[MAXPATHLEN];
	int len = strlen(argv[0]);

	int has_dot_b = 0;
	if (len >= 2 && argv[0][len - 2] == '.' && argv[0][len - 1] == 'b') {
		has_dot_b = 1;
	}

	if (!has_dot_b) {
		int i = 0;
		while (argv[0][i]) {
			altcmd[i] = argv[0][i];
			i++;
		}
		altcmd[i++] = '.';
		altcmd[i++] = 'b';
		altcmd[i] = '\0';
		argv[0] = altcmd;
	}	

	int child = spawn(argv[0], argv);
	close_all();
	if (child >= 0) {
		wait(child);
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	exit(exitcode);
}

void readline(char *buf, u_int n) {
	int len = 0;        // 当前缓冲区长度
	int pos = 0;        // 当前光标位置
	int r;
	char ch;
	const int prompt_len = 2;

	while (1) {
		r = read(0, &ch, 1);
		if (r < 1) continue;

		if (ch == '\r' || ch == '\n') {
			buf[len] = '\0';
			printf("\n");
			return;
		}

		else if (ch == 0x01) { // Ctrl-A
			pos = 0;
		}

		else if (ch == 0x05) { // Ctrl-E
			pos = len;
		}

		else if (ch == 0x0b) { // Ctrl-K
			len = pos;
		}

		else if (ch == 0x15) { // Ctrl-U
			if (pos > 0) {
				for (int i = 0; i < len - pos; ++i)
					buf[i] = buf[i + pos];
				len -= pos;
				pos = 0;
			}
		}

		else if (ch == 0x17) { // Ctrl-W
			if (pos > 0) {
				// 先跳过前面的空格
				int i = pos;
				while (i > 0 && buf[i - 1] == ' ') i--;
				// 再跳过前面的 word
				while (i > 0 && buf[i - 1] != ' ') i--;

				int delete_len = pos - i;
				for (int j = i; j < len - delete_len; ++j)
					buf[j] = buf[j + delete_len];

				pos = i;
				len -= delete_len;
			}
		}

		else if (ch == 0x7f || ch == '\b') { // Backspace
			if (pos > 0) {
				for (int i = pos - 1; i < len - 1; ++i)
					buf[i] = buf[i + 1];
				len--;
				pos--;
			}
		}

		else if (ch == 27) {
			char seq[2];
			if (read(0, &seq[0], 1) != 1) continue;
			if (read(0, &seq[1], 1) != 1) continue;

			if (seq[0] == '[') {
				if (seq[1] == 'A') { // ↑
					printf("\n$ ");
					if (hsty_now == hsty_num) {
						// 记录当前行内容
						memset(tmpbuf, 0, MAXPATHLEN);
						for (int i = 0; i < len; i++) {
							tmpbuf[i] = buf[i];
						}
					}
					if (hsty_now > 0) {
						hsty_now--;
						loadcmd(&pos, buf, hsty_now);
						len = strlen(buf);  // 更新长度
					}
				} else if (seq[1] == 'B') { // ↓
					printf("\r$ ");
					if (hsty_now < hsty_num)
						hsty_now++;
					if (hsty_now == hsty_num) {
						loadcmd_from_buf(&pos, buf, tmpbuf);
					} else {
						loadcmd(&pos, buf, hsty_now);
					}
					len = strlen(buf);  // 更新长度
				} else if (seq[1] == 'C') { // →
					if (pos < len) {
						printf("\e[C");
						pos++;
					}
				} else if (seq[1] == 'D') { // ←
					if (pos > 0) {
						printf("\e[D");
						pos--;
					}
				}
			}
		}

		else if (ch >= 32 && ch <= 126) { // 可打印字符
			if (len >= n - 1) continue;
			for (int i = len; i > pos; --i)
				buf[i] = buf[i - 1];
			buf[pos] = ch;
			len++;
			pos++;
		}

		// 重绘行
		printf("\r$ ");
		for (int i = 0; i < len; ++i) printf("%c", buf[i]);
		printf(" ");
		printf("\r");
		for (int i = 0; i < prompt_len + pos; ++i)
			printf("%c", (i < prompt_len) ? "$ "[i] : buf[i - prompt_len]);
	}
}

// 反引号
void replaceBackquote(char *buf) {
	char result[1024] = {0};
	char temp[1024] = {0};
	int i = 0, j = 0;
	while (buf[i]) {
		if (buf[i] == '`') {
			i++;
			int k = 0;
			while (buf[i] && buf[i] != '`') {
				temp[k++] = buf[i++];
			}
			temp[k] = '\0';
			if (buf[i] != '`') {
				debugf("syntax error: unmatched backquote\n");
				exit(-1);
			}
			i++; // skip closing backquote

			// 执行 temp 命令，捕获 stdout 输出
			int p[2];
			pipe(p);
			int pid = fork();
			if (pid < 0) {
				debugf("fork error\n");
				exit(-1);
			} else if (pid == 0) {
				close(p[0]);
				dup(p[1], 1); // stdout 重定向到写端
				close(p[1]);
				runcmd(temp);
				exit(-1); // 子进程退出
			} else {
				close(p[1]);
				char ch;
				while (read(p[0], &ch, 1) == 1) {
					if (ch != '\n') // 去除换行
						result[j++] = ch;
					else
						result[j++] = ' '; // 或者直接替换为 ' '
				}
				close(p[0]);
				wait(pid);
			}
		} else {
			result[j++] = buf[i++];
		}
	}
	result[j] = '\0';
	strcpy(buf, result); // 替换原 buf
}

char buf[1024];

void usage(void) {
	printf("usage: sh [-ix] [script-file]\n");
	exit(-1);
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;

	printf("\033[38;5;51m");
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("\033[0m"); 
	printf("\033[1mWelcome to JNX's \033[38;5;213mMOS\033[0m\033[1m!\n\033[0m");

	printf("\033[38;5;220m"); 
	printf("          _____                   _______                   _____          \n");
	printf("         /\\    \\                 /::\\    \\                 /\\    \\         \n");
	printf("        /::\\____\\               /::::\\    \\               /::\\    \\        \n");
	printf("       /::::|   |              /::::::\\    \\             /::::\\    \\       \n");
	printf("      /:::::|   |             /::::::::\\    \\           /::::::\\    \\      \n");
	printf("     /::::::|   |            /:::/~~\\:::\\    \\         /:::/\\:::\\    \\     \n");
	printf("    /:::/|::|   |           /:::/    \\:::\\    \\       /:::/__\\:::\\    \\    \n");
	printf("   /:::/ |::|   |          /:::/    / \\:::\\    \\      \\:::\\   \\:::\\    \\   \n");
	printf("  /:::/  |::|___|______   /:::/____/   \\:::\\____\\   ___\\:::\\   \\:::\\    \\  \n");
	printf(" /:::/   |::::::::\\    \\ |:::|    |     |:::|    | /\\   \\:::\\   \\:::\\    \\ \n");
	printf("/:::/    |:::::::::\\____\\|:::|____|     |:::|    |/::\\   \\:::\\   \\:::\\____\\\n");
	printf("\\::/    / ~~~~~/:::/    / \\:::\\    \\   /:::/    / \\:::\\   \\:::\\   \\::/    /\n");
	printf(" \\/____/      /:::/    /   \\:::\\    \\ /:::/    /   \\:::\\   \\:::\\   \\/____/ \n");
	printf("             /:::/    /     \\:::\\    /:::/    /     \\:::\\   \\:::\\    \\     \n");
	printf("            /:::/    /       \\:::\\__/:::/    /       \\:::\\   \\:::\\____\\    \n");
	printf("           /:::/    /         \\::::::::/    /         \\:::\\  /:::/    /    \n");
	printf("          /:::/    /           \\::::::/    /           \\:::\\/:::/    /     \n");
	printf("         /:::/    /             \\::::/    /             \\::::::/    /      \n");
	printf("        /:::/    /               \\::/____/               \\::::/    /       \n");
	printf("        \\::/    /                 ~~                      \\::/    /        \n");
	printf("         \\/____/                                           \\/____/         \n");
	printf("\033[0m"); 

	printf("\n\033[1m\033[38;5;213mMOS\033[0m\033[1m SHELL 2025. & ENHANCED BY \033[38;5;117mJNX\033[0m\n");
	printf("6/15/2025\n");
	printf("-Commands: cat ls echo exit halt touch mkdir rm cd pwd history declare unset sh\n");
	printf("-Other functions: > >> < | && || ` ; ↑ ↓ ← →\n");

	printf("\033[38;5;51m");
	printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("\033[0m"); 

	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[0], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[0], r);
		}
		user_assert(r == 0);
	}

	getShellId();
	r = syscall_get_set_globalVar(0, 0, 5, shellId, 0);
	r = syscall_mem_map(0, (void*)localVar, 0, (void*)localVar, PTE_V | PTE_D | PTE_COW);

	init_history_state();

	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}
		readline(buf, sizeof(buf));

		// 记录历史
		if (buf[0] != '\0') {
			savecmd(buf);
		}
		
		// 反引号
		replaceBackquote(buf);

		// 裁剪行内注释：忽略 # 及其之后内容（若 # 不在引号中）
		int in_single_quote = 0, in_double_quote = 0;
		for (int i = 0; buf[i]; i++) {
			if (buf[i] == '\'' && !in_double_quote) {
				in_single_quote ^= 1;
			}
			if (buf[i] == '\"' && !in_single_quote) {
				in_double_quote ^= 1;
			}
			if (buf[i] == '#' && !in_single_quote && !in_double_quote) {
				buf[i] = '\0';
				break;
			}
		}

		if (buf[0] == '#') {
			continue;
		}
		if (strcmp(buf, "exit") == 0) {
			r = syscall_get_set_globalVar(0,0,7,shellId,0);
			addShellIdToFile(-1);
			exit(-1);
			break;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}

		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
			exit(-1);
		} else {
			wait(r);
		}
	}
	return 0;
}
