// vim:set ts=4 sw=4 ai et smarttab:

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

char *string = "undef";
int integer = 22;
int flag = 0;

int main(int argc, char *argv[])
{
    int option_index = 0;
    int c;

    struct option long_options[] = {
        { "string",         required_argument,  NULL, 's' },
        { "integer",        required_argument,  NULL, 'i' },
        { "flag",           no_argument,        NULL, 'f' },
        { 0, 0, 0, 0}
    };

    char *optstring = "s:i:f";

    while (1) {
        c = getopt_long(argc, argv, optstring, long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 's': string   = optarg;        break;
            case 'i': integer  = atoi(optarg);  break;
            case 'f': flag     = 1;             break;
            default:
                printf("Usage: %s [...]\n", argv[0]);
                exit(1);
        }
    }

    printf("String:  %s\n", string);
    printf("Integer: %d\n", integer);
    printf("Flag:    %s\n", flag? "yes" : "no");

    if (optind < argc) {
        printf("Non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    return 0;
}
