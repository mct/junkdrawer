// vim:set ts=4 sw=4 ai et:
// Michael C. Toren <mct@toren.net>
// mct, Tue Apr 14 14:05:33 PDT 2009

#include <stdlib.h>
#include <stdio.h>

/*
 * gcc inline assembly to query the CPU for its vendor string.
 * Returns a pointer to a static NUL-terminated string.
 *
 * Typical CPU strings are "GenuineIntel" on Intel, and "AuthenticAMD" on
 */

char *cpuid()
{
    static char id[13];
    asm volatile("xor %%eax, %%eax      \n\t"
                 "cpuid                 \n\t"
                 "lea %0, %%eax         \n\t"
                 "mov %%ebx, 0(%%eax)   \n\t"
                 "mov %%edx, 4(%%eax)   \n\t"
                 "mov %%ecx, 8(%%eax)   \n\t"
                 "movb $0,  12(%%eax)   \n\t"
                 : "=m" (*id)                   // output
                 :                              // no input
                 : "eax", "ebx", "edx", "ecx"); // clobber list
    return id;
}

int main(int argc, char *argv[])
{
    printf("%s\n", cpuid());
    return 0;
}
