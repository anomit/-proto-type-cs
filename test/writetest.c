#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./wt <mountpoint> <query>\n");
        exit(1);
    }

    char location[256];
    memset(location, 0, 256);
    strcpy(location, argv[1]);
    strncat(location, "/cs", sizeof("/cs"));


    int fd = open(location, O_RDWR);
    write(fd, argv[2], strlen(argv[2]));
    close(fd);
    return 0;
}
