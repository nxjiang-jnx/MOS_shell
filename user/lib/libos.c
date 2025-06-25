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
