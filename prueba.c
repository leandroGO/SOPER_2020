#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char** argv) {
    int f, x = 17;
    char buf[10];
    
    f = open("numero_leido.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH);
    if (f < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    sprintf(buf, "%d", x);

    if (write(f, &x, strlen(buf)+1) < strlen(buf)) {
        perror("write");
        close(f);
        return EXIT_FAILURE;
    }

    close(f);
    return EXIT_SUCCESS;
}