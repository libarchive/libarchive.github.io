/**************************************************************************
 getopt.h - command line parser

 2007-05-25 jerker_back, created for PSXCRT

 $LastChangedBy: jerker_back $

 Based on implementation from NetBSD, FreeBSD and OpenBSD

===========================================================================

 NT POSIX C Runtime library
 http://sourceforge.net/projects/psxcrt

 Copyright 2010 PSXCRT project
 License: 2 clause BSD license, please read
 http://psxcrt.sourceforge.net/license/license.txt

===========================================================================
 RcsID = $Id: getopt.h 409 2011-02-08 05:50:29Z jerker_back $ */

#pragma once

#if !defined(_GETOPT_H_)
#define _GETOPT_H_ 1
#define _GETOPT_H 1
#include <features.h>

#if defined(__GNUC__) 
#pragma GCC system_header
#endif

// Most of the contents of this header should really be defined in unistd.h
// This header is compatible with and can replace GNU libc/GNUlib getopt.h

// Note: The reason for the excessive use of preprocessor macros below is 
// due to clashes between the system BSD getopt and GNU getopt.
// A GNU project should always use the GNU variant (local).
// define __USE_GNUGETOPT to use a local variant (e.g. to configure).

#pragma pack(push,8)

#if defined(__cplusplus)
extern "C" {
#endif

/* undef all possible variants of getopt */
#undef getopt
#undef getopt_long
#undef getopt_long_only
#undef optarg
#undef opterr
#undef optind
#undef optopt
#undef option
#undef no_argument
#undef required_argument
#undef optional_argument

typedef enum _argument {
	no_argument             = 0,
	required_argument       = 1,
	optional_argument       = 2
};
#define no_argument			no_argument
#define required_argument	required_argument
#define optional_argument	optional_argument

// Default is to use system implementation of getopt
#if defined(__USE_GNUGETOPT)
#define __GETOPT_LOCAL      loc_
#define __GETOPT_CONCAT(y)  ___CONCAT(__GETOPT_LOCAL, y)
#define getopt              __GETOPT_CONCAT(getopt)
#define getopt_long         __GETOPT_CONCAT(getopt_long)
#define getopt_long_only    __GETOPT_CONCAT(getopt_long_only)
#define optarg              __GETOPT_CONCAT(optarg)
#define opterr              __GETOPT_CONCAT(opterr)
#define optind              __GETOPT_CONCAT(optind)
#define optopt              __GETOPT_CONCAT(optopt)
#define option              __GETOPT_CONCAT(option)
#endif

#if !defined(_GETOPT_DATA_DEFINED)
#define _GETOPT_DATA_DEFINED 1
extern char*    optarg;
extern int      optind, opterr, optopt;
extern char*    suboptarg;	/* BSD */
extern int      optreset;	/* BSD */
#endif

// system global data through libc_global()
#if !defined(__USE_GNUGETOPT)
#define optarg              ( (char*)libc_global(G_OPTARG))
#define optind              (*((int*)libc_global(G_OPTIND)))
#define opterr              (*((int*)libc_global(G_OPTERR)))
#define optopt              (*((int*)libc_global(G_OPTOPT)))
#define suboptarg           (*((int*)libc_global(G_SUBOPTARG)))
#define optreset            (*((int*)libc_global(G_OPTRESET)))
#endif

#if !defined(_OPTION_T_DEFINED)
#define _OPTION_T_DEFINED 1
CRT_WARNING_DISABLE_PUSH(4820)	// padding added after data member
typedef struct option {			// intptr_t for padding
	const char*     name;       /* name of long option */
	int             has_arg;    /* one of no_argument, required_argument, and optional_argument: whether option takes an argument */
	int*            flag;       /* if not NULL, set *flag to val when option found */
	int             val;        /* if flag not NULL, value to set *flag to; else return value */
} option_t;
CRT_WARNING_POP
#endif

// -------------------------------------------------------------

#ifndef _GETOPT_DEFINED_
#define _GETOPT_DEFINED_ 1
LIBC_IMPEXP   int	__cdecl getopt(int argc, char* const argv[], const char* optstr);
#endif

LIBC_IMPEXP   int	__cdecl getsubopt(char** poptstr, char* const *pkeylist, char** pvalstr);
LIBC_IMPEXP   int	__cdecl getopt_long(int argc, char* const *argv, const char* optstr, const struct option* long_options, int* opt_index);
LIBC_IMPEXP   int	__cdecl getopt_long_only(int argc, char* const *argv, const char* optstr, const struct option* long_options, int* opt_index);

#if defined(__USE_GNUGETOPT)
// extern int __cdecl _getopt_internal(int argc, char** argv, const char* optstr, const struct option* long_options, int* opt_index, int long_only, int posixly_correct);
#endif

// -------------------------------------------------------------

#if defined(__cplusplus)
}
#endif
#pragma pack(pop)
#endif	// !defined(_GETOPT_H_)
#undef __need_getopt	// not used - expose all from unistd.h (IEEE Std 1003.1-2008)
