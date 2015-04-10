Index: ./libarchive/archive_read_support_format_zip.c
@@ -245,7 +245,7 @@
 archive_read_format_zip_seekable_bid(struct archive_read *a, int best_bid)
 {
 	struct zip *zip = (struct zip *)a->format->data;
-	int64_t filesize;
+	int64_t end_of_central_directory_offset;
 	const char *p;
 
 	/* If someone has already bid more than 32, then avoid
@@ -253,46 +253,46 @@
 	if (best_bid > 32)
 		return (-1);
 
-	filesize = __archive_read_seek(a, -22, SEEK_END);
+	/* end-of-central-directory record is 22 bytes; first check
+	 * for it at very end of file. */
+	end_of_central_directory_offset = __archive_read_seek(a, -22, SEEK_END);
 	/* If we can't seek, then we can't bid. */
-	if (filesize <= 0)
+	if (end_of_central_directory_offset <= 0)
 		return 0;
 
-	/* TODO: More robust search for end of central directory record. */
 	if ((p = __archive_read_ahead(a, 22, NULL)) == NULL)
 		return 0;
 	/* First four bytes are signature for end of central directory
-	   record.  Four zero bytes ensure this isn't a multi-volume
-	   Zip file (which we don't yet support). */
+	 * record.  Four zero bytes ensure this isn't a multi-volume
+	 * Zip file (which we don't yet support). */
 	if (memcmp(p, "PK\005\006\000\000\000\000", 8) != 0) {
 		int64_t i, tail;
 		int found;
 
 		/*
-		 * If there is a comment in end of central directory
-		 * record, 22 bytes are too short. we have to read more
-		 * to properly detect the record. Hopefully, a length
-		 * of the comment is not longer than 16362 bytes(16K-22).
+		 * End-of-central-directory isn't exactly at end of file.
+		 * Try reading last 16k of file and search for it.
 		 */
-		if (filesize + 22 > 1024 * 16) {
+		if (end_of_central_directory_offset + 22 > 1024 * 16) {
 			tail = 1024 * 16;
-			filesize = __archive_read_seek(a, tail * -1, SEEK_END);
+			end_of_central_directory_offset
+			    = __archive_read_seek(a, tail * -1, SEEK_END);
 		} else {
-			tail = filesize + 22;
-			filesize = __archive_read_seek(a, 0, SEEK_SET);
+			tail = end_of_central_directory_offset + 22;
+			end_of_central_directory_offset
+			    = __archive_read_seek(a, 0, SEEK_SET);
 		}
-		if (filesize < 0)
+		if (end_of_central_directory_offset < 0)
 			return 0;
 		if ((p = __archive_read_ahead(a, (size_t)tail, NULL)) == NULL)
 			return 0;
-		for (found = 0, i = 0;!found && i < tail - 22;) {
+		for (found = 0, i = 0; !found && i < tail - 22;) {
 			switch (p[i]) {
 			case 'P':
 				if (memcmp(p+i,
 				    "PK\005\006\000\000\000\000", 8) == 0) {
 					p += i;
-					filesize += tail -
-					    (22 + archive_le16dec(p+20));
+					end_of_central_directory_offset += i;
 					found = 1;
 				} else
 					i += 8;
@@ -313,14 +313,14 @@
 	zip->central_directory_entries = archive_le16dec(p + 10);
 	zip->central_directory_size = archive_le32dec(p + 12);
 	zip->central_directory_offset = archive_le32dec(p + 16);
-	zip->end_of_central_directory_offset = filesize;
+	zip->end_of_central_directory_offset = end_of_central_directory_offset;
 
 	/* Just one volume, so central dir must all be on this volume. */
 	if (zip->central_directory_entries != archive_le16dec(p + 8))
 		return 0;
-	/* Central directory can't extend beyond end of this file. */
+	/* Central directory can't extend beyond start of EOCD record. */
 	if (zip->central_directory_offset +
-	    (int64_t)zip->central_directory_size > filesize)
+	    (int64_t)zip->central_directory_size > end_of_central_directory_offset)
 		return 0;
 
 	/* This is just a tiny bit higher than the maximum returned by
@@ -1619,10 +1619,12 @@
 		switch (headerid) {
 		case 0x0001:
 			/* Zip64 extended information extra field. */
-			if (datasize >= 8)
+			if (datasize >= 8 &&
+			    zip_entry->uncompressed_size == 0xffffffff)
 				zip_entry->uncompressed_size =
 				    archive_le64dec(p + offset);
-			if (datasize >= 16)
+			if (datasize >= 16 &&
+			    zip_entry->compressed_size == 0xffffffff)
 				zip_entry->compressed_size =
 				    archive_le64dec(p + offset + 8);
 			break;
