#include <stdio.h>
#include <bits/wordsize.h>
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>

int main(void)
{
	printf("     off_t: %zd\n", sizeof(off_t));
	printf("    size_t: %zd\n", sizeof(size_t));
	printf("       int: %zd\n", sizeof(int));
	printf("      long: %zd\n", sizeof(long));
	printf("     float: %zd\n", sizeof(float));
	printf(" long long: %zd\n", sizeof(long long));
	printf("     short: %zd\n", sizeof(short));
	printf("   pointer: %zd\n", sizeof(void*));
	printf("    double: %zd\n", sizeof(double));
	printf("  CHAR_BIT: %d bits per char\n", CHAR_BIT);

	return 0;
}
