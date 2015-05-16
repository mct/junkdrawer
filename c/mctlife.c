// vim:set ts=4 sw=4 nocindent ai:
// mct, Mon Nov 24 10:10:57 EST 2003  ...on the train from PHL to NYP
// mct, Mon Nov 24 15:51:38 EST 2003  ...sitting on a bench in central park
// mct, Mon Nov 24 17:23:17 EST 2003  ...on the train from NYP to PHL

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

unsigned char *world;
int worldh, worldw, gennum;

#define LINEZ   80

int HW(int h, int w)
{
    if (h < 0) h = worldh-1;
    if (w < 0) w = worldw-1;
    if (h >= worldh) h = 0;
    if (w >= worldw) w = 0;

    return h*worldw+w;
}

void clear(void)
{
    // lame, lame.  clear(1)'s output under my termcap
    printf("\e[H\e[J");
}

void init(void)
{
    struct timeval tv;
    int h, w;

    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        exit(-1);
    }

    srand(tv.tv_usec);

    world = malloc(worldh * worldw);
    if (world == NULL) {
        perror("malloc");
        exit(-1);
    }

    for (h = 0; h < worldh; h++) {
        for (w = 0; w < worldw; w++) {
            world[HW(h,w)] = 0;
        }
    }

    gennum = 0;
}

void draw(void)
{
    int h, w;

    clear();

    for (h = 0; h < worldh; h++) {
        for (w = 0; w < worldw; w++) {
            printf("%c", world[HW(h,w)] ? 'o': ' ');
        }
        printf("\n");
    }

    printf("Generation %d  ", gennum);
    fflush(stdout);
}

void generation(void)
{
    unsigned char *newworld, *oldworld;
    int h, w;

    newworld = malloc(worldh * worldw);
    if (newworld == NULL) {
        perror("malloc");
        exit(-1);
    }

    for (h = 0; h < worldh; h++) {
        for (w = 0; w < worldw; w++) {
            int count = 0;

            // north row
            if (world[HW(h-1,w-1)]) count++;
            if (world[HW(h-1,w  )]) count++;
            if (world[HW(h-1,w+1)]) count++;

            // east and west
            if (world[HW(h  ,w-1)]) count++;
            if (world[HW(h  ,w+1)]) count++;

            // south row
            if (world[HW(h+1,w-1)]) count++;
            if (world[HW(h+1,w  )]) count++;
            if (world[HW(h+1,w+1)]) count++;

            if (world[HW(h,w)]) {
                if (count == 2 || count == 3)
                    newworld[HW(h,w)] = 1;  // alive and 2 or 3 neighbors
                else
                    newworld[HW(h,w)] = 0;  // alive
            } else {
                if (count == 3)
                    newworld[HW(h,w)] = 1;  // dead with three neighbors
                else
                    newworld[HW(h,w)] = 0;
            }
        }
    }

    oldworld = world;
    world = newworld;
    free(oldworld);
    gennum++;
}

void loadfile(char *file)
{
    FILE *f;
    char buf[LINEZ];
    int readformat, linenum, h, w, offsetw, offseth;

    readformat = 0;
    linenum = 0;
    h = 0;

    f = fopen(file, "r");
    if (f == NULL) {
        perror("open");
        exit(-1);
    }

    while(fgets(buf, LINEZ, f) != NULL) {
        int i, foundnewline;

        foundnewline = 0;
        linenum++;

        for(i = 0; buf[i]; i++) {
            if (buf[i] == '\n') {
                foundnewline = 1;
                buf[i] = '\0';
                break;
            }
        }

        // XXX fixed buffers suck  *sigh*
        if (!foundnewline) {
            fprintf(stderr, "error: line %d: line too long\n", linenum);
            exit(-1);
        }

        // empty line
        if (!readformat && buf[0] == '\0')
            continue;

        // comment
        if (buf[0] == '#' && buf[1] == '#')
            continue;

        // only support the '#P' file format
        if (buf[0] == '#' && buf[1] != 'P') {
            fprintf(stderr, "error: line %d: unsupported format line\n",
                linenum);
            exit(-1);
        }

        // magic line
        if (buf[0] == '#' && buf[1] == 'P') {
            readformat = 1;
            continue;
        }

        // only accept data after we know the file format
        if (!readformat) {
            fprintf(stderr, "error: line %d: data line before format line\n",
                linenum);
            exit(-1);
        }

        if (h == 0) {
            // try to center the file horizontally
            offsetw = (worldw - strlen(buf)) / 2;
            // add 20% vertical whitespace
            // XXX we should be shifting lines to center the image,
            //     once we know how large it is.
            offseth = worldh * 0.20;
        }

        // dataline
        for(w = 0; buf[w]; w++) {
            world[HW(h+offseth, w+offsetw)] =
					(buf[w] == '*' || buf[w] == 'o' || buf[w] == 'O') 
						? 1 : 0;
        }

        h++;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s $(stty size) <filename>\n", argv[0]);
        exit(-1);
    }

    worldh = atoi(argv[1]);
    worldw = atoi(argv[2]);

    if (worldh < 5) {
        fprintf(stderr, "error: less than 5 rows?\n");
        exit(-1);
    }

    if (worldw < 5) {
        fprintf(stderr, "error: less than 5 columns\n");
        exit(-1);
    }

    init();
    loadfile(argv[3]);

    while(1) {
        draw();
        //usleep(50000);
        usleep(500000);
        //sleep(1);
        generation();
    }

    exit(0);
}
