#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t child = fork();

    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);
    } else {
        int status;
        struct user_regs_struct regs;

        while (1) {
            waitpid(child, &status, 0);

            if (WIFEXITED(status))
                break;

            ptrace(PTRACE_GETREGS, child, NULL, &regs);

            printf("Syscall numero: %lld\n", regs.orig_rax);

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }

    return 0;
}
