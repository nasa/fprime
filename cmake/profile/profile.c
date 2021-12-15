#include <sys/time.h>
#include <stdio.h>

int main(int argc, char** argv) {
    struct timeval val;
    gettimeofday(&val, 0);

    FILE* pointer = stdout;
    if (argc > 1)
    {
        pointer = fopen(argv[1], "a");
    }

    fprintf(pointer, "%ld.%06d", val.tv_sec, val.tv_usec);
    for (int i = 2; i < argc; i++)
    {
        fprintf(pointer, " %s", argv[i]);
    }
    fprintf(pointer, "\n");
}
