#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s programa [argumentos]\n", argv[0]);
        return 1;
    }

    pid_t child = fork();

    if (child == -1) {
        perror("fork");
        return 1;
    }

    if (child == 0) {

        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        execvp(argv[1], &argv[1]);

        perror("execvp");
        return 1;

    } else {

        int status;
        int entrada = 1;
        struct user_regs_struct regs;

        /* Espera la parada inicial producida por execvp */
        waitpid(child, &status, 0);

        /* Permite distinguir las paradas producidas por syscalls */
        ptrace(PTRACE_SETOPTIONS,
               child,
               NULL,
               PTRACE_O_TRACESYSGOOD);

        /* Continúa hasta la siguiente syscall */
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);

        while (1) {

            waitpid(child, &status, 0);

            if (WIFEXITED(status)) {
                break;
            }

            if (WIFSTOPPED(status) &&
                WSTOPSIG(status) == (SIGTRAP | 0x80)) {

                ptrace(PTRACE_GETREGS,
                       child,
                       NULL,
                       &regs);

                if (entrada) {
                    printf("PID %d - ENTRADA syscall %lld\n",
                           child,
                           regs.orig_rax);
                } else {
                    printf("PID %d - SALIDA  syscall %lld\n",
                           child,
                           regs.orig_rax);
                }

                entrada = !entrada;
            }

            ptrace(PTRACE_SYSCALL,
                   child,
                   NULL,
                   NULL);
        }
    }

    return 0;
}
