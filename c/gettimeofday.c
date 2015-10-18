// vim:set ts=4 sw=4 ai et:

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    struct timeval tv;

    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);

    printf("%010lu.%06lu\n", (uint64_t) tv.tv_sec, (uint64_t) tv.tv_usec);
    return 0;
}
