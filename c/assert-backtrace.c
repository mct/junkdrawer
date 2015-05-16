// vim:set ts=4 sw=4 ai et:
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/*

Compile with "-rdynamic" to include function names in the backtrace output

Example output:

$ gcc -rdynamic -Werror -o assert-backtrace assert-backtrace.c && ./assert-backtrace
Died with signal 6.  Backtrace:

   0   assert-backtrace(handler+0x25) [0x400b81]
   1   /lib/x86_64-linux-gnu/libc.so.6(+0x321e0) [0x7f4f16a871e0]
   2   /lib/x86_64-linux-gnu/libc.so.6(gsignal+0x35) [0x7f4f16a87165]
   3   /lib/x86_64-linux-gnu/libc.so.6(abort+0x180) [0x7f4f16a8a3e0]
   4   assert-backtrace(qux+0) [0x400c92]
   5   assert-backtrace(qux+0xe) [0x400ca0]
   6   assert-backtrace(baz+0xe) [0x400cb0]
   7   assert-backtrace(bar+0xe) [0x400cc0]
   8   assert-backtrace(foo+0xe) [0x400cd0]
   9   assert-backtrace(main+0x28) [0x400cfa]
  10   /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xfd) [0x7f4f16a73ead]
  11   assert-backtrace() [0x400a79]

*/

#define STACK_DEPTH   20
void handler(int sig)
{
    void *buffer[STACK_DEPTH];
    char **strings;
    int size, i;

    size = backtrace(buffer, STACK_DEPTH);
    strings = backtrace_symbols(buffer, size);

    fprintf(stderr, "Died with signal %d.  Backtrace:\n\n", sig);

    for (i = 0; i < size; i++) {
        if (strings[i][0] == '.' && strings[i][1] == '/')
            strings[i] += 2;

        printf(" %3d   %s\n", i, strings[i]);
    }
    printf("\n");
    exit(1);
}

void quux() { abort(); }
void qux() { quux(); }
void baz() { qux(); }
void bar() { baz(); }
void foo() { bar(); }

int main(int argc, char **argv)
{
    signal(SIGABRT, handler);   // install our handler
    foo();
    return 0;
}
