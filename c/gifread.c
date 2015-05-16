/*
 * mct note:  On January 30, 2007, I downloaded this file from:
 *
 * 	<http://www.let.rug.nl/~kleiweg/gif/gifread.c>
 *
 * The downloaded file had an mtime of Jun 18, 1997.  There is a copyright
 * owner identified in the file, but no licecning information.  The only
 * change I made was to add the single line "#include <errno.h>" to allow
 * it to compile.
 */

/*
 *  file: gifread.c
 */

#ifdef __MSDOS__
#ifndef __COMPACT__
#error Memory model COMPACT required
#endif  /* __COMPACT__  */
#include <dir.h>
#endif  /* __MSDOS__  */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum { FALSE = 0, TRUE = 1 } BOOL;
typedef unsigned char byte;

char
    yes [] = "Yes",
    no [] = "No",
    buffer [3 * 256 + 1],
    *programname;

unsigned char * const ubuf = (unsigned char *) buffer;

unsigned
    image_count = 0,
    logical_screen_width,
    logical_screen_height,
    color_resolution,
    global_color_table_size;

byte
    background_color_index,
    pixel_aspect_ratio;

BOOL
    global_color_table_flag,
    sort_flag;

FILE
    *fp_in;

void
    read_image_descriptor (void),
    read_extension (void),
    print_offset (void),
    printable (int count),
    getbytes (int count),
    get_programname (char const *argv0),
    wexit (char const *format, ...),
    syntax (void);

unsigned
    getunsigned (int offset);

int main (int argc, char *argv [])
{
    unsigned
        u;
    long unsigned
        thispos,
        lastpos;
    BOOL
        bezig;

    get_programname (argv [0]);

    if (argc != 2)
	syntax ();

    fp_in = fopen (argv [1], "rb");
    if (! fp_in)
	wexit ("Opening file \"%s\": %s\n\n", argv [1], strerror (errno));
    printf ("\"%s\"\n", argv [1]);

    /* HEADER */
    print_offset ();
    getbytes (6);
    if (memcmp (buffer, "GIF", 3))
	wexit ("Not a GIF");
    printable (6);
    printf ("%s\n", buffer);

    /* LOGFICAL SCREEN DESCRIPTOR */
    print_offset ();
    getbytes (7);
    logical_screen_width = getunsigned (0);
    logical_screen_height = getunsigned (2);
    global_color_table_flag = (ubuf [4] & 0x80) ? TRUE : FALSE;
    color_resolution = ((ubuf [4] & 0x70) >> 4) + 1;
    sort_flag = (ubuf [4] & 0x08) ? TRUE : FALSE;
    global_color_table_size = 2 << (ubuf [4] & 0x07);
    background_color_index = ubuf [5];
    pixel_aspect_ratio = ubuf [6];

    printf ("Logical Screen Width: %u\n", logical_screen_width);
    printf ("Logical Screen Height: %u\n", logical_screen_height);
    printf ("Color Resolution: %u\n", color_resolution);
    printf (
	"Pixel Aspect Ratio: %u %s\n",
	pixel_aspect_ratio,
	pixel_aspect_ratio ? "" : " (not used)"
    );
    printf ("Global Color Table: %s\n", global_color_table_flag ? yes : no);
    if (global_color_table_flag) {
	printf ("  Size: %u\n", global_color_table_size);
	printf ("  Sorted: %s\n", sort_flag ? yes : no);
	printf ("  Background Color Index: %u\n", (unsigned) background_color_index);
	/* GLOBAL COLOR TABLE */
	print_offset ();
	getbytes (3 * global_color_table_size);
	printf ("  Entries (Red Green Blue):\n");
	for (u = 0; u < global_color_table_size; u++)
	    printf (
		"    %3u: %3u %3u %3u\n",
		u,
                (unsigned) ubuf [3 * u],
                (unsigned) ubuf [3 * u + 1],
                (unsigned) ubuf [3 * u + 2]
            );
    }

    /* DATA, TAILER */
    for (bezig = TRUE; bezig; ) {
	print_offset ();
	getbytes (1);
        switch (ubuf [0]) {
	    case 0x21:
                read_extension ();
                break;
            case 0x2C:
                read_image_descriptor ();
		break;
            case 0x3B:
                printf ("Trailer\n");
                bezig = FALSE;
                break;
            default:
                wexit ("Unrecognized byte code %u\n", (unsigned) ubuf [0]);
	}
    }

    thispos = ftell (fp_in);
    if (thispos == -1L)
        wexit ("Reading file \"%s\": %s", argv [1], strerror (errno));
    fseek (fp_in, 0, SEEK_END);
    lastpos = ftell (fp_in);
    if (lastpos == -1L)
        wexit ("Reading file \"%s\": %s", argv [1], strerror (errno));
    if (thispos < lastpos)
        wexit ("%lu bytes unread", (long unsigned) lastpos - thispos);

    fclose (fp_in);

    return 0;
}

void print_offset ()
{
    printf ("<< %04lX >>\n", ftell (fp_in));
}

void read_image_descriptor ()
{
    unsigned
	u,
	size;
    BOOL
	local_color_table_flag;

    printf ("Image nr. %u\n  Image Descriptor\n", ++image_count);
    getbytes (9);
    printf ("    Left Position: %u\n", getunsigned (0));
    printf ("    Top Position: %u\n", getunsigned (2));
    printf ("    Width: %u\n", getunsigned (4));
    printf ("    Height: %u\n", getunsigned (6));
    printf ("    Interlaced: %s\n", (ubuf [8] & 0x40) ? yes : no);
    local_color_table_flag = (ubuf [8] & 0x80) ? TRUE : FALSE;
    printf ("    Local Color Table: %s\n", local_color_table_flag ? yes : no);
    if (local_color_table_flag) {
	size = 2 << (ubuf [8] & 0x07);
	printf ("      Size: %u\n", size);
	printf ("      Sorted: %s\n", (ubuf [8] & 0x20) ? yes : no);
	printf ("      Entries (Red Green Blue):\n");
	print_offset ();
	getbytes (3 * size);
	for (u = 0; u < size; u++)
	    printf (
		"        %3u: %3u %3u %3u\n",
		u,
		(unsigned) ubuf [3 * u],
		(unsigned) ubuf [3 * u + 1],
		(unsigned) ubuf [3 * u + 2]
	    );
    }
    print_offset ();
    getbytes (1);
    printf ("  LZW Minimum Code Size: %u\n", (unsigned) ubuf [0]);
    for (;;) {
	getbytes (1);
	if ((u = ubuf [0]) == 0)
	    break;
	printf ("  Image Data Block: %u bytes\n", u);
	getbytes (u);
    }
}

void read_extension ()
{
    BOOL
        transparant;
    unsigned
        u;

    getbytes (1);
    switch (ubuf [0]) {
	case 0xF9:
	    printf ("Graphic Control Extension\n");
	    getbytes (6);
	    printf ("  Disposal Method: ");
	    switch ((ubuf [1] & 0x1C) >> 2) {
		case 0:
		    printf ("Not specified\n");
		    break;
		case 1:
		    printf ("Do not dispose\n");
		    break;
		case 2:
		    printf ("Restore to background color\n");
		    break;
		case 3:
		    printf ("Restore to previous\n");
		    break;
		default:
		    printf ("4-7: To be defined\n");
	    }
	    printf ("  User Input: %s\n", (ubuf [1] & 0x02) ? yes : no);
	    transparant = (ubuf [1] & 1) ? TRUE : FALSE;
	    printf ("  Transparant Color: %s\n", transparant ? yes : no);
	    if (transparant)
		printf ("    Color Index: %u\n", (unsigned) ubuf [4]);
	    printf ("  Delay Time: %.2f s.\n", ((float)getunsigned (2)) / 100.0);
	    break;
	case 0xFE:
	    printf ("Comment Extension:\n");
	    for (;;) {
		getbytes (1);
		if ((u = ubuf [0]) == 0)
		    break;
		getbytes (u);
		printable (u);
		printf ("  %s\n", buffer);
	    }
	    break;
	case 0x01:
            printf ("Plain Text Extension\n");
	    getbytes (13);
            printf ("  Text Grid Left Position: %u\n", getunsigned (1));
            printf ("  Text Grid Top Position: %u\n", getunsigned (3));
            printf ("  Text Grid Width: %u\n", getunsigned (5));
            printf ("  Text Grid Height: %u\n", getunsigned (7));
            printf ("  Character Cell Width: %u\n", (unsigned) ubuf [9]);
            printf ("  Character Cell Height: %u\n", (unsigned) ubuf [10]);
            printf ("  Text Foreground Color Index: %u\n", (unsigned) ubuf [11]);
            printf ("  Text Background Color Index: %u\n", (unsigned) ubuf [12]);
	    printf ("  Plain Text Data:\n");
	    print_offset ();
	    for (;;) {
		getbytes (0);
		if ((u = ubuf [0]) == 0)
		    break;
		getbytes (u);
		printable (u);
		printf ("    %s\n", buffer);
	    }
            break;
        case 0xFF:
	    getbytes (9);
            printable (9);
            printf ("Application Extension: %s\n", buffer + 1);
            if (! memcmp (buffer + 1, "NETSCAPE", 8)) {
		getbytes (3);
                printable (3);
		printf ("  NETSCAPE Authentication Code: %s\n", buffer);
		for (;;) {
		    getbytes (1);
                    if ((u = buffer [0]) == 0)
                        break;
		    getbytes (ubuf [0]);
                    if (ubuf [0] == 1)
                        printf (
                            "  Loop Extension\n"
                            "    Number of Iterations: %u %s\n",
                            getunsigned (1),
                            getunsigned (1) ? "" : " (infinite)"
                        );
                    else
                        printf ("  Unknown Data block: %u bytes\n", (unsigned) ubuf [0]);
                }
            } else {
		getbytes (3);
                printf (
                    "  Unknown Application, Application Authentication Code: %u %u %u\n",
                    (unsigned) ubuf [0],
                    (unsigned) ubuf [1],
                    (unsigned) ubuf [2]
		);
		print_offset ();
		for (;;) {
		    getbytes (1);
                    if (! buffer [0])
                        break;
		    printf ("  Data block: %u bytes\n", (unsigned) ubuf [0]);
		    getbytes (ubuf [0]);
                }
            }
            break;
	default:
            printf ("Unrecognized extension code %u\n", (unsigned) ubuf [0]);
	    for (;;) {
		getbytes (1);
		if (! ubuf [0])
		    break;
		printf ("  Skipping: %u bytes\n", (unsigned) ubuf [0]);
		getbytes (ubuf [0]);
	    }
    }
}

unsigned getunsigned (int offset)
{
    return (((unsigned)ubuf [offset]) + 256 * (unsigned) ubuf [offset + 1]);
}

void printable (int count)
{
    while (count--)
        if ((ubuf [count] != '\n' && ubuf [count] < 32) || (ubuf [count] > 126 && ubuf [count] < 160))
            ubuf [count] = '?';
}

void wexit (char const *format, ...)
{
    va_list
	list;

    printf ("\nError %s: ", programname);

    va_start (list, format);
    vprintf (format, list);

    printf ("\n\n");

    exit (1);
}

void getbytes (int count)
{
    int
        i;

    i = fread (buffer, 1, count, fp_in);
    buffer [i] = '\0';
    if (i == count)
        return;

    wexit (feof (fp_in) ? "Unexpected end of file" : "Error reading file");
}

void get_programname (char const *argv0)
{
#ifdef __MSDOS__
    char
        name [MAXFILE];
    fnsplit (argv0, NULL, NULL, name, NULL);
    programname = strdup (name);
#else   /* unix */
    char
        *p;
    p = strrchr (argv0, '/');
    if (p)
        programname = strdup (p + 1);
    else
        programname = strdup (argv0);
#endif    
}

void syntax ()
{
    fprintf (
	stderr,
        "\nGIF reader\n"
	"\nUsage: %s file.gif\n"
        "\n(c) P. Kleiweg 1997\n\n",
	programname
    );
    exit (1);
}
