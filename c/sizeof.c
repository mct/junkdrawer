// vim:set ts=4 sw=4 st=4 ai et:
#include <stdio.h>
#include <bits/wordsize.h>
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>

int main(void)
{
    printf("     off_t: %zd\n", sizeof(off_t));
    printf("    size_t: %zd\n", sizeof(size_t));
    printf("     short: %zd\n", sizeof(short));
    printf("       int: %zd\n", sizeof(int));
    printf("      long: %zd\n", sizeof(long));
    printf("     float: %zd\n", sizeof(float));
    printf(" long long: %zd\n", sizeof(long long));
    printf("   pointer: %zd\n", sizeof(void*));
    printf("    double: %zd\n", sizeof(double));
    printf("\n");
    printf("  CHAR_BIT: %u bits per char\n", CHAR_BIT);
    printf("  SHRT_MIN: %u\n",  SHRT_MIN);
    printf("  SHRT_MAX: %u\n",  SHRT_MAX);
    printf(" USHRT_MAX: %u\n", USHRT_MAX);
    printf("   INT_MIN: %u\n", INT_MIN);
    printf("   INT_MAX: %u\n", INT_MAX);
    printf("  UINT_MAX: %u\n", UINT_MAX);


/*

   printf("The number of bits in a byte %d\n", CHAR_BIT);

   printf("The minimum value of SIGNED CHAR = %d\n", SCHAR_MIN);
   printf("The maximum value of SIGNED CHAR = %d\n", SCHAR_MAX);
   printf("The maximum value of UNSIGNED CHAR = %d\n", UCHAR_MAX);

   printf("The minimum value of SHORT INT = %d\n", SHRT_MIN);
   printf("The maximum value of SHORT INT = %d\n", SHRT_MAX); 

   printf("The minimum value of INT = %d\n", INT_MIN);
   printf("The maximum value of INT = %d\n", INT_MAX);

   printf("The minimum value of CHAR = %d\n", CHAR_MIN);
   printf("The maximum value of CHAR = %d\n", CHAR_MAX);

   printf("The minimum value of LONG = %ld\n", LONG_MIN);
   printf("The maximum value of LONG = %ld\n", LONG_MAX);
  
*/

    return 0;
}
