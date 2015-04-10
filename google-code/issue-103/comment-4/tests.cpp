

#include <QDebug>
#include <QDateTime>

extern "C" {
	#include "archive.h"
	#include "archive_entry.h"
}

void add_entry(archive *a, const QString& namestr, const char * data, int size) {
	archive_entry *entry;

	qDebug() << "Adding entry" << namestr << data << size;

	entry = archive_entry_new();
	archive_entry_set_pathname(entry, namestr.toAscii().constData());
	archive_entry_set_size(entry, size);
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0444);

	//archive_entry_set_birthtime(entry, QDateTime::currentDateTime().toTime_t(), 0);

	if (0 != archive_write_header(a, entry)) {
		qCritical() << "Archive write error" << archive_error_string(a);
		archive_entry_free(entry);
		return;
	}

	if (-1 == archive_write_data(a, data, size)) {
		qFatal("Cannot write to entry: %s", archive_error_string(a));
	}

	//archive_write_finish_entry(a);
	archive_entry_free(entry);

	qDebug() << "Wrote entry to archive" << namestr << size << archive_error_string(a);
}

int main(int argc, char ** argv) {

	if (argc < 2) {
		qDebug() << "First arg is filename.";
		return -1;
	}

	struct archive *a;

	a = archive_write_new();

	Q_ASSERT(0 == archive_write_set_compression_gzip(a));
	Q_ASSERT(0 == archive_write_set_format_ustar(a));

	qDebug() << "Creating archive" << argv[1];

	if (archive_write_open_filename(a, argv[1]) != ARCHIVE_OK) {
		qFatal("Could not open archive %s.", argv[1]);
	}

	for (int i = 0; i < 20; i++) {
		char buffer[] = "this is the dadafsdfasdfasdfasdata";
		add_entry(a, QString("Entry%1.txt").arg(buffer), (const char *)buffer, strlen(buffer));
	}

	archive_write_close(a);

	Q_ASSERT(0 == archive_write_finish(a));
}
