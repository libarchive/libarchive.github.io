#include <iconv.h>

int
main(int argc, char **argv)
{
	char *ccp;
	iconv_t cd = iconv_open("", "");
	iconv(cd, &ccp, 0, 0, 0);
	iconv_close(cd);
	return (0);
}
