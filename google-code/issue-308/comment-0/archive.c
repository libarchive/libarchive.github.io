
#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (argc != 2)
    abort();

  fprintf(stderr, "libarchive_version: %d\n", ARCHIVE_VERSION_NUMBER);

  struct archive *arc = archive_read_new();
  if (! arc)
    abort();

  if (archive_read_support_format_all(arc) != ARCHIVE_OK)
    abort();

#if 1
  if (archive_read_open_filename(arc, argv[1], 8192) != ARCHIVE_OK)
    abort();
#else
  FILE *fh = fopen(argv[1], "rb");
  if (! fh)
    abort();
  if (archive_read_open_FILE(arc, fh) != ARCHIVE_OK)
    abort();
#endif

  struct archive_entry *ent;
  while (archive_read_next_header(arc, &ent) == ARCHIVE_OK) {
    const char *path = archive_entry_pathname(ent);
    fprintf(stderr, "%s\n", path);
  }

  return 0;
}
