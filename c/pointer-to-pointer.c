// vim:set ts=4 sw=4 ai et:

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void f(char **user_buf, int **user_len)
{
    static int len;
    static char buf[1024];

    strcpy(buf, "Hello, world");
    len = strlen(buf);

    *user_buf = buf;
    *user_len = &len;
}

int main(int argc, char *argv[])
{
    char *buf;
    int *len;

    printf("%p\n", buf);

    f(&buf, &len);

    printf("%p %d %s\n", buf, *len, buf);
    return 0;
}
