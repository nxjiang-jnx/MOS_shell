# BUAA_OS_Lab6_shell 挑战性任务笔记

[TOC]

先看看我的 shell！！！

```c
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Welcome to JNX's MOS!
          _____                   _______                   _____          
         /\    \                 /::\    \                 /\    \         
        /::\____\               /::::\    \               /::\    \        
       /::::|   |              /::::::\    \             /::::\    \       
      /:::::|   |             /::::::::\    \           /::::::\    \      
     /::::::|   |            /:::/~~\:::\    \         /:::/\:::\    \     
    /:::/|::|   |           /:::/    \:::\    \       /:::/__\:::\    \    
   /:::/ |::|   |          /:::/    / \:::\    \      \:::\   \:::\    \   
  /:::/  |::|___|______   /:::/____/   \:::\____\   ___\:::\   \:::\    \  
 /:::/   |::::::::\    \ |:::|    |     |:::|    | /\   \:::\   \:::\    \ 
/:::/    |:::::::::\____\|:::|____|     |:::|    |/::\   \:::\   \:::\____\
\::/    / ~~~~~/:::/    / \:::\    \   /:::/    / \:::\   \:::\   \::/    /
 \/____/      /:::/    /   \:::\    \ /:::/    /   \:::\   \:::\   \/____/ 
             /:::/    /     \:::\    /:::/    /     \:::\   \:::\    \     
            /:::/    /       \:::\__/:::/    /       \:::\   \:::\____\    
           /:::/    /         \::::::::/    /         \:::\  /:::/    /    
          /:::/    /           \::::::/    /           \:::\/:::/    /     
         /:::/    /             \::::/    /             \::::::/    /      
        /:::/    /               \::/____/               \::::/    /       
        \::/    /                 ~~                      \::/    /        
         \/____/                                           \/____/         

MOS SHELL 2025. & ENHANCED BY JNX
6/15/2025
-Commands: cat ls echo exit halt touch mkdir rm cd pwd history declare unset sh
-Other functions: > >> < | && || ` ; ↑ ↓ ← →
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

$ 
```

2025年春 Lab6 shell 挑战性任务共16个子任务，所有任务全部完成才能在评测中拿到满分。每个任务相对独立，平均下来每个任务的代码量和思考难度和上机考试 extra 难度相抵。

横向对比各个挑战性人物，shell 任务有点像时间战术的感觉。有的任务侧重在思维设计含量，只要设计完成便可通过所有测试点。而本任务是迭代开发多个相对独立的功能，必须久久为功，耗费大量的时间和经历。

**个人总体开发时间约为 30 小时左右，新增代码量约为 1200 行。**总体而言，从以下几个维度评价各个任务：

- 整体难度：1-5 星，参考平时考试 exam 为1星左右，extra 为3星左右；
- 个人耗时：顺利完成所有需求理解、开发、调试到最终正确实现的总用时；
- AI 辅助建模有效度：AI在辅助开发当前任务的有效程度。0%代表没有任何有价值信息帮助，100%表示投喂需求稍加迭代即可完整辅助完成；
- 个人评述。

| 需求名称         | 整体难度 | 个人耗时（小时） | AI 辅助建模有效度 | 个人评述                                                     |
| ---------------- | -------- | ---------------- | ----------------- | ------------------------------------------------------------ |
| 内建指令pwd      | ⭐        | 0.5              | 60%               | 开山奠基之作，其中为 `env` 结构体新增的当前目录字段在后续有广泛的应用。 |
| 内建指令 cd      | ⭐⭐⭐      | 1.5              | 50%               | 上机考试 extra 的难度和工作量。解析相对路径的2个核心函数在后续有广泛的应用。 |
| 指令 mkdir       | ⭐⭐⭐⭐     | 1.5              | 80%               | 上机考试 extra 的难度和工作量。分类考虑各种情况。            |
| 指令 touch       | ⭐⭐⭐      | 1.5              | 60%               | 上机考试 extra 的难度和工作量。学会遍历父目录。              |
| 指令 rm          | ⭐⭐       | 1                | 60%               | 上机考试 exam 的难度和工作量。学会调用 Lab4 的 remove。      |
| 内建指令 exit    | ⭐        | 0.25             | 100%              | 非常直接。**==（到此25分）==**                               |
| 环境变量管理     | ⭐⭐⭐⭐⭐    | 5                | 10%               | 上机考试 extra 的2倍难度和工作量。个人认为最难。             |
| 实现注释功能     | ⭐⭐       | 1                | 100%              | 比较巧妙。                                                   |
| 不带 .b 后缀指令 | ⭐⭐       | 0.5              | 80%               | 实现方式很多。最简单直接拼接。**==（到此50分）==**           |
| 指令自由输入     | ⭐⭐⭐⭐     | 2                | 70%               | 需要有一些背景知识，灵活调试。                               |
| 快捷键           | ⭐⭐⭐      | 1                | 70%               | 和自由输入本质上一样，可以两个需求一起实现                   |
| 历史指令         | ⭐⭐⭐⭐⭐    | 5                | 50%               | 上机考试 extra 的2倍难度和工作量。个人认为非常难。**==（到此75分）==** |
| 一行多指令       | ⭐⭐       | 1                | 80%               | 需要巧妙理解                                                 |
| 追加重定向       | ⭐⭐⭐      | 2.5              | 40%               | 实现代码量不大，但是细节非常多。需要时间调试。               |
| 实现反引号       | ⭐⭐⭐      | 1                | 80%               | 上机考试 exam 和 extra 难度之间。                            |
| 指令条件执行     | ⭐⭐⭐⭐     | 4                | 0%                | 上机考试 extra 的难度和工作量。**==（到此100分）==**         |

下面分指令具体给出基本实现方式。

## 内建指令pwd

功能需求：

| **输入**            | **行为**         | **输出**                             | **返回值** |
| :------------------ | :--------------- | :----------------------------------- | :--------- |
| `pwd`               | 输出当前工作目录 | `/dir1/dir2\n`                       | 0          |
| `pwd arg1 ... argn` | 参数数量错误     | `pwd: expected 0 arguments; got n\n` | 2          |

整体思路是对当前进程（进程控制块结构体 `Env`）**维护**一个 “当前目录” 属性，然后在用户态获取即可。

首先，进行全操作系统的维护：

在 `include/env.h` 中添加：

```c
// Lab 6 shell challenge
char env_curpath[MAXPATHLEN];
```

在 `kern/env.c` 的 `env_alloc` 函数中合适位置添加：

```c
strcpy(e->env_curpath, "/");
```

在 `kern/syscall_all.c` 的 `sys_exofork` 函数最后添加：

```c
strcpy(e->env_curpath, curenv->env_curpath);
```

在 `user/lib/file.c` 中新增函数：

```c
char *getcwd(char *buf) {
	strcpy(buf, env->env_curpath);
	return buf;
}
```

然后记得在 `user/include/lib.h` 中声明 `char *getcwd(char *buf);`。

最后在 `user/lib` 目录下新建内建功能文件 `pwd.c` （可选，对于内建指令不用，而是用户程序）：

```c
#include <lib.h>

int main(int argc, char **argv) {
    if (argc != 1) {
        debugf("pwd: expected 0 arguments; got %d\n", argc - 1);
        return 2;
    }

    char path[MAXPATHLEN];
    getcwd(path);
    printf("%s\n", path);
    return 0;
}
```

在 `user/include.mk` 的合理位置添加 `pwd.b \`。

内建命令要求修改 Shell 的解析逻辑。因此需要修改 `user/sh.c` 中的 `runcmd` 逻辑。先在顶部设置一个全局变量 `int exitcode = 0;`  模拟返回值，然后在尝试调用 `spawn(argv[0], argv)` 之前，先判断是否是内建指令，并直接调用对应逻辑：

```c
// 内建命令
if (strcmp(argv[0], "pwd") == 0) {
	if (argc != 1) {
		printf("pwd: expected 0 arguments; got %d\n", argc - 1);
		exitcode = 2;
	} else {
		char path[MAXPATHLEN];
		getcwd(path);
		printf("%s\n", path);
		exitcode = 0;
	}
	return;
}
```

至此该指令就全部完成了。

## 内建指令 cd

功能需求：

| **输入**            | **行为**                                                     | **输出**                                        | **返回值** |
| :------------------ | :----------------------------------------------------------- | :---------------------------------------------- | :--------- |
| `cd`                | 切换工作目录到 `/`                                           | 无                                              | 0          |
| `cd <abspath>`      | 若绝对路径 `<abspath>` 存在且为目录，切换到该目录            | 无                                              | 0          |
| `cd <relpath>`      | 根据当前工作路径，对相对路径`<relpath>`，若存在且为目录，切换到该目录 | 无                                              | 0          |
| `cd <noexist_path>` | 路径不存在                                                   | `cd: The directory '原始输入' does not exist\n` | 1          |
| `cd <filepath>`     | 路径存在但不是目录                                           | `cd: '原始输入' is not a directory\n`           | 1          |
| `cd <arg1>..<argn>` | 输入多于 1 个参数                                            | `Too many args for cd command\n`                | 1          |
| `cd .`              | 解析为当前目录 `/dir1/dir2`，无变化                          | 无                                              | 0          |
| `cd ..`             | 解析为 `/dir1`，若存在且为目录，切换到该目录                 | 无                                              | 0          |

首先，在 `user/lib/file.c` 中添加功能函数，将相对路径转化为绝对路径：

```c
void relpath2abspath(char *abspath, const char *relpath) {
	if (!relpath) {
		return;
	}
	
	if (relpath[0] != '/') {
		// 相对路径
		getcwd(abspath);
		int len = strlen(abspath);
		abspath[len++] = '/';
		strcpy(abspath + len, relpath);
	} else {
		// 绝对路径
		strcpy(abspath, relpath);	
	}
}
```

记得在 `user/include/lib.h` 中合理位置注册该函数。

接下来实现几个辅助功能。第一个是**规范化一个绝对路径字符串**，即将路径中的多余斜杠、`.` 和 `..` 等解析并折叠为标准的绝对路径形式，类似于 Unix 下的 `relpath()` 的简化版（**不访问文件系统，仅对字符串进行解析**）。在 `user/lib/file.c` 中实现以下函数，记得在`user/include/lib.h` 中注册：

```c
char *format_abspath(char *buf, char *path) {
	char *rt = path;
	char *p, *q;
	char name[MAXNAMELEN];
	buf[0] = '/';
	q = buf + 1;
	path++;
	while (*path != '\0') {
		p = path;

		while (*path != '/' && *path != '\0') {
			path++;
		}

		memcpy(name, p, path - p);
		name[path - p] = '\0';
		while (*path == '/') {
			path++;
		}
		
		if (name[0] == '\0') {
			continue;
		} else if (strcmp(name, ".") == 0) {
			continue;
		} else if (strcmp(name, "..") == 0) {
			q = (q == buf + 1 ? q : q - 2);
			while (*q != '/') {
				q--;
			}
			q++;
		} else {
			strcpy(q, name);
			q += strlen(name);
			*q = '/';
			q++;
		}
	}
	
	if (q > buf + 1) {
		*(q - 1) = '\0';
	} else {
		*q = '\0';
	}

	return rt;
}
```

第二个辅助功能是一个系统调用（Lab4 经典内容），实现设置当前进程的路径。

首先在用户态 `user/lib/sysvall_lib.c` 下实现用户态调用：

```c
int syscall_set_env_curpath(u_int envid, const char *path) {
	return msyscall(SYS_set_env_curpath, envid, path);
}
```

然后在 `include/syscall.h` 中注册 `SYS_set_env_curpath,`，并在 `kern/syscall_all.c` 的 `void *syscall_table[MAX_SYSNO]` 中注册，并具体实现：

```c
void sys_set_env_curpath(int envid, const char *path) {
	struct Env *env;

	try(envid2env(envid, &env, 0));
	
	memset(env->env_curpath, 0, sizeof(env->env_curpath));
	strcpy(env->env_curpath, path);
	return 0;
}
```

注意还要在 `user/include/lib.h` 中声明。最后，在 `user/sh.c` 中组装起来形成一个用于**更改指定进程当前工作目录**（相对路径基准）的 cd 实现：

```c
else if (strcmp(argv[0], "cd") == 0) {
	if (argc > 2) {
		printf("Too many args for cd command\n");
		exitcode = 1;
		return;
	}

	const char *target = (argc == 1) ? "/" : argv[1];
	char abspath[MAXPATHLEN];
	relpath2abspath(abspath, target);

	struct Stat stat_buf;
	if (stat(abspath, &stat_buf) < 0) {
		printf("cd: The directory '%s' does not exist\n", target);
		exitcode = 1;
		return;
	}
	if (!stat_buf.st_isdir) {
		printf("cd: '%s' is not a directory\n", target);
		exitcode = 1;
		return;
	}

	char formatedpath[MAXPATHLEN];
	format_abspath(formatedpath, abspath);
	syscall_set_env_curpath(env->env_parent_id, formatedpath);
	exitcode = 0;
	return;
}
```

## 外部与内建指令 mkdir

对于内建指令：

首先新增系统调用` fsipc_mkdir`。在 `user/include/fsreq.h` 中新增结构体：

```c
// 先在上面注册 FSREQ_MKDIR
struct Fsreq_mkdir {
	char req_path[MAXPATHLEN];
	u_int f_type;
};
```

然后在 `user/lib/fsipc.c` 中：

```c
int fsipc_mkdir(const char *path, u_int f_type, struct Fd* fd) {
	u_int perm;
	struct Fsreq_mkdir *req;

	req = (struct Fsreq_mkdir *)fsipcbuf;

	// The path is too long.
	if (strlen(path) >= MAXPATHLEN) {
		return -E_BAD_PATH;
	}

	strcpy((char *)req->req_path, path);
	req->f_type = f_type;
	return fsipc(FSREQ_MKDIR, req, fd, &perm);
}
```

记得在 `suer/include/lib.h` 中注册。然后在 `fs/serv.c` 中新增：

```c
void serve_mkdir(u_int envid, struct Fsreq_mkdir *rq) {
	struct File *f;
	int r;
		if ((r = file_create(rq->req_path, &f)) < 0) {
			ipc_send(envid, r, 0, 0);
			return;
		}
	f->f_type = rq->f_type;
	ipc_send(envid, 0, 0, 0);
}

// serve_table 中注册
```

接下来在 `user/lib/file.c` 中实现功能接口：

```c
int mkdir(const char *path) {
	fsipc_mkdir(path, FTYPE_DIR, NULL);
}
```

记得在 `user/include/lib.h` 中注册。然后在 `user/sh.c` 中实现内建指令：

```c
else if (strcmp(argv[0], "mkdir") == 0) {
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
					return;
				}
			} else if (!st.st_isdir) {
				printf("mkdir: cannot create directory '%s': Not a directory\n", tmp);
				exitcode = 1;
				return;
			}
			*p = old;
			if (*p) p++;
		}
		exitcode = 0;
		return;
	} else {
		// mkdir 普通模式
		struct Stat st;
		if (stat(path, &st) == 0) {
			printf("mkdir: cannot create directory '%s': File exists\n", dir);
			exitcode = 1;
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
				return;
			}
		}

		if (mkdir(path) < 0) {
			printf("mkdir: cannot create directory '%s'\n", dir);
			exitcode = 1;
			return;
		}

		exitcode = 0;
		return;
	}
}
```

对于外部指令：

新建 `user/mkdir.c`：

```c
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
					return 1;
				}
			} else if (!st.st_isdir) {
				printf("mkdir: cannot create directory '%s': Not a directory\n", tmp);
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
				return 1;
			}
		}

		if (mkdir(path) < 0) {
			printf("mkdir: cannot create directory '%s'\n", dir);
			return 1;
		}

		return 0;
	}
}
```

然后在 `user/include.mk` 中添加 `mkdir.b`。

## 外部与内建指令 touch

| 条件                     | 行为                   |
| ------------------------ | ---------------------- |
| 文件存在                 | 不输出，退出码为 0     |
| 文件不存在，父目录存在   | 创建空文件，退出码为 0 |
| 文件不存在，父目录不存在 | 输出错误，退出码为 1   |

对于内建版本：

首先在 `user/lib/file.c` 中实现并在 `user/include/lib.h` 中注册：

```c
int touch(const char *path) {
	fsipc_mkdir(path, FTYPE_REG, NULL);
}
```

然后在顶层实现：

```c
else if (strcmp(argv[0], "touch") == 0) {
	if (argc != 2) {
		printf("Usage: touch <file>\n");
		exitcode = 1;
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
			return;
		}
	}

	if (touch(path) < 0) {
		printf("touch: cannot touch '%s'\n", target);
		exitcode = 1;
		return;
	}

	exitcode = 0;
	return;
}
```

对于外部版本：

新建：

```c
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
```

然后新增 `touch.b`。

## 外部与内建指令 rm

| 命令                   | 行为                                     |
| ---------------------- | ---------------------------------------- |
| `rm <file>`            | 删除文件；若不存在则报错；若是目录则报错 |
| `rm <dir>`             | 报错：`Is a directory`                   |
| `rm -r <file or dir>`  | 若存在则删除文件或目录；否则报错         |
| `rm -rf <file or dir>` | 若存在则删除；不存在则**不报错**         |

对于内建版本：

由于我们已经在 Lab4 中实现了 `remove` 接口，因此只需要内建指令即可：

```c
else if (strcmp(argv[0], "rm") == 0) {
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
		return;
	}

	const char *target = argv[target_index];
	char abspath[MAXPATHLEN];
	relpath2abspath(abspath, target);

	struct Stat st;
	if (stat(abspath, &st) < 0) {
		if (!force) {
			printf("rm: cannot remove '%s': No such file or directory\n", target);
			exitcode = 1;
		} else {
			exitcode = 0;
		}
		return;
	}

	if (!recursive && st.st_isdir) {
		printf("rm: cannot remove '%s': Is a directory\n", target);
		exitcode = 1;
		return;
	}

	// 调用底层删除接口
	if (remove(abspath) < 0) {
		if (!force) {
			printf("rm: cannot remove '%s'\n", target);
			exitcode = 1;
		} else {
			exitcode = 0;
		}
		return;
	}

	exitcode = 0;
	return;
}
```

对于外部版本：

```c
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

	struct Stat st;
	if (stat(abspath, &st) < 0) {
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

	if (remove(abspath) < 0) {
		if (!force) {
			printf("rm: cannot remove '%s'\n", target);
			return 1;
		}
	}

	return 0;
}
```

并添加 `tm.b`。

## 内建指令 exit

```c
else if (strcmp(argv[0], "exit") == 0) {
	exitcode = 0;
	exit();
}
```

并在 main 中添加：

```c
if (strcmp(buf, "exit") == 0) {
	break;
}
```

## 环境变量管理

**这是我认为该挑战性任务中最复杂的部分。**凭借我的开发能力，从理解需求到最终正确完成该任务耗时 10 小时左右。个人认为工作量相当于一次 OO 作业，难度相当于 Lab5 extra 的 2 倍。

涉及到 **shell 和 Lab4 fork 的诸多细节**，尤其是许多课上考试没有涉及到的细节，必须全部理解才能完全正确。这里梳理一下**最为困难实现的点**：

- 加 -x 子 shell 和父 shell 共享环境变量，即执行 sh 后 declare 能够显示。
- 不加 -x 说明该环境变量只存在于当前 shell 中，不继承，不延续，即执行 sh 后 declare 不能显示，如果有父 shell，父 shell 中也不存在该环境变量。
- -r 权限的正确实现。
- 在命令中展开变量的值时候，需要考虑**全面情况的鲁棒性**，例如 `$.../$.../...$... $...`。

从具体技术角度，上述四个非常困难的点可以归结于以下几个**具体技术难点**：

- 要理解 fork 的逻辑，熟悉 fork.c 文件中的几个操作，尤其是熟悉 `duppage` 函数的实现（Lab4）。知道我们的 shell 中实际上每个输入的指令都是经历一次 fork 来实现，清楚 fork 前后共享什么，**认同 fork 前后都位于当前shell之中**，**共享所有页面和内存**，**不论 -x 与否都要共享访问**，通过几个系统调用来实现该行为。
- 理解什么时候切换 shell，理解 sh 指令的逻辑，明白 shell 继承过程中共享什么、不共享什么，和fork有什么关系；推理出**需要为每个shell设置独一无二的 id**，需要在**整个 MOS 全局**维护全局的环境变量，在各个 shell 内部维护局部的环境变量，明白上述描述如何实现。需要诸多辅助函数和系统调用。
- 对于命令中展开变量的值，需要**非常全面细致的字符串处理**和情况的考虑。

回顾一下需求描述：

> MOS 中的Shell目前并不支持环境变量,你需要在shell中增加对环境变量的支持。
>
> 规定环境变量在命令中以`$`开头，名称与C语言变量命名要求，且长度不超过16，环境变量的值长度同样不超过16。环境变量可分为局部变量与非局部变量，仅非局部变量可传入子Shell中，并且只有非只读变量可被修改。
>
> 你需要实现以下内容:
>
> 1. 支持 内建指令`declare [-xr] [NAME [=VALUE]]` ，其中：
>    - `-x` 表示变量 `NAME` 为环境变量，否则为局部变量。
>      - 环境变量对子 shell 可见，也就是说在 shell 中输入 `sh.b` 启动一个子 shell 后，可以读取并修改 `NAME` 的值，即支持环境变量的继承。
>      - 局部变量对子 shell **不可见**，也就是说在 shell 中输入 `sh.b` 启动一个子 shell 后，没有该局部变量。
>    - `-r` 表示将变量 `NAME` 设为只读。只读变量不能被 `declare` 重新赋值或被 `unset` 删除。
>    - 如果变量 `NAME` 不存在，需要创建该环境变量；如果变量 `NAME` 存在，将该变量赋值为 `VALUE`。
>    - 其中 `VALUE` 为可选参数，缺省时将该变量赋值为空字符串即可。
>    - 如果没有 `[-xr]` 及 `[NAME [=VALUE]]` 部分，即只输入 `declare`，则输出当前 shell 的所有变量，包括局部变量和环境变量。
> 2. 支持内建指令 `unset NAME` 命令，若变量 `NAME` 不是只读变量，则删除变量 `NAME`。
> 3. 支持在命令中展开变量的值，如使用 `echo.b $NAME` 打印变量 `NAME` 的值。
>
> 注意事项：当执行`declare`指令时，需要以`<var>=<val>`(`<var>`为环境变量名,`<val>`为对应的值)输出当前Shell中的所有环境变量，评测不会对输出顺序进行测试。子Shell对环境变量的修改不会影响父Shell，且上述指令不能正确执行时返回非零值。

涉及到诸多文件需要修改，代码量相对很大，且非常具有技术含量，因此不一一列出，而是给出sh.c中的关键操作：

```c
void runcmd(char *s) {
    //...
    for (int i = 0; i < argc; i++) {
		replaceEnvVar(argv[i], target[i]);
		argv[i] = target[i];
	}
    
    // ...
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
			return;
		}

		// 设置变量
		for (; argi < argc; argi++) {
			declare(argv[argi], mode);
		}
		exitcode = 0;
		return;
	} else if (strcmp(argv[0], "unset") == 0) {
		if (argc < 2) {
			printf("unset: missing variable name\n");
			exitcode = 1;
			return;
		}
		for (int i = 1; i < argc; i++) {
			unset(argv[i]);
		}
		exitcode = 0;
		return;
	} else if (strcmp(argv[0], "sh") == 0 || strcmp(argv[0], "sh.b") == 0 || strcmp(argv[0], "/sh.b") == 0) {
		addShellIdToFile(1);
	}
	// ...
}

// 打印所有环境变量，注意全局+局部
void showVar();

// 设置或获取局部环境变量
int get_set_localVar(char* name, char* val, int mode, int rwMode);

// 核心逻辑封装，因为代码量太长情况太多
void declare(char* nameAndVal, int mode);
void unset(char* name);

// 一些辅助功能
void getShellId();
void addShellIdToFile(int type);
static inline int is_var_char(char c);
void replaceEnvVar(const char *src, char *dst);
```

## 实现注释功能

需要使用 `#` 实现注释功能，例如 `ls | cat # this is a comment meow`，`ls | cat` 会被正确执行，而后面的注释则会被抛弃。**此需求为最简单**，只需修改 `runcmd` 逻辑即可：

```c
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
```

## 不带 .b 后缀指令

也是很简单的需求：

```c
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
```

至此， sh.c 中的代码行数为900行左右（如果不封装到外面文件，也不建议封装，因为正确性优先）。

## 指令自由输入

需求：现有的 shell 不支持在输入命令时移动光标。你需要实现：键入命令时，可以使用 Left 和 Right 移动光标位置，并可以在当前光标位置进行字符的增加与删除。要求每次在不同位置键入后，可以完整回显修改后的命令，并且键入回车后可以正常运行修改后的命令。

具体做法是修改 readline 函数。一开始做的时候遇到一个bug是光标每次左右移动时跳两个字符，这是因为在光标移动之后**额外打印了前缀 `"$ "` 两次**，使得位置错位。每次移动光标时，**以为光标在 `$` 后的位置是 `pos`**，但实际上**已经占用了两个字符 (`$` 和空格)**。所以光标移动应该考虑前缀偏移。正确做法是把 `$ ` 这个提示符长度设为常量，在所有光标移动和 `\r` 重绘时都 **加上这个偏移**。

具体操作详解下列快捷键：

## 快捷键

在已有的 `readline()` 函数基础上，**添加更多 Bash 风格快捷键功能**，包括：

| 快捷键    | 动作描述                           |
| --------- | ---------------------------------- |
| ← →       | 光标左右移动 ✅                     |
| Backspace | 删除光标左边字符 ✅                 |
| Ctrl-A    | 移动到开头                         |
| Ctrl-E    | 移动到结尾                         |
| Ctrl-K    | 删除光标到行尾                     |
| Ctrl-U    | 删除行首到光标                     |
| Ctrl-W    | 删除光标前的一个单词（含空格处理） |

查阅资料可知，**上下左右**键在linux中的编码为：

| 键   | 编码       |
| ---- | ---------- |
| 上   | 27 ‘[‘ ‘A’ |
| 下   | 27 ‘[‘ ‘B’ |
| 左   | 27 ‘[‘ ‘D’ |
| 右   | 27 ‘[‘ ‘C’ |

也就是说，当我们在linux系统下，在键盘下按下一个方向键，就拿’↑’举例子吧，就会往缓冲区顺序写入三个字符，分别是`ascii[27]`，’[‘以及’A’。

基于上述理解，我们对shell的命令编辑体验进行优化。

```c
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

		else if (ch == 27) { // Escape sequence
			char seq[2];
			if (read(0, &seq[0], 1) != 1) continue;
			if (read(0, &seq[1], 1) != 1) continue;

			if (seq[0] == '[') {
				if (seq[1] == 'D') { // ←
					if (pos > 0) pos--;
				} else if (seq[1] == 'C') { // →
					if (pos < len) pos++;
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
```

## 历史指令

实现细节非常多，个人认为综合下来难度仅次于环境变量。该功能个人代码量500行左右。整体而言有以下2大类功能：

- 实现内建指令 history
- 实现上下快捷键切换历史指令

其中第一个比较简单。着重记录第二个的实现。其中最为精华的两句话是 `printf("\n$ ");` 和 `printf("\r$ ");`。我这里直接贴出来了，但是仅仅这个点，自己思考推理完全理解并写出至少需要花费1小时以上。GPT对此无法给出正确答案：

```c
else if (ch == 27) {
	char seq[2];
	if (read(0, &seq[0], 1) != 1) continue;
	if (read(0, &seq[1], 1) != 1) continue;

	if (seq[0] == '[') {
		if (seq[1] == 'A') { // ↑
			printf("\n$ ");		// 非常重要！！思考：为什么？？？
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
			printf("\r$ ");		// 非常重要！！思考：为什么？？？
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

// 具体实现功能模块
```

## 一行多指令

做法是对于 `parsecmd` 函数新增一个参数，然后fork时进行维护：

```c
int parsecmd(char **argv, int *rightpipe, int *leftenv) {
    // ...
    case ';': 
		argv[argc] = NULL; // 当前指令切断，避免右边指令进入本次执行

		*leftenv = fork();
		if (*leftenv < 0) {
			debugf("fork: error\n");
			exit();
		} else if (*leftenv == 0) {
			return argc; // 子进程执行当前指令
		} else {
			// 父进程继续解析下一个命令（右边的）
			return parsecmd(argv + argc + 1, rightpipe, leftenv);
		}
		break;
	}
}

// ...
```

## 追加重定向

这里实现代码量不大，但是细节非常多。具体实现如下：

```c
// parsecmd
case '>':
	// 追加重定向 >>
	char *next;
	int peek = gettoken(0, &next);  // 预取下一个 token

	if (peek == '>') {
		if (gettoken(0, &t) != 'w') {
			debugf("syntax error: >> not followed by word\n");
			exit();
		}
		char abspath[MAXPATHLEN];
		char path[MAXPATHLEN];
		relpath2abspath(abspath, t);
		format_abspath(path, abspath);

		fd = open(path, O_WRONLY | O_CREAT | O_APPEND);	// 新增权限位。注意：为什么不能几个权限并起来？？？
		if (fd < 0) {
			debugf("failed to open '%s'\n", t);
			exit();
		}
		dup(fd, 1);
		close(fd);
		break;
	}

	// 普通 >

// user/lib/fd.c 中 write 函数补充
if (fd->fd_omode & O_APPEND) {
	struct Filefd *ffd = (struct Filefd *)fd;  // 注意强转为 Filefd 而非 File
	fd->fd_offset = ffd->f_file.f_size;        // 用结构中真正最新的 f_size
}

// 思考，下列写法为什么错误？？？
if (fd->fd_omode & O_APPEND) {
	struct File *f = (struct File *)fd2data(fd);
	fd->fd_offset = f->f_size;
}
```

## 实现反引号

做法是：当命令中出现反引号对（\`...\`）时，提取其中子命令，执行其结果，并将标准输出捕获后替换到原指令中继续执行。

新增处理函数：

```c
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
				exit();
			}
			i++; // skip closing backquote

			// 执行 temp 命令，捕获 stdout 输出
			int p[2];
			pipe(p);
			int pid = fork();
			if (pid < 0) {
				debugf("fork error\n");
				exit();
			} else if (pid == 0) {
				close(p[0]);
				dup(p[1], 1); // stdout 重定向到写端
				close(p[1]);
				runcmd(temp);
				exit(); // 子进程退出
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
```

在 `main()` 中合理位置调用该函数。

## 指令条件执行

具体实现方式很多样，一种可行的方案是：

记录了一个全局 `excu` 来判定是否需要执行接下来的一条指令，然后再根据新的结果确定新的`excu`的值。

```c
// _gettoken
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

// parsecmd
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
```

然后按照题目中注意事项说的，修改 `exit` 实现：

```c
#include <env.h>
#include <lib.h>
#include <mmu.h>

void exit(int ret) {
    exit_r(ret);
}

void exit_r(int ret) {
    close_all();
    syscall_ipc_try_send(env->env_parent_id, ret, 0, 0);
    syscall_env_destroy(0);
    user_panic("unreachable");
}

const volatile struct Env *env;
extern int main(int, char **);

void libmain(int argc, char **argv) {
	// set env to point at our env structure in envs[].
	env = &envs[ENVX(syscall_getenvid())];

	// call user main routine
	int ret = main(argc, argv);

	// exit gracefully
	exit_r(ret);
}
```

剩下还有一些细节微调，就比较简单了。
