#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <seccomp.h>

int main(void)
{
    scmp_filter_ctx ctx;
    int rc;

    /* Por defecto, permitir todas las syscalls */
    ctx = seccomp_init(SCMP_ACT_ALLOW);

    if (ctx == NULL) {
        fprintf(stderr, "Error al crear el filtro seccomp\n");
        return 1;
    }

    /*
     * Bloquear open() y openat().
     * En sistemas Linux modernos, open() normalmente termina
     * utilizando openat() internamente.
     */
    rc = seccomp_rule_add(
        ctx,
        SCMP_ACT_ERRNO(EPERM),
        SCMP_SYS(open),
        0
    );

    if (rc < 0) {
        fprintf(stderr, "Error agregando regla para open\n");
        seccomp_release(ctx);
        return 1;
    }

    rc = seccomp_rule_add(
        ctx,
        SCMP_ACT_ERRNO(EPERM),
        SCMP_SYS(openat),
        0
    );

    if (rc < 0) {
        fprintf(stderr, "Error agregando regla para openat\n");
        seccomp_release(ctx);
        return 1;
    }

    /* Activar el filtro */
    rc = seccomp_load(ctx);

    if (rc < 0) {
        fprintf(stderr, "Error al cargar el filtro seccomp\n");
        seccomp_release(ctx);
        return 1;
    }

    printf("Filtro seccomp instalado.\n");
    printf("Intentando abrir prueba_seccomp.txt...\n");

    int fd = open("prueba_seccomp.txt", O_RDONLY);

    if (fd == -1) {
        printf("La apertura fallo.\n");
        printf("errno = %d\n", errno);
        printf("Error: %s\n", strerror(errno));
    } else {
        printf("El archivo se abrio correctamente.\n");
        close(fd);
    }

    seccomp_release(ctx);

    return 0;
}
