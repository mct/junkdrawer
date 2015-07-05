// vim:set ts=4 sw=4 ai et:

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("%d\n", getdtablesize());
    return 0;
}
