/*-
 * Copyright (c) 2003-2015 Tim Kientzle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "test.h"
__FBSDID("$FreeBSD$");

static int
get_format(const char *archive)
{
	struct archive *a;
	struct archive_entry *ae;
	int found_format;
 
	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_mtree(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_raw(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_empty(a));
	assertEqualIntA(a, ARCHIVE_OK,
	    archive_read_open_memory(a, archive, strlen(archive)));
	// Read one header to settle the format.
	// This might return EOF or OK.
	archive_read_next_header(a, &ae);

	found_format = archive_format(a);

	assertEqualInt(ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));

	return found_format;
}

DEFINE_TEST(test_read_format_mtree_bid)
{
	assertEqualInt(ARCHIVE_FORMAT_RAW,
	    get_format("b704c67b39978aff8cc254e78976f001ea07a6d0\n"));

	assertEqualInt(ARCHIVE_FORMAT_MTREE,
	    get_format("#mtree\na type=file\n"));

	assertEqualInt(ARCHIVE_FORMAT_MTREE,
	    get_format("/set type=dir\n.\n"));

	assertEqualInt(ARCHIVE_FORMAT_MTREE, get_format("a type=file\n"));

	/* Mtree implementations might add new file types,
	   we should recognize them as mtree files. */
	assertEqualInt(ARCHIVE_FORMAT_MTREE,
	    get_format("/set type=nonsense\n.\n"));

	/* But the type codes should be alphanumeric! */
	assertEqualInt(ARCHIVE_FORMAT_MTREE,
	    get_format("/set type=.=4\n.\n"));

	/* First line must specify type. */
	assertEqualInt(ARCHIVE_FORMAT_RAW, get_format("a\nb type=file\n"));

	/* Mtree files must have complete lines, this is missing a NL. */
	assertEqualInt(ARCHIVE_FORMAT_RAW, get_format("a type=file"));
}

