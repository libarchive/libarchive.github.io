#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int64_t
tar_atol256(const char *_p, size_t char_cnt)
{
        uint64_t l;
        const unsigned char *p = (const unsigned char *)_p;
	unsigned char c;
	unsigned char neg;

	/* Extend first byte to 8-bit twos-complement, decide sign. */
	c = *p;
        neg = (0x40 & c) == 0x40 ? 0xff : 0;
	if (neg) {
		c |= 0x80;
                l = ~0ULL;
	} else {
		c &= 0x7f;
                l = 0;
	}

	/* If more than 8 bytes, ensure high-order bits are
	 * consistent with sign bit. */
	while (char_cnt > sizeof(int64_t)) {
		--char_cnt;
		if (c != neg)
			return neg ? INT64_MIN : INT64_MAX;
		c = *++p;
	}
	/* c is first byte that fits; if sign mismatch, return overflow */
	if ((c ^ neg) & 0x80) {
		return neg ? INT64_MIN : INT64_MAX;
	}

	/* Accumulate remaining bytes. */
        while (--char_cnt > 0) {
                l = (l << 8) | c;
		c = *++p;
        }
	l = (l << 8) | c;
	/* Return signed twos-complement value. */
        return (int64_t)(l);
}

int
main(int argc, char **argv)
{
	char buff[16];
	int len;

	assert(-1 == tar_atol256("\xff", 1));
	assert(-1 == tar_atol256("\xff\xff", 2));
	assert(-1 == tar_atol256("\xff\xff\xff", 3));
	assert(-1 == tar_atol256("\xff\xff\xff\xff", 4));
	assert(-1 == tar_atol256("\xff\xff\xff\xff\xff", 5));
	assert(-1 == tar_atol256("\xff\xff\xff\xff\xff\xff", 6));
	assert(-1 == tar_atol256("\xff\xff\xff\xff\xff\xff\xff", 7));
	assert(-1 == tar_atol256("\xff\xff\xff\xff\xff\xff\xff\xff", 8));
	assert(-1 == tar_atol256("\xff\xff\xff\xff\xff\xff\xff\xff\xff", 9));

	assert(0 == tar_atol256("\0", 1));
	assert(0 == tar_atol256("\0\0", 2));
	assert(0 == tar_atol256("\0\0\0", 3));
	assert(0 == tar_atol256("\0\0\0\0", 4));
	assert(0 == tar_atol256("\0\0\0\0\0", 5));
	assert(0 == tar_atol256("\0\0\0\0\0\0", 6));
	assert(0 == tar_atol256("\0\0\0\0\0\0\0", 7));
	assert(0 == tar_atol256("\0\0\0\0\0\0\0\0", 8));
	assert(0 == tar_atol256("\0\0\0\0\0\0\0\0\0", 9));
	assert(0 == tar_atol256("\x80\0\0\0\0\0\0\0\0", 9));
	assert(0 == tar_atol256("\x80\0\0\0\0\0\0\0\0\0", 10));

	assert(-1 == tar_atol256("\x7f", 1));
	assert(-64 == tar_atol256("\x40", 1));
	assert(0 == tar_atol256("\x80", 1));
	assert(63 == tar_atol256("\x3f", 1));

	// Underflow
	assert(INT64_MIN == tar_atol256("\xff\0\0\0\0\0\0\0\0", 9));
	assert(INT64_MIN == tar_atol256("\xfe\xff\xff\xff\xff\xff\xff\xff\xff", 9));
	// Not underflow
	assert(INT64_MIN == tar_atol256("\xff\x80\0\0\0\0\0\0\0", 9));
	assert(INT64_MIN + 1 == tar_atol256("\xff\x80\0\0\0\0\0\0\x01", 9));
	// Overflow
	assert(INT64_MAX == tar_atol256("\x00\x80\0\0\0\0\0\0\0", 9));
	assert(INT64_MAX == tar_atol256("\0\xff\0\0\0\0\0\0\0\0", 10));
	assert(INT64_MAX == tar_atol256("\x80\xff\0\0\0\0\0\0\0\0", 10));
	assert(INT64_MAX == tar_atol256("\x90\0\0\0\0\0\0\0\0\0", 9));
	assert(INT64_MAX == tar_atol256("\x90\0\0\0\0\0\0\0\0\0", 10));


	for (len = 0; len < 8; ++len) {
		memset(buff, 0, sizeof(buff));
		for (long long i = 255; i >= 0; --i) {
			buff[0] = i;
			if (i & 0x40) {
				assert(
					(int64_t)(((~0ULL << 6) | (i & 0x3f)) << len * 8)
					== tar_atol256(buff, len + 1));
			} else {
				assert(((i & 0x3f) << (len * 8)) == tar_atol256(buff, len + 1));
			}
		}
	}

	return 0;
}
