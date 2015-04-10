#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>

int calls=0;

struct archive_entry *ae;
struct archive *a;

int main (int argc, char *argv[])
{
    int r;
    int fp; 
    size_t size;
    char first_buff[32768];

    memset(first_buff,0,sizeof(first_buff));

    a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_raw(a);

    r = archive_read_open_filename(a, argv[1], 16384);
    if (r != ARCHIVE_OK)
    {
        printf ("It's broken\n");
        exit(1);
    }

    r = archive_read_next_header(a, &ae);
    if (r != ARCHIVE_OK) {
       /* ERROR */
    }

    fp = fopen( argv[2], "wb" );
    for (;;) {
       size = archive_read_data(a, first_buff, 16384);
       if (size < 0)
       {
         printf ( "error archive_read_data\n" );
         return 0;
       }

       if (size == 0) break;

       if ( fp ) fwrite(first_buff, 1, size, fp);
    }

    fclose( fp );
}
