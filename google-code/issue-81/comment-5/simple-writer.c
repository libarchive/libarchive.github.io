#include <archive.h>
#include <archive_entry.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int
main(void)
{
	char buf[10];
	int i;
	struct archive *archive;
	struct archive_entry *entry;
	struct stat sb;

	archive = archive_write_new();
	archive_write_set_format_iso9660(archive);
	archive_write_open_filename(archive, "output.iso");
	entry = archive_entry_new();
	stat("/etc/fstab", &sb);
	sb.st_size = 0;
	archive_entry_copy_stat(entry, &sb);
	for (i = 0; i < 100000; ++i) {
		snprintf(buf, sizeof(buf), "%d", i);
		archive_entry_set_size(entry, strlen(buf));
		archive_entry_set_pathname(entry, buf);
		if (archive_write_header(archive, entry) != ARCHIVE_OK) {
			fputs("archive_write_header\n", stderr);
			return 1;
		}
		archive_write_data(archive, buf, strlen(buf));
		if (i % 1000 == 0)
			fputc('.', stderr);
	}
	if (archive_write_finish(archive) != ARCHIVE_OK) {
		fputs("archive_write_finish\n", stderr);
		return 1;
	}
	return 0;
}
