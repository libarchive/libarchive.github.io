/* gcc -g -O2 -Wall -larchive -o test test.c */
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <archive.h>

int main(int argc, char *argv[])
{
	struct archive *archive;
	int fd;

	fprintf(stderr, "new\n");
	archive = archive_read_new();

	archive_read_support_compression_all(archive);
	archive_read_support_format_all(archive);

	if (argc > 1 && strcmp(argv[1], "null") == 0) {
		archive = NULL;
		archive_read_finish(archive);
		return 0;
	}

	if (argc > 1 && strcmp(argv[1], "open") == 0) {
		printf("%d\n", argc);
		fd = open("/etc/passwd", O_RDONLY);	
		archive_read_open_fd(archive, fd, 4096);
	}

	fprintf(stderr, "finish 1\n");
	archive_read_finish(archive);
	fprintf(stderr, "finish 2\n");
	archive_read_finish(archive);
	fprintf(stderr, "done\n");
	return 0;
}
