/* ==========================================
    Hunt Project - A Test Framework for C
    Copyright (c) 2007-19 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#ifndef HUNT_INTERNALS_H
#define HUNT_INTERNALS_H

#ifdef HUNT_INCLUDE_CONFIG_H

#include "hunt_config.h"

#endif

#ifndef HUNT_EXCLUDE_SETJMP_H

#include <setjmp.h>

#endif

#ifndef HUNT_EXCLUDE_MATH_H

#include <math.h>

#endif

#ifndef HUNT_EXCLUDE_STDDEF_H

#include <stddef.h>

#endif

#ifdef HUNT_INCLUDE_PRINT_FORMATTED
#include <stdarg.h>
#endif

/* Hunt Attempts to Auto-Detect Integer Types
 * Attempt 1: UINT_MAX, ULONG_MAX in <limits.h>, or default to 32 bits
 * Attempt 2: UINTPTR_MAX in <stdint.h>, or default to same size as long
 * The user may override any of these derived constants:
 * HUNT_INT_WIDTH, HUNT_LONG_WIDTH, HUNT_POINTER_WIDTH */
#ifndef HUNT_EXCLUDE_STDINT_H

#include <stdint.h>

#endif

#ifndef HUNT_EXCLUDE_LIMITS_H

#include <limits.h>

#endif

/*-------------------------------------------------------
 * Guess Widths If Not Specified
 *-------------------------------------------------------*/

/* Determine the size of an int, if not already specified.
 * We cannot use sizeof(int), because it is not yet defined
 * at this stage in the translation of the C program.
 * Also sizeof(int) does return the size in addressable units on all platforms,
 * which may not necessarily be the size in bytes.
 * Therefore, infer it from UINT_MAX if possible. */
#ifndef HUNT_INT_WIDTH
#ifdef UINT_MAX
#if (UINT_MAX == 0xFFFF)
#define HUNT_INT_WIDTH (16)
#elif (UINT_MAX == 0xFFFFFFFF)
#define HUNT_INT_WIDTH (32)
#elif (UINT_MAX == 0xFFFFFFFFFFFFFFFF)
#define HUNT_INT_WIDTH (64)
#endif
#else /* Set to default */
#define HUNT_INT_WIDTH (32)
#endif /* UINT_MAX */
#endif

/* Determine the size of a long, if not already specified. */
#ifndef HUNT_LONG_WIDTH
#ifdef ULONG_MAX
#if (ULONG_MAX == 0xFFFF)
#define HUNT_LONG_WIDTH (16)
#elif (ULONG_MAX == 0xFFFFFFFF)
#define HUNT_LONG_WIDTH (32)
#elif (ULONG_MAX == 0xFFFFFFFFFFFFFFFF)
#define HUNT_LONG_WIDTH (64)
#endif
#else /* Set to default */
#define HUNT_LONG_WIDTH (32)
#endif /* ULONG_MAX */
#endif

/* Determine the size of a pointer, if not already specified. */
#ifndef HUNT_POINTER_WIDTH
#ifdef UINTPTR_MAX
#if (UINTPTR_MAX <= 0xFFFF)
#define HUNT_POINTER_WIDTH (16)
#elif (UINTPTR_MAX <= 0xFFFFFFFF)
#define HUNT_POINTER_WIDTH (32)
#elif (UINTPTR_MAX <= 0xFFFFFFFFFFFFFFFF)
#define HUNT_POINTER_WIDTH (64)
#endif
#else /* Set to default */
#define HUNT_POINTER_WIDTH HUNT_LONG_WIDTH
#endif /* UINTPTR_MAX */
#endif

/*-------------------------------------------------------
 * Int Support (Define types based on detected sizes)
 *-------------------------------------------------------*/

#if (HUNT_INT_WIDTH == 32)
typedef unsigned char HUNT_UINT8;
typedef unsigned short HUNT_UINT16;
typedef unsigned int HUNT_UINT32;
typedef signed char HUNT_INT8;
typedef signed short HUNT_INT16;
typedef signed int HUNT_INT32;
#elif (HUNT_INT_WIDTH == 16)
typedef unsigned char   HUNT_UINT8;
typedef unsigned int    HUNT_UINT16;
typedef unsigned long   HUNT_UINT32;
typedef signed char     HUNT_INT8;
typedef signed int      HUNT_INT16;
typedef signed long     HUNT_INT32;
#else
#error Invalid HUNT_INT_WIDTH specified! (16 or 32 are supported)
#endif

/*-------------------------------------------------------
 * 64-bit Support
 *-------------------------------------------------------*/

/* Auto-detect 64 Bit Support */
#ifndef HUNT_SUPPORT_64
#if HUNT_LONG_WIDTH == 64 || HUNT_POINTER_WIDTH == 64
#define HUNT_SUPPORT_64
#endif
#endif

/* 64-Bit Support Dependent Configuration */
#ifndef HUNT_SUPPORT_64
/* No 64-bit Support */
typedef HUNT_UINT32 HUNT_UINT;
typedef HUNT_INT32  HUNT_INT;
#define HUNT_MAX_NIBBLES (8)  /* Maximum number of nibbles in a HUNT_(U)INT */
#else
/* 64-bit Support */
#if (HUNT_LONG_WIDTH == 32)
typedef unsigned long long HUNT_UINT64;
typedef signed long long   HUNT_INT64;
#elif (HUNT_LONG_WIDTH == 64)
typedef unsigned long HUNT_UINT64;
typedef signed long HUNT_INT64;
#else
#error Invalid HUNT_LONG_WIDTH specified! (32 or 64 are supported)
#endif
typedef HUNT_UINT64 HUNT_UINT;
typedef HUNT_INT64 HUNT_INT;
#define HUNT_MAX_NIBBLES (16) /* Maximum number of nibbles in a HUNT_(U)INT */
#endif

/*-------------------------------------------------------
 * Pointer Support
 *-------------------------------------------------------*/

#if (HUNT_POINTER_WIDTH == 32)
#define HUNT_PTR_TO_INT HUNT_INT32
#define HUNT_DISPLAY_STYLE_POINTER HUNT_DISPLAY_STYLE_HEX32
#elif (HUNT_POINTER_WIDTH == 64)
#define HUNT_PTR_TO_INT HUNT_INT64
#define HUNT_DISPLAY_STYLE_POINTER HUNT_DISPLAY_STYLE_HEX64
#elif (HUNT_POINTER_WIDTH == 16)
#define HUNT_PTR_TO_INT HUNT_INT16
#define HUNT_DISPLAY_STYLE_POINTER HUNT_DISPLAY_STYLE_HEX16
#else
#error Invalid HUNT_POINTER_WIDTH specified! (16, 32 or 64 are supported)
#endif

#ifndef HUNT_PTR_ATTRIBUTE
#define HUNT_PTR_ATTRIBUTE
#endif

#ifndef HUNT_INTERNAL_PTR
#define HUNT_INTERNAL_PTR HUNT_PTR_ATTRIBUTE const void*
#endif

/*-------------------------------------------------------
 * Float Support
 *-------------------------------------------------------*/

#ifdef HUNT_EXCLUDE_FLOAT

/* No Floating Point Support */
#ifndef HUNT_EXCLUDE_DOUBLE
#define HUNT_EXCLUDE_DOUBLE /* Remove double when excluding float support */
#endif
#ifndef HUNT_EXCLUDE_FLOAT_PRINT
#define HUNT_EXCLUDE_FLOAT_PRINT
#endif

#else

/* Floating Point Support */
#ifndef HUNT_FLOAT_PRECISION
#define HUNT_FLOAT_PRECISION (0.00001f)
#endif
#ifndef HUNT_FLOAT_TYPE
#define HUNT_FLOAT_TYPE float
#endif
typedef HUNT_FLOAT_TYPE HUNT_FLOAT;

/* isinf & isnan macros should be provided by math.h */
#ifndef isinf
/* The value of Inf - Inf is NaN */
#define isinf(n) (isnan((n) - (n)) && !isnan(n))
#endif

#ifndef isnan
/* NaN is the only floating point value that does NOT equal itself.
 * Therefore if n != n, then it is NaN. */
#define isnan(n) ((n != n) ? 1 : 0)
#endif

#endif

/*-------------------------------------------------------
 * Double Float Support
 *-------------------------------------------------------*/

/* unlike float, we DON'T include by default */
#if defined(HUNT_EXCLUDE_DOUBLE) || !defined(HUNT_INCLUDE_DOUBLE)

/* No Floating Point Support */
#ifndef HUNT_EXCLUDE_DOUBLE
#define HUNT_EXCLUDE_DOUBLE
#else
#undef HUNT_INCLUDE_DOUBLE
#endif

#ifndef HUNT_EXCLUDE_FLOAT
#ifndef HUNT_DOUBLE_TYPE
#define HUNT_DOUBLE_TYPE double
#endif
typedef HUNT_FLOAT HUNT_DOUBLE;
/* For parameter in HuntPrintFloat(HUNT_DOUBLE), which aliases to double or float */
#endif

#else

/* Double Floating Point Support */
#ifndef HUNT_DOUBLE_PRECISION
#define HUNT_DOUBLE_PRECISION (1e-12)
#endif

#ifndef HUNT_DOUBLE_TYPE
#define HUNT_DOUBLE_TYPE double
#endif
typedef HUNT_DOUBLE_TYPE HUNT_DOUBLE;

#endif

/*-------------------------------------------------------
 * Output Method: stdout (DEFAULT)
 *-------------------------------------------------------*/
#ifndef HUNT_OUTPUT_CHAR
/* Default to using putchar, which is defined in stdio.h */
#include <stdio.h>
#define HUNT_OUTPUT_CHAR(a) (void)putchar(a)
#else
/* If defined as something else, make sure we declare it here so it's ready for use */
#ifdef HUNT_OUTPUT_CHAR_HEADER_DECLARATION
extern void HUNT_OUTPUT_CHAR_HEADER_DECLARATION;
#endif
#endif

#ifndef HUNT_OUTPUT_FLUSH
#ifdef HUNT_USE_FLUSH_STDOUT
/* We want to use the stdout flush utility */
#include <stdio.h>
#define HUNT_OUTPUT_FLUSH() (void)fflush(stdout)
#else
/* We've specified nothing, therefore flush should just be ignored */
#define HUNT_OUTPUT_FLUSH()
#endif
#else
/* If defined as something else, make sure we declare it here so it's ready for use */
#ifdef HUNT_OUTPUT_FLUSH_HEADER_DECLARATION
extern void HUNT_OUTPUT_FLUSH_HEADER_DECLARATION;
#endif
#endif

#ifndef HUNT_OUTPUT_FLUSH
#define HUNT_FLUSH_CALL()
#else
#define HUNT_FLUSH_CALL() HUNT_OUTPUT_FLUSH()
#endif

#ifndef HUNT_PRINT_EOL
#define HUNT_PRINT_EOL()    HUNT_OUTPUT_CHAR('\n')
#endif

#ifndef HUNT_OUTPUT_START
#define HUNT_OUTPUT_START()
#endif

#ifndef HUNT_OUTPUT_COMPLETE
#define HUNT_OUTPUT_COMPLETE()
#endif

#ifdef HUNT_INCLUDE_EXEC_TIME
#if !defined(HUNT_EXEC_TIME_START) && \
      !defined(HUNT_EXEC_TIME_STOP) && \
      !defined(HUNT_PRINT_EXEC_TIME) && \
      !defined(HUNT_TIME_TYPE)
/* If none any of these macros are defined then try to provide a default implementation */

#if defined(HUNT_CLOCK_MS)
/* This is a simple way to get a default implementation on platforms that support getting a millisecond counter */
#define HUNT_TIME_TYPE HUNT_UINT
#define HUNT_EXEC_TIME_START() Hunt.CurrentTestStartTime = HUNT_CLOCK_MS()
#define HUNT_EXEC_TIME_STOP() Hunt.CurrentTestStopTime = HUNT_CLOCK_MS()
#define HUNT_PRINT_EXEC_TIME() { \
        HUNT_UINT execTimeMs = (Hunt.CurrentTestStopTime - Hunt.CurrentTestStartTime); \
        HuntPrint(" ("); \
        HuntPrintNumberUnsigned(execTimeMs); \
        HuntPrint(" ms)"); \
        }
#elif defined(_WIN32)
#include <time.h>
#define HUNT_TIME_TYPE clock_t
#define HUNT_GET_TIME(t) t = (clock_t)((clock() * 1000) / CLOCKS_PER_SEC)
#define HUNT_EXEC_TIME_START() HUNT_GET_TIME(Hunt.CurrentTestStartTime)
#define HUNT_EXEC_TIME_STOP() HUNT_GET_TIME(Hunt.CurrentTestStopTime)
#define HUNT_PRINT_EXEC_TIME() { \
        HUNT_UINT execTimeMs = (Hunt.CurrentTestStopTime - Hunt.CurrentTestStartTime); \
        HuntPrint(" ("); \
        HuntPrintNumberUnsigned(execTimeMs); \
        HuntPrint(" ms)"); \
        }
#elif defined(__unix__)
#include <time.h>
#define HUNT_TIME_TYPE struct timespec
#define HUNT_GET_TIME(t) clock_gettime(CLOCK_MONOTONIC, &t)
#define HUNT_EXEC_TIME_START() HUNT_GET_TIME(Hunt.CurrentTestStartTime)
#define HUNT_EXEC_TIME_STOP() HUNT_GET_TIME(Hunt.CurrentTestStopTime)
#define HUNT_PRINT_EXEC_TIME() { \
        HUNT_UINT execTimeMs = ((Hunt.CurrentTestStopTime.tv_sec - Hunt.CurrentTestStartTime.tv_sec) * 1000L); \
        execTimeMs += ((Hunt.CurrentTestStopTime.tv_nsec - Hunt.CurrentTestStartTime.tv_nsec) / 1000000L); \
        HuntPrint(" ("); \
        HuntPrintNumberUnsigned(execTimeMs); \
        HuntPrint(" ms)"); \
        }
#endif
#endif
#endif

#ifndef HUNT_EXEC_TIME_START
#define HUNT_EXEC_TIME_START() do{}while(0)
#endif

#ifndef HUNT_EXEC_TIME_STOP
#define HUNT_EXEC_TIME_STOP() do{}while(0)
#endif

#ifndef HUNT_TIME_TYPE
#define HUNT_TIME_TYPE HUNT_UINT
#endif

#ifndef HUNT_PRINT_EXEC_TIME
#define HUNT_PRINT_EXEC_TIME() do{}while(0)
#endif

/*-------------------------------------------------------
 * Footprint
 *-------------------------------------------------------*/

#ifndef HUNT_LINE_TYPE
#define HUNT_LINE_TYPE HUNT_UINT
#endif

#ifndef HUNT_COUNTER_TYPE
#define HUNT_COUNTER_TYPE HUNT_UINT
#endif

/*-------------------------------------------------------
 * Language Features Available
 *-------------------------------------------------------*/
#if !defined(HUNT_WEAK_ATTRIBUTE) && !defined(HUNT_WEAK_PRAGMA)
#   if defined(__GNUC__) || defined(__ghs__) /* __GNUC__ includes clang */
#       if !(defined(__WIN32__) && defined(__clang__)) && !defined(__TMS470__)
#           define HUNT_WEAK_ATTRIBUTE __attribute__((weak))
#       endif
#   endif
#endif

#ifdef HUNT_NO_WEAK
#   undef HUNT_WEAK_ATTRIBUTE
#   undef HUNT_WEAK_PRAGMA
#endif

/*-------------------------------------------------------
 * Internal Structs Needed
 *-------------------------------------------------------*/

typedef void (*HuntTestFunction)(void);

#define HUNT_DISPLAY_RANGE_INT  (0x10)
#define HUNT_DISPLAY_RANGE_UINT (0x20)
#define HUNT_DISPLAY_RANGE_HEX  (0x40)

typedef enum {
  HUNT_DISPLAY_STYLE_INT = (HUNT_INT_WIDTH / 8) + HUNT_DISPLAY_RANGE_INT,
  HUNT_DISPLAY_STYLE_INT8 = 1 + HUNT_DISPLAY_RANGE_INT,
  HUNT_DISPLAY_STYLE_INT16 = 2 + HUNT_DISPLAY_RANGE_INT,
  HUNT_DISPLAY_STYLE_INT32 = 4 + HUNT_DISPLAY_RANGE_INT,
#ifdef HUNT_SUPPORT_64
  HUNT_DISPLAY_STYLE_INT64 = 8 + HUNT_DISPLAY_RANGE_INT,
#endif

  HUNT_DISPLAY_STYLE_UINT = (HUNT_INT_WIDTH / 8) + HUNT_DISPLAY_RANGE_UINT,
  HUNT_DISPLAY_STYLE_UINT8 = 1 + HUNT_DISPLAY_RANGE_UINT,
  HUNT_DISPLAY_STYLE_UINT16 = 2 + HUNT_DISPLAY_RANGE_UINT,
  HUNT_DISPLAY_STYLE_UINT32 = 4 + HUNT_DISPLAY_RANGE_UINT,
#ifdef HUNT_SUPPORT_64
  HUNT_DISPLAY_STYLE_UINT64 = 8 + HUNT_DISPLAY_RANGE_UINT,
#endif

  HUNT_DISPLAY_STYLE_HEX8 = 1 + HUNT_DISPLAY_RANGE_HEX,
  HUNT_DISPLAY_STYLE_HEX16 = 2 + HUNT_DISPLAY_RANGE_HEX,
  HUNT_DISPLAY_STYLE_HEX32 = 4 + HUNT_DISPLAY_RANGE_HEX,
#ifdef HUNT_SUPPORT_64
  HUNT_DISPLAY_STYLE_HEX64 = 8 + HUNT_DISPLAY_RANGE_HEX,
#endif

  HUNT_DISPLAY_STYLE_UNKNOWN
} HUNT_DISPLAY_STYLE_T;

typedef enum {
  HUNT_WITHIN = 0x0,
  HUNT_EQUAL_TO = 0x1,
  HUNT_GREATER_THAN = 0x2,
  HUNT_GREATER_OR_EQUAL = 0x2 + HUNT_EQUAL_TO,
  HUNT_SMALLER_THAN = 0x4,
  HUNT_SMALLER_OR_EQUAL = 0x4 + HUNT_EQUAL_TO,
  HUNT_UNKNOWN
} HUNT_COMPARISON_T;

#ifndef HUNT_EXCLUDE_FLOAT
typedef enum HUNT_FLOAT_TRAIT {
  HUNT_FLOAT_IS_NOT_INF = 0,
  HUNT_FLOAT_IS_INF,
  HUNT_FLOAT_IS_NOT_NEG_INF,
  HUNT_FLOAT_IS_NEG_INF,
  HUNT_FLOAT_IS_NOT_NAN,
  HUNT_FLOAT_IS_NAN,
  HUNT_FLOAT_IS_NOT_DET,
  HUNT_FLOAT_IS_DET,
  HUNT_FLOAT_INVALID_TRAIT
} HUNT_FLOAT_TRAIT_T;
#endif

typedef enum {
  HUNT_ARRAY_TO_VAL = 0,
  HUNT_ARRAY_TO_ARRAY,
  HUNT_ARRAY_UNKNOWN
} HUNT_FLAGS_T;

struct HUNT_STORAGE_T {
  const char *TestFile;
  const char *CurrentTestName;
#ifndef HUNT_EXCLUDE_DETAILS
  const char *CurrentDetail1;
  const char *CurrentDetail2;
#endif
  HUNT_LINE_TYPE CurrentTestLineNumber;
  HUNT_COUNTER_TYPE NumberOfTests;
  HUNT_COUNTER_TYPE TestFailures;
  HUNT_COUNTER_TYPE TestIgnores;
  HUNT_COUNTER_TYPE CurrentTestFailed;
  HUNT_COUNTER_TYPE CurrentTestIgnored;
#ifdef HUNT_INCLUDE_EXEC_TIME
  HUNT_TIME_TYPE CurrentTestStartTime;
  HUNT_TIME_TYPE CurrentTestStopTime;
#endif
#ifndef HUNT_EXCLUDE_SETJMP_H
  jmp_buf AbortFrame;
#endif
};

extern struct HUNT_STORAGE_T Hunt;

/*-------------------------------------------------------
 * Test Suite Management
 *-------------------------------------------------------*/

void HuntBegin(const char *filename);

int HuntEnd(void);

void HuntConcludeTest(void);

void HuntDefaultTestRun(HuntTestFunction Func, const char *FuncName, const int FuncLineNum);

/*-------------------------------------------------------
 * Details Support
 *-------------------------------------------------------*/

#ifdef HUNT_EXCLUDE_DETAILS
#define HUNT_CLR_DETAILS()
#define HUNT_SET_DETAIL(d1)
#define HUNT_SET_DETAILS(d1,d2)
#else
#define HUNT_CLR_DETAILS()      { Hunt.CurrentDetail1 = 0;   Hunt.CurrentDetail2 = 0;  }
#define HUNT_SET_DETAIL(d1)     { Hunt.CurrentDetail1 = (d1);  Hunt.CurrentDetail2 = 0;  }
#define HUNT_SET_DETAILS(d1, d2) { Hunt.CurrentDetail1 = (d1);  Hunt.CurrentDetail2 = (d2); }

#ifndef HUNT_DETAIL1_NAME
#define HUNT_DETAIL1_NAME "Function"
#endif

#ifndef HUNT_DETAIL2_NAME
#define HUNT_DETAIL2_NAME "Argument"
#endif
#endif

/*-------------------------------------------------------
 * Test Output
 *-------------------------------------------------------*/

void HuntPrint(const char *string);

#ifdef HUNT_INCLUDE_PRINT_FORMATTED
void HuntPrintFormatted(const char* format, ...);
#endif

void HuntPrintLen(const char *string, const HUNT_UINT32 length);

void HuntPrintMask(const HUNT_UINT mask, const HUNT_UINT number);

void HuntPrintNumberByStyle(const HUNT_INT number, const HUNT_DISPLAY_STYLE_T style);

void HuntPrintNumber(const HUNT_INT number_to_print);

void HuntPrintNumberUnsigned(const HUNT_UINT number);

void HuntPrintNumberHex(const HUNT_UINT number, const char nibbles_to_print);

#ifndef HUNT_EXCLUDE_FLOAT_PRINT

void HuntPrintFloat(const HUNT_DOUBLE input_number);

#endif

/*-------------------------------------------------------
 * Test Assertion Functions
 *-------------------------------------------------------
 *  Use the macros below this section instead of calling
 *  these directly. The macros have a consistent naming
 *  convention and will pull in file and line information
 *  for you. */

void HuntAssertEqualNumber(const HUNT_INT expected,
                           const HUNT_INT actual,
                           const char *msg,
                           const HUNT_LINE_TYPE lineNumber,
                           const HUNT_DISPLAY_STYLE_T style);

void HuntAssertGreaterOrLessOrEqualNumber(const HUNT_INT threshold,
                                          const HUNT_INT actual,
                                          const HUNT_COMPARISON_T compare,
                                          const char *msg,
                                          const HUNT_LINE_TYPE lineNumber,
                                          const HUNT_DISPLAY_STYLE_T style);

void HuntAssertEqualIntArray(HUNT_INTERNAL_PTR expected,
                             HUNT_INTERNAL_PTR actual,
                             const HUNT_UINT32 num_elements,
                             const char *msg,
                             const HUNT_LINE_TYPE lineNumber,
                             const HUNT_DISPLAY_STYLE_T style,
                             const HUNT_FLAGS_T flags);

void HuntAssertBits(const HUNT_INT mask,
                    const HUNT_INT expected,
                    const HUNT_INT actual,
                    const char *msg,
                    const HUNT_LINE_TYPE lineNumber);

void HuntAssertEqualString(const char *expected,
                           const char *actual,
                           const char *msg,
                           const HUNT_LINE_TYPE lineNumber);

void HuntAssertEqualStringLen(const char *expected,
                              const char *actual,
                              const HUNT_UINT32 length,
                              const char *msg,
                              const HUNT_LINE_TYPE lineNumber);

void HuntAssertEqualStringArray(HUNT_INTERNAL_PTR expected,
                                const char **actual,
                                const HUNT_UINT32 num_elements,
                                const char *msg,
                                const HUNT_LINE_TYPE lineNumber,
                                const HUNT_FLAGS_T flags);

void HuntAssertEqualMemory(HUNT_INTERNAL_PTR expected,
                           HUNT_INTERNAL_PTR actual,
                           const HUNT_UINT32 length,
                           const HUNT_UINT32 num_elements,
                           const char *msg,
                           const HUNT_LINE_TYPE lineNumber,
                           const HUNT_FLAGS_T flags);

void HuntAssertNumbersWithin(const HUNT_UINT delta,
                             const HUNT_INT expected,
                             const HUNT_INT actual,
                             const char *msg,
                             const HUNT_LINE_TYPE lineNumber,
                             const HUNT_DISPLAY_STYLE_T style);

void HuntAssertNumbersArrayWithin(const HUNT_UINT delta,
                                  HUNT_INTERNAL_PTR expected,
                                  HUNT_INTERNAL_PTR actual,
                                  const HUNT_UINT32 num_elements,
                                  const char *msg,
                                  const HUNT_LINE_TYPE lineNumber,
                                  const HUNT_DISPLAY_STYLE_T style,
                                  const HUNT_FLAGS_T flags);

void HuntFail(const char *message, const HUNT_LINE_TYPE line);

void HuntIgnore(const char *message, const HUNT_LINE_TYPE line);

void HuntMessage(const char *message, const HUNT_LINE_TYPE line);

#ifndef HUNT_EXCLUDE_FLOAT

void HuntAssertFloatsWithin(const HUNT_FLOAT delta,
                            const HUNT_FLOAT expected,
                            const HUNT_FLOAT actual,
                            const char *msg,
                            const HUNT_LINE_TYPE lineNumber);

void HuntAssertEqualFloatArray(HUNT_PTR_ATTRIBUTE const HUNT_FLOAT *expected,
                               HUNT_PTR_ATTRIBUTE const HUNT_FLOAT *actual,
                               const HUNT_UINT32 num_elements,
                               const char *msg,
                               const HUNT_LINE_TYPE lineNumber,
                               const HUNT_FLAGS_T flags);

void HuntAssertFloatSpecial(const HUNT_FLOAT actual,
                            const char *msg,
                            const HUNT_LINE_TYPE lineNumber,
                            const HUNT_FLOAT_TRAIT_T style);

#endif

#ifndef HUNT_EXCLUDE_DOUBLE
void HuntAssertDoublesWithin(const HUNT_DOUBLE delta,
                              const HUNT_DOUBLE expected,
                              const HUNT_DOUBLE actual,
                              const char* msg,
                              const HUNT_LINE_TYPE lineNumber);

void HuntAssertEqualDoubleArray(HUNT_PTR_ATTRIBUTE const HUNT_DOUBLE* expected,
                                 HUNT_PTR_ATTRIBUTE const HUNT_DOUBLE* actual,
                                 const HUNT_UINT32 num_elements,
                                 const char* msg,
                                 const HUNT_LINE_TYPE lineNumber,
                                 const HUNT_FLAGS_T flags);

void HuntAssertDoubleSpecial(const HUNT_DOUBLE actual,
                              const char* msg,
                              const HUNT_LINE_TYPE lineNumber,
                              const HUNT_FLOAT_TRAIT_T style);
#endif

/*-------------------------------------------------------
 * Helpers
 *-------------------------------------------------------*/

HUNT_INTERNAL_PTR HuntNumToPtr(const HUNT_INT num, const HUNT_UINT8 size);

#ifndef HUNT_EXCLUDE_FLOAT

HUNT_INTERNAL_PTR HuntFloatToPtr(const float num);

#endif
#ifndef HUNT_EXCLUDE_DOUBLE
HUNT_INTERNAL_PTR HuntDoubleToPtr(const double num);
#endif

/*-------------------------------------------------------
 * Error Strings We Might Need
 *-------------------------------------------------------*/

extern const char HuntStrOk[];
extern const char HuntStrPass[];
extern const char HuntStrFail[];
extern const char HuntStrIgnore[];

extern const char HuntStrErrFloat[];
extern const char HuntStrErrDouble[];
extern const char HuntStrErr64[];
extern const char HuntStrErrShorthand[];

/*-------------------------------------------------------
 * Test Running Macros
 *-------------------------------------------------------*/

#ifndef HUNT_EXCLUDE_SETJMP_H
#define TEST_PROTECT() (setjmp(Hunt.AbortFrame) == 0)
#define TEST_ABORT() longjmp(Hunt.AbortFrame, 1)
#else
#define TEST_PROTECT() 1
#define TEST_ABORT() return
#endif

/* This tricky series of macros gives us an optional line argument to treat it as RUN_TEST(func, num=__LINE__) */
#ifndef RUN_TEST
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 199901L
#define RUN_TEST(...) HuntDefaultTestRun(RUN_TEST_FIRST(__VA_ARGS__), RUN_TEST_SECOND(__VA_ARGS__))
#define RUN_TEST_FIRST(...) RUN_TEST_FIRST_HELPER(__VA_ARGS__, throwaway)
#define RUN_TEST_FIRST_HELPER(first, ...) (first), #first
#define RUN_TEST_SECOND(...) RUN_TEST_SECOND_HELPER(__VA_ARGS__, __LINE__, throwaway)
#define RUN_TEST_SECOND_HELPER(first, second, ...) (second)
#endif
#endif
#endif

/* If we can't do the tricky version, we'll just have to require them to always include the line number */
#ifndef RUN_TEST
#ifdef CMOCK
#define RUN_TEST(func, num) HuntDefaultTestRun(func, #func, num)
#else
#define RUN_TEST(func) HuntDefaultTestRun(func, #func, __LINE__)
#endif
#endif

#define TEST_LINE_NUM (Hunt.CurrentTestLineNumber)
#define TEST_IS_IGNORED (Hunt.CurrentTestIgnored)
#define HUNT_NEW_TEST(a) \
    Hunt.CurrentTestName = (a); \
    Hunt.CurrentTestLineNumber = (HUNT_LINE_TYPE)(__LINE__); \
    Hunt.NumberOfTests++;

#ifndef HUNT_BEGIN
#define HUNT_BEGIN() HuntBegin(__FILE__)
#endif

#ifndef HUNT_END
#define HUNT_END() HuntEnd()
#endif

#ifndef HUNT_SHORTHAND_AS_INT
#ifndef HUNT_SHORTHAND_AS_MEM
#ifndef HUNT_SHORTHAND_AS_NONE
#ifndef HUNT_SHORTHAND_AS_RAW
#define HUNT_SHORTHAND_AS_OLD
#endif
#endif
#endif
#endif

/*-----------------------------------------------
 * Command Line Argument Support
 *-----------------------------------------------*/

#ifdef HUNT_USE_COMMAND_LINE_ARGS
int HuntParseOptions(int argc, char** argv);
int HuntTestMatches(void);
#endif

/*-------------------------------------------------------
 * Basic Fail and Ignore
 *-------------------------------------------------------*/

#define HUNT_TEST_FAIL(line, message)   HuntFail(   (message), (HUNT_LINE_TYPE)(line))
#define HUNT_TEST_IGNORE(line, message) HuntIgnore( (message), (HUNT_LINE_TYPE)(line))

/*-------------------------------------------------------
 * Test Asserts
 *-------------------------------------------------------*/

#define HUNT_TEST_ASSERT(condition, line, message)                                              if (condition) {} else {HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), (message));}
#define HUNT_TEST_ASSERT_NULL(pointer, line, message)                                           HUNT_TEST_ASSERT(((pointer) == NULL),  (HUNT_LINE_TYPE)(line), (message))
#define HUNT_TEST_ASSERT_NOT_NULL(pointer, line, message)                                       HUNT_TEST_ASSERT(((pointer) != NULL),  (HUNT_LINE_TYPE)(line), (message))

#define HUNT_TEST_ASSERT_EQUAL_INT(expected, actual, line, message)                             HuntAssertEqualNumber((HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT)
#define HUNT_TEST_ASSERT_EQUAL_INT8(expected, actual, line, message)                            HuntAssertEqualNumber((HUNT_INT)(HUNT_INT8 )(expected), (HUNT_INT)(HUNT_INT8 )(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8)
#define HUNT_TEST_ASSERT_EQUAL_INT16(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(HUNT_INT16)(expected), (HUNT_INT)(HUNT_INT16)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16)
#define HUNT_TEST_ASSERT_EQUAL_INT32(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(HUNT_INT32)(expected), (HUNT_INT)(HUNT_INT32)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32)
#define HUNT_TEST_ASSERT_EQUAL_UINT(expected, actual, line, message)                            HuntAssertEqualNumber((HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT)
#define HUNT_TEST_ASSERT_EQUAL_UINT8(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(HUNT_UINT8 )(expected), (HUNT_INT)(HUNT_UINT8 )(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8)
#define HUNT_TEST_ASSERT_EQUAL_UINT16(expected, actual, line, message)                          HuntAssertEqualNumber((HUNT_INT)(HUNT_UINT16)(expected), (HUNT_INT)(HUNT_UINT16)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16)
#define HUNT_TEST_ASSERT_EQUAL_UINT32(expected, actual, line, message)                          HuntAssertEqualNumber((HUNT_INT)(HUNT_UINT32)(expected), (HUNT_INT)(HUNT_UINT32)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32)
#define HUNT_TEST_ASSERT_EQUAL_HEX8(expected, actual, line, message)                            HuntAssertEqualNumber((HUNT_INT)(HUNT_INT8 )(expected), (HUNT_INT)(HUNT_INT8 )(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8)
#define HUNT_TEST_ASSERT_EQUAL_HEX16(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(HUNT_INT16)(expected), (HUNT_INT)(HUNT_INT16)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16)
#define HUNT_TEST_ASSERT_EQUAL_HEX32(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(HUNT_INT32)(expected), (HUNT_INT)(HUNT_INT32)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32)
#define HUNT_TEST_ASSERT_BITS(mask, expected, actual, line, message)                            HuntAssertBits((HUNT_INT)(mask), (HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line))

#define HUNT_TEST_ASSERT_GREATER_THAN_INT(threshold, actual, line, message)                     HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold),              (HUNT_INT)(actual),              HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT)
#define HUNT_TEST_ASSERT_GREATER_THAN_INT8(threshold, actual, line, message)                    HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT8 )(threshold), (HUNT_INT)(HUNT_INT8 )(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8)
#define HUNT_TEST_ASSERT_GREATER_THAN_INT16(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT16)(threshold), (HUNT_INT)(HUNT_INT16)(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16)
#define HUNT_TEST_ASSERT_GREATER_THAN_INT32(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT32)(threshold), (HUNT_INT)(HUNT_INT32)(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32)
#define HUNT_TEST_ASSERT_GREATER_THAN_UINT(threshold, actual, line, message)                    HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold),              (HUNT_INT)(actual),              HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT)
#define HUNT_TEST_ASSERT_GREATER_THAN_UINT8(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8)
#define HUNT_TEST_ASSERT_GREATER_THAN_UINT16(threshold, actual, line, message)                  HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16)
#define HUNT_TEST_ASSERT_GREATER_THAN_UINT32(threshold, actual, line, message)                  HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32)
#define HUNT_TEST_ASSERT_GREATER_THAN_HEX8(threshold, actual, line, message)                    HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8)
#define HUNT_TEST_ASSERT_GREATER_THAN_HEX16(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16)
#define HUNT_TEST_ASSERT_GREATER_THAN_HEX32(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_GREATER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32)

#define HUNT_TEST_ASSERT_SMALLER_THAN_INT(threshold, actual, line, message)                     HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold),              (HUNT_INT)(actual),              HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT)
#define HUNT_TEST_ASSERT_SMALLER_THAN_INT8(threshold, actual, line, message)                    HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT8 )(threshold), (HUNT_INT)(HUNT_INT8 )(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8)
#define HUNT_TEST_ASSERT_SMALLER_THAN_INT16(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT16)(threshold), (HUNT_INT)(HUNT_INT16)(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16)
#define HUNT_TEST_ASSERT_SMALLER_THAN_INT32(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT32)(threshold), (HUNT_INT)(HUNT_INT32)(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32)
#define HUNT_TEST_ASSERT_SMALLER_THAN_UINT(threshold, actual, line, message)                    HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold),              (HUNT_INT)(actual),              HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT)
#define HUNT_TEST_ASSERT_SMALLER_THAN_UINT8(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8)
#define HUNT_TEST_ASSERT_SMALLER_THAN_UINT16(threshold, actual, line, message)                  HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16)
#define HUNT_TEST_ASSERT_SMALLER_THAN_UINT32(threshold, actual, line, message)                  HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32)
#define HUNT_TEST_ASSERT_SMALLER_THAN_HEX8(threshold, actual, line, message)                    HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8)
#define HUNT_TEST_ASSERT_SMALLER_THAN_HEX16(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16)
#define HUNT_TEST_ASSERT_SMALLER_THAN_HEX32(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_SMALLER_THAN, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32)

#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual, line, message)                 HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)              (threshold), (HUNT_INT)              (actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT8(threshold, actual, line, message)                HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT8 ) (threshold), (HUNT_INT)(HUNT_INT8 ) (actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT16(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT16) (threshold), (HUNT_INT)(HUNT_INT16) (actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT32(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT32) (threshold), (HUNT_INT)(HUNT_INT32) (actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual, line, message)                HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)              (threshold), (HUNT_INT)              (actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT8(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT16(threshold, actual, line, message)              HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT32(threshold, actual, line, message)              HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX8(threshold, actual, line, message)                HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX16(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX32(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32)

#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT(threshold, actual, line, message)                 HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)             (threshold),  (HUNT_INT)              (actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT8(threshold, actual, line, message)                HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT8 )(threshold),  (HUNT_INT)(HUNT_INT8 ) (actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT16(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT16)(threshold),  (HUNT_INT)(HUNT_INT16) (actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT32(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_INT32)(threshold),  (HUNT_INT)(HUNT_INT32) (actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT(threshold, actual, line, message)                HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)             (threshold),  (HUNT_INT)              (actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT8(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT16(threshold, actual, line, message)              HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT32(threshold, actual, line, message)              HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX8(threshold, actual, line, message)                HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT8 )(threshold), (HUNT_INT)(HUNT_UINT8 )(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX16(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT16)(threshold), (HUNT_INT)(HUNT_UINT16)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX32(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(HUNT_UINT32)(threshold), (HUNT_INT)(HUNT_UINT32)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32)

#define HUNT_TEST_ASSERT_INT_WITHIN(delta, expected, actual, line, message)                     HuntAssertNumbersWithin(              (delta), (HUNT_INT)                          (expected), (HUNT_INT)                          (actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT)
#define HUNT_TEST_ASSERT_INT8_WITHIN(delta, expected, actual, line, message)                    HuntAssertNumbersWithin((HUNT_UINT8 )(delta), (HUNT_INT)(HUNT_INT8 )             (expected), (HUNT_INT)(HUNT_INT8 )             (actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8)
#define HUNT_TEST_ASSERT_INT16_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((HUNT_UINT16)(delta), (HUNT_INT)(HUNT_INT16)             (expected), (HUNT_INT)(HUNT_INT16)             (actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16)
#define HUNT_TEST_ASSERT_INT32_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((HUNT_UINT32)(delta), (HUNT_INT)(HUNT_INT32)             (expected), (HUNT_INT)(HUNT_INT32)             (actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32)
#define HUNT_TEST_ASSERT_UINT_WITHIN(delta, expected, actual, line, message)                    HuntAssertNumbersWithin(              (delta), (HUNT_INT)                          (expected), (HUNT_INT)                          (actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT)
#define HUNT_TEST_ASSERT_UINT8_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((HUNT_UINT8 )(delta), (HUNT_INT)(HUNT_UINT)(HUNT_UINT8 )(expected), (HUNT_INT)(HUNT_UINT)(HUNT_UINT8 )(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8)
#define HUNT_TEST_ASSERT_UINT16_WITHIN(delta, expected, actual, line, message)                  HuntAssertNumbersWithin((HUNT_UINT16)(delta), (HUNT_INT)(HUNT_UINT)(HUNT_UINT16)(expected), (HUNT_INT)(HUNT_UINT)(HUNT_UINT16)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16)
#define HUNT_TEST_ASSERT_UINT32_WITHIN(delta, expected, actual, line, message)                  HuntAssertNumbersWithin((HUNT_UINT32)(delta), (HUNT_INT)(HUNT_UINT)(HUNT_UINT32)(expected), (HUNT_INT)(HUNT_UINT)(HUNT_UINT32)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32)
#define HUNT_TEST_ASSERT_HEX8_WITHIN(delta, expected, actual, line, message)                    HuntAssertNumbersWithin((HUNT_UINT8 )(delta), (HUNT_INT)(HUNT_UINT)(HUNT_UINT8 )(expected), (HUNT_INT)(HUNT_UINT)(HUNT_UINT8 )(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8)
#define HUNT_TEST_ASSERT_HEX16_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((HUNT_UINT16)(delta), (HUNT_INT)(HUNT_UINT)(HUNT_UINT16)(expected), (HUNT_INT)(HUNT_UINT)(HUNT_UINT16)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16)
#define HUNT_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((HUNT_UINT32)(delta), (HUNT_INT)(HUNT_UINT)(HUNT_UINT32)(expected), (HUNT_INT)(HUNT_UINT)(HUNT_UINT32)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32)


#define HUNT_TEST_ASSERT_INT_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)     HuntAssertNumbersArrayWithin(              (delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_INT8_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)    HuntAssertNumbersArrayWithin((HUNT_UINT8 )(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_INT16_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HuntAssertNumbersArrayWithin((HUNT_UINT16)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_INT32_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HuntAssertNumbersArrayWithin((HUNT_UINT32)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_UINT_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)    HuntAssertNumbersArrayWithin(              (delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_UINT8_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)  HuntAssertNumbersArrayWithin( (HUNT_UINT16)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_UINT16_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)  HuntAssertNumbersArrayWithin((HUNT_UINT16)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_UINT32_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)  HuntAssertNumbersArrayWithin((HUNT_UINT32)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_HEX8_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)    HuntAssertNumbersArrayWithin((HUNT_UINT8 )(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_HEX16_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HuntAssertNumbersArrayWithin((HUNT_UINT16)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_HEX32_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HuntAssertNumbersArrayWithin((HUNT_UINT32)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), ((HUNT_UINT32)(num_elements)), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32, HUNT_ARRAY_TO_ARRAY)


#define HUNT_TEST_ASSERT_EQUAL_PTR(expected, actual, line, message)                             HuntAssertEqualNumber((HUNT_PTR_TO_INT)(expected), (HUNT_PTR_TO_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_POINTER)
#define HUNT_TEST_ASSERT_EQUAL_STRING(expected, actual, line, message)                          HuntAssertEqualString((const char*)(expected), (const char*)(actual), (message), (HUNT_LINE_TYPE)(line))
#define HUNT_TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len, line, message)                 HuntAssertEqualStringLen((const char*)(expected), (const char*)(actual), (HUNT_UINT32)(len), (message), (HUNT_LINE_TYPE)(line))
#define HUNT_TEST_ASSERT_EQUAL_MEMORY(expected, actual, len, line, message)                     HuntAssertEqualMemory((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(len), 1, (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_ARRAY)

#define HUNT_TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements, line, message)         HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT,     HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8,    HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16,   HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32,   HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT,    HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8,   HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements, line, message)      HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16,  HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements, line, message)      HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32,  HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8,    HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16,   HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32,   HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_PTR_ARRAY(expected, actual, num_elements, line, message)         HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_POINTER, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements, line, message)      HuntAssertEqualStringArray((HUNT_INTERNAL_PTR)(expected), (const char**)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements, line, message) HuntAssertEqualMemory((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(len), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_ARRAY)

#define HUNT_TEST_ASSERT_EACH_EQUAL_INT(expected, actual, num_elements, line, message)          HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)              (expected), (HUNT_INT_WIDTH / 8)),          (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT,     HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_INT8(expected, actual, num_elements, line, message)         HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT8  )(expected), 1),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT8,    HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_INT16(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT16 )(expected), 2),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT16,   HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_INT32(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT32 )(expected), 4),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32,   HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_UINT(expected, actual, num_elements, line, message)         HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)              (expected), (HUNT_INT_WIDTH / 8)),          (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT,    HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_UINT8(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_UINT8 )(expected), 1),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT8,   HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_UINT16(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_UINT16)(expected), 2),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT16,  HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_UINT32(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_UINT32)(expected), 4),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32,  HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_HEX8(expected, actual, num_elements, line, message)         HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT8  )(expected), 1),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX8,    HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_HEX16(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT16 )(expected), 2),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX16,   HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_HEX32(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT32 )(expected), 4),                              (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX32,   HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_PTR(expected, actual, num_elements, line, message)          HuntAssertEqualIntArray(HuntNumToPtr((HUNT_PTR_TO_INT)       (expected), (HUNT_POINTER_WIDTH / 8)),      (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_POINTER, HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_STRING(expected, actual, num_elements, line, message)       HuntAssertEqualStringArray((HUNT_INTERNAL_PTR)(expected), (const char**)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_MEMORY(expected, actual, len, num_elements, line, message)  HuntAssertEqualMemory((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(len), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_VAL)

#ifdef HUNT_SUPPORT_64
#define HUNT_TEST_ASSERT_EQUAL_INT64(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64)
#define HUNT_TEST_ASSERT_EQUAL_UINT64(expected, actual, line, message)                          HuntAssertEqualNumber((HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64)
#define HUNT_TEST_ASSERT_EQUAL_HEX64(expected, actual, line, message)                           HuntAssertEqualNumber((HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64)
#define HUNT_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64,  HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements, line, message)      HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray((HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64,  HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EACH_EQUAL_INT64(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT64)(expected), 8), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64,  HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_UINT64(expected, actual, num_elements, line, message)       HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_UINT64)(expected), 8), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64, HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_EACH_EQUAL_HEX64(expected, actual, num_elements, line, message)        HuntAssertEqualIntArray(HuntNumToPtr((HUNT_INT)(HUNT_INT64)(expected), 8), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64,  HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_INT64_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((delta), (HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64)
#define HUNT_TEST_ASSERT_UINT64_WITHIN(delta, expected, actual, line, message)                  HuntAssertNumbersWithin((delta), (HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64)
#define HUNT_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual, line, message)                   HuntAssertNumbersWithin((delta), (HUNT_INT)(expected), (HUNT_INT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64)
#define HUNT_TEST_ASSERT_GREATER_THAN_INT64(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_GREATER_THAN,     (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64)
#define HUNT_TEST_ASSERT_GREATER_THAN_UINT64(threshold, actual, line, message)                  HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_GREATER_THAN,     (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64)
#define HUNT_TEST_ASSERT_GREATER_THAN_HEX64(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_GREATER_THAN,     (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT64(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT64(threshold, actual, line, message)              HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX64(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_GREATER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64)
#define HUNT_TEST_ASSERT_SMALLER_THAN_INT64(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_SMALLER_THAN,     (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64)
#define HUNT_TEST_ASSERT_SMALLER_THAN_UINT64(threshold, actual, line, message)                  HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_SMALLER_THAN,     (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64)
#define HUNT_TEST_ASSERT_SMALLER_THAN_HEX64(threshold, actual, line, message)                   HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_SMALLER_THAN,     (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT64(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT64)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT64(threshold, actual, line, message)              HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT64)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX64(threshold, actual, line, message)               HuntAssertGreaterOrLessOrEqualNumber((HUNT_INT)(threshold), (HUNT_INT)(actual), HUNT_SMALLER_OR_EQUAL, (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64)
#define HUNT_TEST_ASSERT_INT64_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HuntAssertNumbersArrayWithin((HUNT_UINT32)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_INT32, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_UINT64_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)  HuntAssertNumbersArrayWithin((HUNT_UINT32)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_UINT32, HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_HEX64_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HuntAssertNumbersArrayWithin((HUNT_UINT32)(delta), (HUNT_INTERNAL_PTR)(expected), (HUNT_INTERNAL_PTR)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_DISPLAY_STYLE_HEX64, HUNT_ARRAY_TO_ARRAY)
#else
#define HUNT_TEST_ASSERT_EQUAL_INT64(expected, actual, line, message)                           HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_EQUAL_UINT64(expected, actual, line, message)                          HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_EQUAL_HEX64(expected, actual, line, message)                           HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements, line, message)       HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements, line, message)      HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements, line, message)       HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_INT64_WITHIN(delta, expected, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_UINT64_WITHIN(delta, expected, actual, line, message)                  HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_GREATER_THAN_INT64(threshold, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_GREATER_THAN_UINT64(threshold, actual, line, message)                  HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_GREATER_THAN_HEX64(threshold, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT64(threshold, actual, line, message)               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT64(threshold, actual, line, message)              HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX64(threshold, actual, line, message)               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_SMALLER_THAN_INT64(threshold, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_SMALLER_THAN_UINT64(threshold, actual, line, message)                  HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_SMALLER_THAN_HEX64(threshold, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT64(threshold, actual, line, message)               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT64(threshold, actual, line, message)              HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX64(threshold, actual, line, message)               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_INT64_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_UINT64_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)  HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#define HUNT_TEST_ASSERT_HEX64_ARRAY_WITHIN(delta, expected, actual, num_elements, line, message)   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErr64)
#endif

#ifdef HUNT_EXCLUDE_FLOAT
#define HUNT_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual, line, message)                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_EQUAL_FLOAT(expected, actual, line, message)                           HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements, line, message)       HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_EACH_EQUAL_FLOAT(expected, actual, num_elements, line, message)        HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_INF(actual, line, message)                                    HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_NEG_INF(actual, line, message)                                HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_NAN(actual, line, message)                                    HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_DETERMINATE(actual, line, message)                            HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_INF(actual, line, message)                                HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_NEG_INF(actual, line, message)                            HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_NAN(actual, line, message)                                HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE(actual, line, message)                        HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrFloat)
#else
#define HUNT_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual, line, message)                   HuntAssertFloatsWithin((HUNT_FLOAT)(delta), (HUNT_FLOAT)(expected), (HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line))
#define HUNT_TEST_ASSERT_EQUAL_FLOAT(expected, actual, line, message)                           HUNT_TEST_ASSERT_FLOAT_WITHIN((HUNT_FLOAT)(expected) * (HUNT_FLOAT)HUNT_FLOAT_PRECISION, (HUNT_FLOAT)(expected), (HUNT_FLOAT)(actual), (HUNT_LINE_TYPE)(line), (message))
#define HUNT_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements, line, message)       HuntAssertEqualFloatArray((HUNT_FLOAT*)(expected), (HUNT_FLOAT*)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EACH_EQUAL_FLOAT(expected, actual, num_elements, line, message)        HuntAssertEqualFloatArray(HuntFloatToPtr(expected), (HUNT_FLOAT*)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_FLOAT_IS_INF(actual, line, message)                                    HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_INF)
#define HUNT_TEST_ASSERT_FLOAT_IS_NEG_INF(actual, line, message)                                HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NEG_INF)
#define HUNT_TEST_ASSERT_FLOAT_IS_NAN(actual, line, message)                                    HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NAN)
#define HUNT_TEST_ASSERT_FLOAT_IS_DETERMINATE(actual, line, message)                            HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_DET)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_INF(actual, line, message)                                HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_INF)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_NEG_INF(actual, line, message)                            HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_NEG_INF)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_NAN(actual, line, message)                                HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_NAN)
#define HUNT_TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE(actual, line, message)                        HuntAssertFloatSpecial((HUNT_FLOAT)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_DET)
#endif

#ifdef HUNT_EXCLUDE_DOUBLE
#define HUNT_TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual, line, message)                  HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_EQUAL_DOUBLE(expected, actual, line, message)                          HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements, line, message)      HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_EACH_EQUAL_DOUBLE(expected, actual, num_elements, line, message)       HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_INF(actual, line, message)                                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NEG_INF(actual, line, message)                               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NAN(actual, line, message)                                   HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_DETERMINATE(actual, line, message)                           HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_INF(actual, line, message)                               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NEG_INF(actual, line, message)                           HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NAN(actual, line, message)                               HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_DETERMINATE(actual, line, message)                       HUNT_TEST_FAIL((HUNT_LINE_TYPE)(line), HuntStrErrDouble)
#else
#define HUNT_TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual, line, message)                  HuntAssertDoublesWithin((HUNT_DOUBLE)(delta), (HUNT_DOUBLE)(expected), (HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line))
#define HUNT_TEST_ASSERT_EQUAL_DOUBLE(expected, actual, line, message)                          HUNT_TEST_ASSERT_DOUBLE_WITHIN((HUNT_DOUBLE)(expected) * (HUNT_DOUBLE)HUNT_DOUBLE_PRECISION, (HUNT_DOUBLE)(expected), (HUNT_DOUBLE)(actual), (HUNT_LINE_TYPE)(line), (message))
#define HUNT_TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements, line, message)      HuntAssertEqualDoubleArray((HUNT_DOUBLE*)(expected), (HUNT_DOUBLE*)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_ARRAY)
#define HUNT_TEST_ASSERT_EACH_EQUAL_DOUBLE(expected, actual, num_elements, line, message)       HuntAssertEqualDoubleArray(HuntDoubleToPtr(expected), (HUNT_DOUBLE*)(actual), (HUNT_UINT32)(num_elements), (message), (HUNT_LINE_TYPE)(line), HUNT_ARRAY_TO_VAL)
#define HUNT_TEST_ASSERT_DOUBLE_IS_INF(actual, line, message)                                   HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_INF)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NEG_INF(actual, line, message)                               HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NEG_INF)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NAN(actual, line, message)                                   HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NAN)
#define HUNT_TEST_ASSERT_DOUBLE_IS_DETERMINATE(actual, line, message)                           HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_DET)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_INF(actual, line, message)                               HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_INF)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NEG_INF(actual, line, message)                           HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_NEG_INF)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NAN(actual, line, message)                               HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_NAN)
#define HUNT_TEST_ASSERT_DOUBLE_IS_NOT_DETERMINATE(actual, line, message)                       HuntAssertDoubleSpecial((HUNT_DOUBLE)(actual), (message), (HUNT_LINE_TYPE)(line), HUNT_FLOAT_IS_NOT_DET)
#endif

/* End of HUNT_INTERNALS_H */
#endif
