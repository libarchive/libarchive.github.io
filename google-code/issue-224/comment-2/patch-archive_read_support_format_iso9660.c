--- libarchive/archive_read_support_format_iso9660.c.orig	2011-12-28 21:49:14.000000000 -0500
+++ libarchive/archive_read_support_format_iso9660.c	2012-01-17 19:00:29.000000000 -0500
@@ -338,6 +338,7 @@ struct iso9660 {
 
 	int64_t		previous_number;
 	struct archive_string previous_pathname;
+	struct archive_string previous_symlink;
 
 	struct file_info		*use_files;
 	struct heap_queue		 pending_files;
@@ -1261,7 +1262,9 @@ archive_read_format_iso9660_read_header(
 	 * the same data, we have to return it as a hardlink to the
 	 * original entry. */
 	if (file->number != -1 &&
-	    file->number == iso9660->previous_number) {
+	    file->number == iso9660->previous_number &&
+	    file->symlink.s == NULL &&
+	    archive_strlen(&iso9660->previous_symlink) == 0) {
 		if (iso9660->seenJoliet) {
 			r = archive_entry_copy_hardlink_l(entry,
 			    (const char *)iso9660->utf16be_previous_path,
@@ -1344,6 +1347,7 @@ archive_read_format_iso9660_read_header(
 	} else
 		archive_strcpy(
 		    &iso9660->previous_pathname, iso9660->pathname.s);
+	archive_strcpy(&iso9660->previous_symlink, file->symlink.s);
 
 	/* Reset entry_bytes_remaining if the file is multi extent. */
 	iso9660->entry_content = file->contents.first;
@@ -1692,6 +1696,7 @@ archive_read_format_iso9660_cleanup(stru
 	free(iso9660->read_ce_req.reqs);
 	archive_string_free(&iso9660->pathname);
 	archive_string_free(&iso9660->previous_pathname);
+	archive_string_free(&iso9660->previous_symlink);
 	if (iso9660->pending_files.files)
 		free(iso9660->pending_files.files);
 #ifdef HAVE_ZLIB_H
