// vim:set ts=4 sw=4 ai et smarttab:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// djb's hash from http://www.cse.yorku.ca/~oz/hash.html

uint32_t hash32(char *str)
{
    uint32_t hash = 5381;
    char c;

    while ((c = *str++))
        hash = hash * 33 + c;
    return hash;
}

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) {
        printf("%u\n",  hash32(argv[i]));
        //printf("%ld\n", hash64(argv[i]));
        //printf("%08lx\n", taketwo(argv[i]));
    }
    return 0;
}
