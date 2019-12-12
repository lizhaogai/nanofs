/* ==========================================
    Hunt Project - A Test Framework for C
    Copyright (c) 2007-19 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#ifndef HUNT_FRAMEWORK_H
#define HUNT_FRAMEWORK_H
#define HUNT

#define HUNT_VERSION_MAJOR    2
#define HUNT_VERSION_MINOR    5
#define HUNT_VERSION_BUILD    0
#define HUNT_VERSION          ((HUNT_VERSION_MAJOR << 16) | (HUNT_VERSION_MINOR << 8) | HUNT_VERSION_BUILD)

#ifdef __cplusplus
extern "C"
{
#endif

#include "hunt_internals.h"

/*-------------------------------------------------------
 * Test Setup / Teardown
 *-------------------------------------------------------*/

/* These functions are intended to be called before and after each test. */
void setUp(void);

void tearDown(void);

/* These functions are intended to be called at the beginning and end of an
 * entire test suite.  suiteTearDown() is passed the number of tests that
 * failed, and its return value becomes the exit code of main(). */
void suiteSetUp(void);

int suiteTearDown(int num_failures);

/* If the compiler supports it, the following block provides stub
 * implementations of the above functions as weak symbols.  Note that on
 * some platforms (MinGW for example), weak function implementations need
 * to be in the same translation unit they are called from.  This can be
 * achieved by defining HUNT_INCLUDE_SETUP_STUBS before including hunt.h. */
#ifdef HUNT_INCLUDE_SETUP_STUBS
#ifdef HUNT_WEAK_ATTRIBUTE
HUNT_WEAK_ATTRIBUTE void setUp(void) { }
HUNT_WEAK_ATTRIBUTE void tearDown(void) { }
HUNT_WEAK_ATTRIBUTE void suiteSetUp(void) { }
HUNT_WEAK_ATTRIBUTE int suiteTearDown(int num_failures) { return num_failures; }
#elif defined(HUNT_WEAK_PRAGMA)
#pragma weak setUp
void setUp(void) { }
#pragma weak tearDown
void tearDown(void) { }
#pragma weak suiteSetUp
void suiteSetUp(void) { }
#pragma weak suiteTearDown
int suiteTearDown(int num_failures) { return num_failures; }
#endif
#endif

/*-------------------------------------------------------
 * Configuration Options
 *-------------------------------------------------------
 * All options described below should be passed as a compiler flag to all files using Hunt. If you must add #defines, place them BEFORE the #include above.

 * Integers/longs/pointers
 *     - Hunt attempts to automatically discover your integer sizes
 *       - define HUNT_EXCLUDE_STDINT_H to stop attempting to look in <stdint.h>
 *       - define HUNT_EXCLUDE_LIMITS_H to stop attempting to look in <limits.h>
 *     - If you cannot use the automatic methods above, you can force Hunt by using these options:
 *       - define HUNT_SUPPORT_64
 *       - set HUNT_INT_WIDTH
 *       - set HUNT_LONG_WIDTH
 *       - set HUNT_POINTER_WIDTH

 * Floats
 *     - define HUNT_EXCLUDE_FLOAT to disallow floating point comparisons
 *     - define HUNT_FLOAT_PRECISION to specify the precision to use when doing TEST_ASSERT_EQUAL_FLOAT
 *     - define HUNT_FLOAT_TYPE to specify doubles instead of single precision floats
 *     - define HUNT_INCLUDE_DOUBLE to allow double floating point comparisons
 *     - define HUNT_EXCLUDE_DOUBLE to disallow double floating point comparisons (default)
 *     - define HUNT_DOUBLE_PRECISION to specify the precision to use when doing TEST_ASSERT_EQUAL_DOUBLE
 *     - define HUNT_DOUBLE_TYPE to specify something other than double
 *     - define HUNT_EXCLUDE_FLOAT_PRINT to trim binary size, won't print floating point values in errors

 * Output
 *     - by default, Hunt prints to standard out with putchar.  define HUNT_OUTPUT_CHAR(a) with a different function if desired
 *     - define HUNT_DIFFERENTIATE_FINAL_FAIL to print FAILED (vs. FAIL) at test end summary - for automated search for failure

 * Optimization
 *     - by default, line numbers are stored in unsigned shorts.  Define HUNT_LINE_TYPE with a different type if your files are huge
 *     - by default, test and failure counters are unsigned shorts.  Define HUNT_COUNTER_TYPE with a different type if you want to save space or have more than 65535 Tests.

 * Test Cases
 *     - define HUNT_SUPPORT_TEST_CASES to include the TEST_CASE macro, though really it's mostly about the runner generator script

 * Parameterized Tests
 *     - you'll want to create a define of TEST_CASE(...) which basically evaluates to nothing

 * Tests with Arguments
 *     - you'll want to define HUNT_USE_COMMAND_LINE_ARGS if you have the test runner passing arguments to Hunt

 *-------------------------------------------------------
 * Basic Fail and Ignore
 *-------------------------------------------------------*/

#define TEST_FAIL_MESSAGE(message)                                                                 HUNT_TEST_FAIL(__LINE__, (message))
#define TEST_FAIL()                                                                                HUNT_TEST_FAIL(__LINE__, NULL)
#define TEST_IGNORE_MESSAGE(message)                                                               HUNT_TEST_IGNORE(__LINE__, (message))
#define TEST_IGNORE()                                                                              HUNT_TEST_IGNORE(__LINE__, NULL)
#define TEST_MESSAGE(message)                                                                      HuntMessage((message), __LINE__)
#define TEST_ONLY()

/* It is not necessary for you to call PASS. A PASS condition is assumed if nothing fails.
 * This method allows you to abort a test immediately with a PASS state, ignoring the remainder of the test. */
#define TEST_PASS()                                                                                TEST_ABORT()
#define TEST_PASS_MESSAGE(message)                                                                 do { HuntMessage((message), __LINE__); TEST_ABORT(); } while(0)

/* This macro does nothing, but it is useful for build tools (like Ceedling) to make use of this to figure out
 * which files should be linked to in order to perform a test. Use it like TEST_FILE("sandwiches.c") */
#define TEST_FILE(a)

/*-------------------------------------------------------
 * Test Asserts (simple)
 *-------------------------------------------------------*/

/* Boolean */
#define TEST_ASSERT(condition)                                                                     HUNT_TEST_ASSERT(       (condition), __LINE__, " Expression Evaluated To FALSE")
#define TEST_ASSERT_TRUE(condition)                                                                HUNT_TEST_ASSERT(       (condition), __LINE__, " Expected TRUE Was FALSE")
#define TEST_ASSERT_UNLESS(condition)                                                              HUNT_TEST_ASSERT(      !(condition), __LINE__, " Expression Evaluated To TRUE")
#define TEST_ASSERT_FALSE(condition)                                                               HUNT_TEST_ASSERT(      !(condition), __LINE__, " Expected FALSE Was TRUE")
#define TEST_ASSERT_NULL(pointer)                                                                  HUNT_TEST_ASSERT_NULL(    (pointer), __LINE__, " Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer)                                                              HUNT_TEST_ASSERT_NOT_NULL((pointer), __LINE__, " Expected Non-NULL")

/* Integers (of all sizes) */
#define TEST_ASSERT_EQUAL_INT(expected, actual)                                                    HUNT_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT8(expected, actual)                                                   HUNT_TEST_ASSERT_EQUAL_INT8((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT16(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_INT16((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT32(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_INT32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT64(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_INT64((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT(expected, actual)                                                   HUNT_TEST_ASSERT_EQUAL_UINT( (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT8(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_UINT8( (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT16(expected, actual)                                                 HUNT_TEST_ASSERT_EQUAL_UINT16( (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT32(expected, actual)                                                 HUNT_TEST_ASSERT_EQUAL_UINT32( (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT64(expected, actual)                                                 HUNT_TEST_ASSERT_EQUAL_UINT64( (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_size_t(expected, actual)                                                 HUNT_TEST_ASSERT_EQUAL_UINT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX(expected, actual)                                                    HUNT_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX8(expected, actual)                                                   HUNT_TEST_ASSERT_EQUAL_HEX8( (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX16(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_HEX16((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX32(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX64(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_HEX64((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_BITS(mask, expected, actual)                                                   HUNT_TEST_ASSERT_BITS((mask), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_BITS_HIGH(mask, actual)                                                        HUNT_TEST_ASSERT_BITS((mask), (HUNT_UINT32)(-1), (actual), __LINE__, NULL)
#define TEST_ASSERT_BITS_LOW(mask, actual)                                                         HUNT_TEST_ASSERT_BITS((mask), (HUNT_UINT32)(0), (actual), __LINE__, NULL)
#define TEST_ASSERT_BIT_HIGH(bit, actual)                                                          HUNT_TEST_ASSERT_BITS(((HUNT_UINT32)1 << (bit)), (HUNT_UINT32)(-1), (actual), __LINE__, NULL)
#define TEST_ASSERT_BIT_LOW(bit, actual)                                                           HUNT_TEST_ASSERT_BITS(((HUNT_UINT32)1 << (bit)), (HUNT_UINT32)(0), (actual), __LINE__, NULL)

/* Integer Greater Than/ Less Than (of all sizes) */
#define TEST_ASSERT_GREATER_THAN(threshold, actual)                                                HUNT_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT(threshold, actual)                                            HUNT_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT8(threshold, actual)                                           HUNT_TEST_ASSERT_GREATER_THAN_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT16(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT32(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT64(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT(threshold, actual)                                           HUNT_TEST_ASSERT_GREATER_THAN_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT8(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT16(threshold, actual)                                         HUNT_TEST_ASSERT_GREATER_THAN_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT32(threshold, actual)                                         HUNT_TEST_ASSERT_GREATER_THAN_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT64(threshold, actual)                                         HUNT_TEST_ASSERT_GREATER_THAN_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_size_t(threshold, actual)                                         HUNT_TEST_ASSERT_GREATER_THAN_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX8(threshold, actual)                                           HUNT_TEST_ASSERT_GREATER_THAN_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX16(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX32(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX64(threshold, actual)                                          HUNT_TEST_ASSERT_GREATER_THAN_HEX64((threshold), (actual), __LINE__, NULL)

#define TEST_ASSERT_LESS_THAN(threshold, actual)                                                   HUNT_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT(threshold, actual)                                               HUNT_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT8(threshold, actual)                                              HUNT_TEST_ASSERT_SMALLER_THAN_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT16(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT32(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT64(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT(threshold, actual)                                              HUNT_TEST_ASSERT_SMALLER_THAN_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT8(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT16(threshold, actual)                                            HUNT_TEST_ASSERT_SMALLER_THAN_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT32(threshold, actual)                                            HUNT_TEST_ASSERT_SMALLER_THAN_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT64(threshold, actual)                                            HUNT_TEST_ASSERT_SMALLER_THAN_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_size_t(threshold, actual)                                            HUNT_TEST_ASSERT_SMALLER_THAN_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX8(threshold, actual)                                              HUNT_TEST_ASSERT_SMALLER_THAN_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX16(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX32(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX64(threshold, actual)                                             HUNT_TEST_ASSERT_SMALLER_THAN_HEX64((threshold), (actual), __LINE__, NULL)

#define TEST_ASSERT_GREATER_OR_EQUAL(threshold, actual)                                            HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual)                                        HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT8(threshold, actual)                                       HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT16(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT32(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT64(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual)                                       HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT8(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT16(threshold, actual)                                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT32(threshold, actual)                                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT64(threshold, actual)                                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_size_t(threshold, actual)                                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX8(threshold, actual)                                       HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX16(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX32(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX64(threshold, actual)                                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, NULL)

#define TEST_ASSERT_LESS_OR_EQUAL(threshold, actual)                                               HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT(threshold, actual)                                           HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT8(threshold, actual)                                          HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT16(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT32(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT64(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT(threshold, actual)                                          HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT8(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT16(threshold, actual)                                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT32(threshold, actual)                                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT64(threshold, actual)                                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_size_t(threshold, actual)                                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX8(threshold, actual)                                          HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX16(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX32(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX64(threshold, actual)                                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, NULL)

/* Integer Ranges (of all sizes) */
#define TEST_ASSERT_INT_WITHIN(delta, expected, actual)                                            HUNT_TEST_ASSERT_INT_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT8_WITHIN(delta, expected, actual)                                           HUNT_TEST_ASSERT_INT8_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT16_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_INT16_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT32_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_INT32_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT64_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_INT64_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT_WITHIN(delta, expected, actual)                                           HUNT_TEST_ASSERT_UINT_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT8_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_UINT8_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT16_WITHIN(delta, expected, actual)                                         HUNT_TEST_ASSERT_UINT16_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT32_WITHIN(delta, expected, actual)                                         HUNT_TEST_ASSERT_UINT32_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT64_WITHIN(delta, expected, actual)                                         HUNT_TEST_ASSERT_UINT64_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_size_t_WITHIN(delta, expected, actual)                                         HUNT_TEST_ASSERT_UINT_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_HEX_WITHIN(delta, expected, actual)                                            HUNT_TEST_ASSERT_HEX32_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_HEX8_WITHIN(delta, expected, actual)                                           HUNT_TEST_ASSERT_HEX8_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_HEX16_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_HEX16_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_HEX32_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_HEX32_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_HEX64_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_HEX64_WITHIN((delta), (expected), (actual), __LINE__, NULL)

/* Integer Array Ranges (of all sizes) */
#define TEST_ASSERT_INT_ARRAY_WITHIN(delta, expected, actual, num_elements)                        HUNT_TEST_ASSERT_INT_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_INT8_ARRAY_WITHIN(delta, expected, actual, num_elements)                       HUNT_TEST_ASSERT_INT8_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_INT16_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_INT16_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_INT32_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_INT32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_INT64_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_INT64_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_UINT_ARRAY_WITHIN(delta, expected, actual, num_elements)                       HUNT_TEST_ASSERT_UINT_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_UINT8_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_UINT8_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_UINT16_ARRAY_WITHIN(delta, expected, actual, num_elements)                     HUNT_TEST_ASSERT_UINT16_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_UINT32_ARRAY_WITHIN(delta, expected, actual, num_elements)                     HUNT_TEST_ASSERT_UINT32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_UINT64_ARRAY_WITHIN(delta, expected, actual, num_elements)                     HUNT_TEST_ASSERT_UINT64_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_size_t_ARRAY_WITHIN(delta, expected, actual, num_elements)                     HUNT_TEST_ASSERT_UINT_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_HEX_ARRAY_WITHIN(delta, expected, actual, num_elements)                        HUNT_TEST_ASSERT_HEX32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_HEX8_ARRAY_WITHIN(delta, expected, actual, num_elements)                       HUNT_TEST_ASSERT_HEX8_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_HEX16_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_HEX16_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_HEX32_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_HEX32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)
#define TEST_ASSERT_HEX64_ARRAY_WITHIN(delta, expected, actual, num_elements)                      HUNT_TEST_ASSERT_HEX64_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, NULL)


/* Structs and Strings */
#define TEST_ASSERT_EQUAL_PTR(expected, actual)                                                    HUNT_TEST_ASSERT_EQUAL_PTR((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING(expected, actual)                                                 HUNT_TEST_ASSERT_EQUAL_STRING((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)                                        HUNT_TEST_ASSERT_EQUAL_STRING_LEN((expected), (actual), (len), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)                                            HUNT_TEST_ASSERT_EQUAL_MEMORY((expected), (actual), (len), __LINE__, NULL)

/* Arrays */
#define TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)                                HUNT_TEST_ASSERT_EQUAL_INT_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EQUAL_INT8_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_INT16_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_INT32_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_INT64_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EQUAL_UINT_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_UINT8_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements)                             HUNT_TEST_ASSERT_EQUAL_UINT16_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements)                             HUNT_TEST_ASSERT_EQUAL_UINT32_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements)                             HUNT_TEST_ASSERT_EQUAL_UINT64_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_size_t_ARRAY(expected, actual, num_elements)                             HUNT_TEST_ASSERT_EQUAL_UINT_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX_ARRAY(expected, actual, num_elements)                                HUNT_TEST_ASSERT_EQUAL_HEX32_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EQUAL_HEX8_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_HEX16_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_HEX32_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_HEX64_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_PTR_ARRAY(expected, actual, num_elements)                                HUNT_TEST_ASSERT_EQUAL_PTR_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)                             HUNT_TEST_ASSERT_EQUAL_STRING_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)                        HUNT_TEST_ASSERT_EQUAL_MEMORY_ARRAY((expected), (actual), (len), (num_elements), __LINE__, NULL)

/* Arrays Compared To Single Value */
#define TEST_ASSERT_EACH_EQUAL_INT(expected, actual, num_elements)                                 HUNT_TEST_ASSERT_EACH_EQUAL_INT((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT8(expected, actual, num_elements)                                HUNT_TEST_ASSERT_EACH_EQUAL_INT8((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT16(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_INT16((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT32(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_INT32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT64(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_INT64((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT(expected, actual, num_elements)                                HUNT_TEST_ASSERT_EACH_EQUAL_UINT((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT8(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_UINT8((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT16(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EACH_EQUAL_UINT16((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT32(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EACH_EQUAL_UINT32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT64(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EACH_EQUAL_UINT64((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_size_t(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EACH_EQUAL_UINT((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX(expected, actual, num_elements)                                 HUNT_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX8(expected, actual, num_elements)                                HUNT_TEST_ASSERT_EACH_EQUAL_HEX8((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX16(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_HEX16((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX32(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX64(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_HEX64((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_PTR(expected, actual, num_elements)                                 HUNT_TEST_ASSERT_EACH_EQUAL_PTR((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_STRING(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EACH_EQUAL_STRING((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_MEMORY(expected, actual, len, num_elements)                         HUNT_TEST_ASSERT_EACH_EQUAL_MEMORY((expected), (actual), (len), (num_elements), __LINE__, NULL)

/* Floating Point (If Enabled) */
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)                                          HUNT_TEST_ASSERT_FLOAT_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual)                                                  HUNT_TEST_ASSERT_EQUAL_FLOAT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EQUAL_FLOAT_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_FLOAT(expected, actual, num_elements)                               HUNT_TEST_ASSERT_EACH_EQUAL_FLOAT((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_INF(actual)                                                           HUNT_TEST_ASSERT_FLOAT_IS_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_NEG_INF(actual)                                                       HUNT_TEST_ASSERT_FLOAT_IS_NEG_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_NAN(actual)                                                           HUNT_TEST_ASSERT_FLOAT_IS_NAN((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_DETERMINATE(actual)                                                   HUNT_TEST_ASSERT_FLOAT_IS_DETERMINATE((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_NOT_INF(actual)                                                       HUNT_TEST_ASSERT_FLOAT_IS_NOT_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_NOT_NEG_INF(actual)                                                   HUNT_TEST_ASSERT_FLOAT_IS_NOT_NEG_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_NOT_NAN(actual)                                                       HUNT_TEST_ASSERT_FLOAT_IS_NOT_NAN((actual), __LINE__, NULL)
#define TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE(actual)                                               HUNT_TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE((actual), __LINE__, NULL)

/* Double (If Enabled) */
#define TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual)                                         HUNT_TEST_ASSERT_DOUBLE_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual)                                                 HUNT_TEST_ASSERT_EQUAL_DOUBLE((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)                             HUNT_TEST_ASSERT_EQUAL_DOUBLE_ARRAY((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_DOUBLE(expected, actual, num_elements)                              HUNT_TEST_ASSERT_EACH_EQUAL_DOUBLE((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_INF(actual)                                                          HUNT_TEST_ASSERT_DOUBLE_IS_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_NEG_INF(actual)                                                      HUNT_TEST_ASSERT_DOUBLE_IS_NEG_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_NAN(actual)                                                          HUNT_TEST_ASSERT_DOUBLE_IS_NAN((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_DETERMINATE(actual)                                                  HUNT_TEST_ASSERT_DOUBLE_IS_DETERMINATE((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_NOT_INF(actual)                                                      HUNT_TEST_ASSERT_DOUBLE_IS_NOT_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_NOT_NEG_INF(actual)                                                  HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NEG_INF((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_NOT_NAN(actual)                                                      HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NAN((actual), __LINE__, NULL)
#define TEST_ASSERT_DOUBLE_IS_NOT_DETERMINATE(actual)                                              HUNT_TEST_ASSERT_DOUBLE_IS_NOT_DETERMINATE((actual), __LINE__, NULL)

/* Shorthand */
#ifdef HUNT_SHORTHAND_AS_OLD
#define TEST_ASSERT_EQUAL(expected, actual)                                                        HUNT_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL(expected, actual)                                                    HUNT_TEST_ASSERT(((expected) != (actual)), __LINE__, " Expected Not-Equal")
#endif
#ifdef HUNT_SHORTHAND_AS_INT
#define TEST_ASSERT_EQUAL(expected, actual)                                                        HUNT_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL(expected, actual)                                                    HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#endif
#ifdef HUNT_SHORTHAND_AS_MEM
#define TEST_ASSERT_EQUAL(expected, actual)                                                        HUNT_TEST_ASSERT_EQUAL_MEMORY((&expected), (&actual), sizeof(expected), __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL(expected, actual)                                                    HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#endif
#ifdef HUNT_SHORTHAND_AS_RAW
#define TEST_ASSERT_EQUAL(expected, actual)                                                        HUNT_TEST_ASSERT(((expected) == (actual)), __LINE__, " Expected Equal")
#define TEST_ASSERT_NOT_EQUAL(expected, actual)                                                    HUNT_TEST_ASSERT(((expected) != (actual)), __LINE__, " Expected Not-Equal")
#endif
#ifdef HUNT_SHORTHAND_AS_NONE
#define TEST_ASSERT_EQUAL(expected, actual)                                                        HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#define TEST_ASSERT_NOT_EQUAL(expected, actual)                                                    HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#endif

/*-------------------------------------------------------
 * Test Asserts (with additional messages)
 *-------------------------------------------------------*/

/* Boolean */
#define TEST_ASSERT_MESSAGE(condition, message)                                                    HUNT_TEST_ASSERT(       (condition), __LINE__, (message))
#define TEST_ASSERT_TRUE_MESSAGE(condition, message)                                               HUNT_TEST_ASSERT(       (condition), __LINE__, (message))
#define TEST_ASSERT_UNLESS_MESSAGE(condition, message)                                             HUNT_TEST_ASSERT(      !(condition), __LINE__, (message))
#define TEST_ASSERT_FALSE_MESSAGE(condition, message)                                              HUNT_TEST_ASSERT(      !(condition), __LINE__, (message))
#define TEST_ASSERT_NULL_MESSAGE(pointer, message)                                                 HUNT_TEST_ASSERT_NULL(    (pointer), __LINE__, (message))
#define TEST_ASSERT_NOT_NULL_MESSAGE(pointer, message)                                             HUNT_TEST_ASSERT_NOT_NULL((pointer), __LINE__, (message))

/* Integers (of all sizes) */
#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)                                   HUNT_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, message)                                  HUNT_TEST_ASSERT_EQUAL_INT8((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_INT16((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_INT32((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_INT64((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, message)                                  HUNT_TEST_ASSERT_EQUAL_UINT( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_UINT8( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, message)                                HUNT_TEST_ASSERT_EQUAL_UINT16( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, message)                                HUNT_TEST_ASSERT_EQUAL_UINT32( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, message)                                HUNT_TEST_ASSERT_EQUAL_UINT64( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_size_t_MESSAGE(expected, actual, message)                                HUNT_TEST_ASSERT_EQUAL_UINT( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX_MESSAGE(expected, actual, message)                                   HUNT_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, message)                                  HUNT_TEST_ASSERT_EQUAL_HEX8( (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_HEX16((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_HEX64((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_BITS_MESSAGE(mask, expected, actual, message)                                  HUNT_TEST_ASSERT_BITS((mask), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_BITS_HIGH_MESSAGE(mask, actual, message)                                       HUNT_TEST_ASSERT_BITS((mask), (HUNT_UINT32)(-1), (actual), __LINE__, (message))
#define TEST_ASSERT_BITS_LOW_MESSAGE(mask, actual, message)                                        HUNT_TEST_ASSERT_BITS((mask), (HUNT_UINT32)(0), (actual), __LINE__, (message))
#define TEST_ASSERT_BIT_HIGH_MESSAGE(bit, actual, message)                                         HUNT_TEST_ASSERT_BITS(((HUNT_UINT32)1 << (bit)), (HUNT_UINT32)(-1), (actual), __LINE__, (message))
#define TEST_ASSERT_BIT_LOW_MESSAGE(bit, actual, message)                                          HUNT_TEST_ASSERT_BITS(((HUNT_UINT32)1 << (bit)), (HUNT_UINT32)(0), (actual), __LINE__, (message))

/* Integer Greater Than/ Less Than (of all sizes) */
#define TEST_ASSERT_GREATER_THAN_MESSAGE(threshold, actual, message)                               HUNT_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT_MESSAGE(threshold, actual, message)                           HUNT_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT8_MESSAGE(threshold, actual, message)                          HUNT_TEST_ASSERT_GREATER_THAN_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT16_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT64_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(threshold, actual, message)                          HUNT_TEST_ASSERT_GREATER_THAN_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT8_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT16_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_GREATER_THAN_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_GREATER_THAN_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT64_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_GREATER_THAN_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_GREATER_THAN_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX8_MESSAGE(threshold, actual, message)                          HUNT_TEST_ASSERT_GREATER_THAN_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX16_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX32_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX64_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_GREATER_THAN_HEX64((threshold), (actual), __LINE__, (message))

#define TEST_ASSERT_LESS_THAN_MESSAGE(threshold, actual, message)                                  HUNT_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT_MESSAGE(threshold, actual, message)                              HUNT_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT8_MESSAGE(threshold, actual, message)                             HUNT_TEST_ASSERT_SMALLER_THAN_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT16_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT32_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT64_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT_MESSAGE(threshold, actual, message)                             HUNT_TEST_ASSERT_SMALLER_THAN_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT8_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT16_MESSAGE(threshold, actual, message)                           HUNT_TEST_ASSERT_SMALLER_THAN_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT32_MESSAGE(threshold, actual, message)                           HUNT_TEST_ASSERT_SMALLER_THAN_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT64_MESSAGE(threshold, actual, message)                           HUNT_TEST_ASSERT_SMALLER_THAN_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_size_t_MESSAGE(threshold, actual, message)                           HUNT_TEST_ASSERT_SMALLER_THAN_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX8_MESSAGE(threshold, actual, message)                             HUNT_TEST_ASSERT_SMALLER_THAN_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX16_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX32_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX64_MESSAGE(threshold, actual, message)                            HUNT_TEST_ASSERT_SMALLER_THAN_HEX64((threshold), (actual), __LINE__, (message))

#define TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(threshold, actual, message)                           HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(threshold, actual, message)                       HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT8_MESSAGE(threshold, actual, message)                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT32_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT64_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(threshold, actual, message)                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT8_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT16_MESSAGE(threshold, actual, message)                    HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT32_MESSAGE(threshold, actual, message)                    HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(threshold, actual, message)                    HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_size_t_MESSAGE(threshold, actual, message)                    HUNT_TEST_ASSERT_GREATER_OR_EQUAL_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX8_MESSAGE(threshold, actual, message)                      HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX16_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX32_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX64_MESSAGE(threshold, actual, message)                     HUNT_TEST_ASSERT_GREATER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, (message))

#define TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(threshold, actual, message)                              HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(threshold, actual, message)                          HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT8_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT32_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT64_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT8_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT16_MESSAGE(threshold, actual, message)                       HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT32_MESSAGE(threshold, actual, message)                       HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT64_MESSAGE(threshold, actual, message)                       HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_size_t_MESSAGE(threshold, actual, message)                       HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX8_MESSAGE(threshold, actual, message)                         HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX16_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX32_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX64_MESSAGE(threshold, actual, message)                        HUNT_TEST_ASSERT_SMALLER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, (message))

/* Integer Ranges (of all sizes) */
#define TEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, message)                           HUNT_TEST_ASSERT_INT_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_INT8_WITHIN_MESSAGE(delta, expected, actual, message)                          HUNT_TEST_ASSERT_INT8_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_INT16_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_INT16_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_INT32_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_INT32_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_INT64_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_INT64_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, message)                          HUNT_TEST_ASSERT_UINT_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_UINT8_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_UINT8_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_UINT16_WITHIN_MESSAGE(delta, expected, actual, message)                        HUNT_TEST_ASSERT_UINT16_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_UINT32_WITHIN_MESSAGE(delta, expected, actual, message)                        HUNT_TEST_ASSERT_UINT32_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_UINT64_WITHIN_MESSAGE(delta, expected, actual, message)                        HUNT_TEST_ASSERT_UINT64_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_size_t_WITHIN_MESSAGE(delta, expected, actual, message)                        HUNT_TEST_ASSERT_UINT_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_HEX_WITHIN_MESSAGE(delta, expected, actual, message)                           HUNT_TEST_ASSERT_HEX32_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, message)                          HUNT_TEST_ASSERT_HEX8_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_HEX16_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_HEX32_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_HEX64_WITHIN((delta), (expected), (actual), __LINE__, (message))

/* Integer Array Ranges (of all sizes) */
#define TEST_ASSERT_INT_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)       HUNT_TEST_ASSERT_INT_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_INT8_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)      HUNT_TEST_ASSERT_INT8_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_INT16_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_INT16_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_INT32_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_INT32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_INT64_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_INT64_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_UINT_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)      HUNT_TEST_ASSERT_UINT_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_UINT8_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_UINT8_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_UINT16_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)    HUNT_TEST_ASSERT_UINT16_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_UINT32_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)    HUNT_TEST_ASSERT_UINT32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_UINT64_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)    HUNT_TEST_ASSERT_UINT64_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_size_t_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)    HUNT_TEST_ASSERT_UINT_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_HEX_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)       HUNT_TEST_ASSERT_HEX32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_HEX8_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)      HUNT_TEST_ASSERT_HEX8_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_HEX16_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_HEX16_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_HEX32_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_HEX32_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))
#define TEST_ASSERT_HEX64_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num_elements, message)     HUNT_TEST_ASSERT_HEX64_ARRAY_WITHIN((delta), (expected), (actual), num_elements, __LINE__, (message))


/* Structs and Strings */
#define TEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, message)                                   HUNT_TEST_ASSERT_EQUAL_PTR((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message)                                HUNT_TEST_ASSERT_EQUAL_STRING((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, message)                       HUNT_TEST_ASSERT_EQUAL_STRING_LEN((expected), (actual), (len), __LINE__, (message))
#define TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, message)                           HUNT_TEST_ASSERT_EQUAL_MEMORY((expected), (actual), (len), __LINE__, (message))

/* Arrays */
#define TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, message)               HUNT_TEST_ASSERT_EQUAL_INT_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EQUAL_INT8_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_INT16_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_INT32_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_INT64_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EQUAL_UINT_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_UINT8_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, num_elements, message)            HUNT_TEST_ASSERT_EQUAL_UINT16_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, num_elements, message)            HUNT_TEST_ASSERT_EQUAL_UINT32_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, num_elements, message)            HUNT_TEST_ASSERT_EQUAL_UINT64_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_size_t_ARRAY_MESSAGE(expected, actual, num_elements, message)            HUNT_TEST_ASSERT_EQUAL_UINT_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX_ARRAY_MESSAGE(expected, actual, num_elements, message)               HUNT_TEST_ASSERT_EQUAL_HEX32_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EQUAL_HEX8_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_HEX16_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_HEX32_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_HEX64_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_PTR_ARRAY_MESSAGE(expected, actual, num_elements, message)               HUNT_TEST_ASSERT_EQUAL_PTR_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, message)            HUNT_TEST_ASSERT_EQUAL_STRING_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, message)       HUNT_TEST_ASSERT_EQUAL_MEMORY_ARRAY((expected), (actual), (len), (num_elements), __LINE__, (message))

/* Arrays Compared To Single Value*/
#define TEST_ASSERT_EACH_EQUAL_INT_MESSAGE(expected, actual, num_elements, message)                HUNT_TEST_ASSERT_EACH_EQUAL_INT((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT8_MESSAGE(expected, actual, num_elements, message)               HUNT_TEST_ASSERT_EACH_EQUAL_INT8((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT16_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_INT16((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT32_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_INT32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT64_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_INT64((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT_MESSAGE(expected, actual, num_elements, message)               HUNT_TEST_ASSERT_EACH_EQUAL_UINT((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_UINT8((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT16_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EACH_EQUAL_UINT16((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT32_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EACH_EQUAL_UINT32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT64_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EACH_EQUAL_UINT64((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_size_t_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EACH_EQUAL_UINT((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX_MESSAGE(expected, actual, num_elements, message)                HUNT_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX8_MESSAGE(expected, actual, num_elements, message)               HUNT_TEST_ASSERT_EACH_EQUAL_HEX8((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX16_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_HEX16((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX32_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX64_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_HEX64((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_PTR_MESSAGE(expected, actual, num_elements, message)                HUNT_TEST_ASSERT_EACH_EQUAL_PTR((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_STRING_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EACH_EQUAL_STRING((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_MEMORY_MESSAGE(expected, actual, len, num_elements, message)        HUNT_TEST_ASSERT_EACH_EQUAL_MEMORY((expected), (actual), (len), (num_elements), __LINE__, (message))

/* Floating Point (If Enabled) */
#define TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, message)                         HUNT_TEST_ASSERT_FLOAT_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message)                                 HUNT_TEST_ASSERT_EQUAL_FLOAT((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EQUAL_FLOAT_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_FLOAT_MESSAGE(expected, actual, num_elements, message)              HUNT_TEST_ASSERT_EACH_EQUAL_FLOAT((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_INF_MESSAGE(actual, message)                                          HUNT_TEST_ASSERT_FLOAT_IS_INF((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_NEG_INF_MESSAGE(actual, message)                                      HUNT_TEST_ASSERT_FLOAT_IS_NEG_INF((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_NAN_MESSAGE(actual, message)                                          HUNT_TEST_ASSERT_FLOAT_IS_NAN((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_DETERMINATE_MESSAGE(actual, message)                                  HUNT_TEST_ASSERT_FLOAT_IS_DETERMINATE((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_NOT_INF_MESSAGE(actual, message)                                      HUNT_TEST_ASSERT_FLOAT_IS_NOT_INF((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_NOT_NEG_INF_MESSAGE(actual, message)                                  HUNT_TEST_ASSERT_FLOAT_IS_NOT_NEG_INF((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_NOT_NAN_MESSAGE(actual, message)                                      HUNT_TEST_ASSERT_FLOAT_IS_NOT_NAN((actual), __LINE__, (message))
#define TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE_MESSAGE(actual, message)                              HUNT_TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE((actual), __LINE__, (message))

/* Double (If Enabled) */
#define TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(delta, expected, actual, message)                        HUNT_TEST_ASSERT_DOUBLE_WITHIN((delta), (expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, message)                                HUNT_TEST_ASSERT_EQUAL_DOUBLE((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(expected, actual, num_elements, message)            HUNT_TEST_ASSERT_EQUAL_DOUBLE_ARRAY((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_DOUBLE_MESSAGE(expected, actual, num_elements, message)             HUNT_TEST_ASSERT_EACH_EQUAL_DOUBLE((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_INF_MESSAGE(actual, message)                                         HUNT_TEST_ASSERT_DOUBLE_IS_INF((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_NEG_INF_MESSAGE(actual, message)                                     HUNT_TEST_ASSERT_DOUBLE_IS_NEG_INF((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_NAN_MESSAGE(actual, message)                                         HUNT_TEST_ASSERT_DOUBLE_IS_NAN((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_DETERMINATE_MESSAGE(actual, message)                                 HUNT_TEST_ASSERT_DOUBLE_IS_DETERMINATE((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_NOT_INF_MESSAGE(actual, message)                                     HUNT_TEST_ASSERT_DOUBLE_IS_NOT_INF((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_NOT_NEG_INF_MESSAGE(actual, message)                                 HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NEG_INF((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_NOT_NAN_MESSAGE(actual, message)                                     HUNT_TEST_ASSERT_DOUBLE_IS_NOT_NAN((actual), __LINE__, (message))
#define TEST_ASSERT_DOUBLE_IS_NOT_DETERMINATE_MESSAGE(actual, message)                             HUNT_TEST_ASSERT_DOUBLE_IS_NOT_DETERMINATE((actual), __LINE__, (message))

/* Shorthand */
#ifdef HUNT_SHORTHAND_AS_OLD
#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)                                       HUNT_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, (message))
#define TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)                                   HUNT_TEST_ASSERT(((expected) != (actual)), __LINE__, (message))
#endif
#ifdef HUNT_SHORTHAND_AS_INT
#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)                                       HUNT_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, message)
#define TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)                                   HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#endif
#ifdef  HUNT_SHORTHAND_AS_MEM
#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)                                       HUNT_TEST_ASSERT_EQUAL_MEMORY((&expected), (&actual), sizeof(expected), __LINE__, message)
#define TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)                                   HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#endif
#ifdef  HUNT_SHORTHAND_AS_RAW
#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)                                       HUNT_TEST_ASSERT(((expected) == (actual)), __LINE__, message)
#define TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)                                   HUNT_TEST_ASSERT(((expected) != (actual)), __LINE__, message)
#endif
#ifdef HUNT_SHORTHAND_AS_NONE
#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)                                       HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#define TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)                                   HUNT_TEST_FAIL(__LINE__, HuntStrErrShorthand)
#endif

/* end of HUNT_FRAMEWORK_H */
#ifdef __cplusplus
}
#endif
#endif
