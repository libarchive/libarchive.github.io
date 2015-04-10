#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <archive.h>
#include <archive_entry.h>

int main(int argc, char **argv) {
	printf("stat struct size = %u\n", sizeof(struct stat));
	
	int r;
	struct stat finfo;
	struct archive_entry *ent;
	struct archive *tar = NULL;
    
	size_t maxArch = 128 * 1024; // alloc 128k by default
    
	void *arch = malloc(maxArch);
	size_t used = 0;
 
	tar = archive_write_new();
	if (tar == NULL) {
		printf("Failed to allocate archive\n");
		exit(-1);
	}
	
	r = archive_write_set_compression_gzip(tar);
	if (r != ARCHIVE_OK) {
		printf("Failed to set compression\n");
		exit(-1);
	}
	    
	r = archive_write_set_format_ustar(tar);
	if (r != ARCHIVE_OK) {
		printf("Failed to set format\n");
		exit(-1);
	}
	
	r = archive_write_open_memory(tar, arch, maxArch, &used);
	if (r != ARCHIVE_OK) {
		printf("Failed to open\n");
		exit(-1);
	}
	
	(void) lstat("/bin/ls", &finfo);

	ent = archive_entry_new();
	if (ent == NULL) {
		printf("Failed to allocate entry\n");
		exit(-1);
	}
    
	archive_entry_copy_stat(ent, &finfo);
	archive_entry_copy_pathname(ent, "ls");

	r = archive_write_header(tar, ent);
	printf("archive_write_header() = %d\n", r);
	
	return (int)argv[argc-1]; // just to use them
}
