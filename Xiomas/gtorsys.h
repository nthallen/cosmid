// Header file common to all Genitor Generated Source Modules
//
#ifndef GTORSYS_INCLUDE
#define GTORSYS_INCLUDE

//lint -e537
#define __STDC_LIMIT_MACROS
#if defined(__linux__) && !defined(__LINUX__)
#include <glob.h>
#endif
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <memory.h>
#include <climits>
#include <limits.h>

#if defined(__linux__) && !defined(__LINUX__)
#define __LINUX__
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
# include <winsock2.h>
# include <windows.h>
# include <stdio.h>
# include <errno.h>
# include "errno_mingw.h"
# undef __CYGWIN32__
# undef __CYGWIN__
#endif

#if defined(__CYGWIN32__) && !defined(__CYGWIN__)
#define __CYGWIN__
#include <endian.h>
#endif

#if (defined(__CYGWIN__) || defined(__LINUX__)) && !defined(__ARMV7__)
# if !defined(__POSIX__)
# define __POSIX__
# endif
typedef int  SOCKET;
#endif

#if defined(WIN32) || defined(__CYGWIN__)
typedef void* HANDLE;
#else
typedef int HANDLE;
#endif

#if defined(__ARMV7__)
#define bzero(b, len) memset(b, 0, len)
#define iprintf printf
#define viprintf vprintf
#define siprintf sprintf
#define vsiprintf vsprintf

#endif

#if defined(__POSIX__) && !defined(WIN32)
# include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdarg.h>
# define INVALID_HANDLE_VALUE (HANDLE)(-1)
#endif

#if defined(__LINUX__) && !defined(__LINT__)
#define iprintf printf
#define viprintf vprintf
#define siprintf sprintf
#define vsiprintf vsprintf
#endif

// #include "XiomasTypes.h"

//------------------------------------------
// set endian macros
//------------------------------------------
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define __LITTLE_ENDIAN__
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
# define __BIG_ENDIAN__
#else
# error "__BYTE_ORDER not defined"
#endif

#if !defined(INT_MAX)
/* Minimum and maximum values a `signed int' can hold.  */
#  define INT_MIN	(-INT_MAX - 1)
#  define INT_MAX	2147483647

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#  define UINT_MAX	4294967295U
#endif

#define SRC_INLINE inline	// used to have code inlined in source module

#define  GTOR_BEGIN_NAMESPACE(n) namespace n {
#define  GTOR_END_NAMESPACE()      }

#ifndef EXTERN_C
# if __cplusplus
#  define EXTERN_C	extern "C"
# else
#  define EXTERN_C
# endif
#endif

#ifndef EXTERN
#define EXTERN	extern
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p) (p)=(p)	// to quiet compiler warnings
#endif

#ifdef LINT
// need to define prototype for LINT to succeed
//lint -esym(*,__builtin_va_start)
#include <stdarg.h>
EXTERN_C void __builtin_va_start(va_list v, ...);
#endif

#ifndef NULL
#define NULL	((void*)0)
#endif


#ifndef limit
//lint -e(652)
#define limit(v, mi, ma)	((v) < (mi) ? (mi) : ((v) < (ma) ? (v) : (ma)))
#endif

#define _export

#if !defined(__STRING)
#define __STRING(exp) #exp
#endif

// Setup assert to end up in dbgAssert()
#undef __assert
#undef FW_ASSERT
#undef ASSERT
#define ASSERT	FW_ASSERT
//#define ASSERT	assert
void dbgAssertFail(const char *pszAssertion, const char *pszFile,
			   unsigned int nLine, const char *pszFunction);
# define FW_ASSERT(expr) \
  (static_cast<void> ((expr) ? 0 :					      \
		       (dbgAssertFail (__STRING(expr), __FILE__, __LINE__, __FUNCTION__), 0)))

#define ATTR_NORETURN
#if defined(__CYGWIN__)
// Define ASSERT for program under CYGWIN
EXTERN_C void winAssert(
	const char *pszFile,
	int nLine,
	const char *pszTest);
//lint -e(652) -e(683)
//#define __assert winAssert
#elif defined(__LINUX__)
#undef ATTR_NORETURN
#define ATTR_NORETURN   __attribute__ ((__noreturn__))
//#define ATTR_NORETURN   _Noreturn

// Define ASSERT for program under LINUX
EXTERN_C void ATTR_NORETURN linuxAssert(
	const char *pszFile,
	const char *pszTest,
	int nLine);


EXTERN_C void ATTR_NORETURN linuxAssertFail(const char *pszAssertion, const char *pszFile,
			   unsigned int nLine, const char *pszFunction);

//#define __assert_fail linuxAssertFail
//#define __assert linuxAssert
#endif
//#include <assert.h>

//---------------------------------------------------------------------
// NOP method -- add processor as defined
//---------------------------------------------------------------------
#if defined(__X86__) || defined(__X64__) || defined(__X86__) || defined(__amd64__)
#define nop()	asm volatile("nop")
#elif defined(__ARM__)
#define nop()	asm volatile("nop");
#else
#	error "No Processor Defined"
#endif

#endif	// GTORSYS_INCLUDE
