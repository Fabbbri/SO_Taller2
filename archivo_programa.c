#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    int fd = open("salida.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char mensaje[] = "Hola fabri, como estas\n";

    write(fd, mensaje, sizeof(mensaje) - 1);

    close(fd);

    return 0;
}
