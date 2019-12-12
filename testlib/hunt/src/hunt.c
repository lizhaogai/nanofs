/* =========================================================================
    Hunt Project - A Test Framework for C
    Copyright (c) 2007-19 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
============================================================================ */

#include "hunt.h"
#include <stddef.h>

#ifdef AVR
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

/* If omitted from header, declare overrideable prototypes here so they're ready for use */
#ifdef HUNT_OMIT_OUTPUT_CHAR_HEADER_DECLARATION
void HUNT_OUTPUT_CHAR(int);
#endif

/* Helpful macros for us to use here in Assert functions */
#define HUNT_FAIL_AND_BAIL   { Hunt.CurrentTestFailed  = 1; HUNT_OUTPUT_FLUSH(); TEST_ABORT(); }
#define HUNT_IGNORE_AND_BAIL { Hunt.CurrentTestIgnored = 1; HUNT_OUTPUT_FLUSH(); TEST_ABORT(); }
#define RETURN_IF_FAIL_OR_IGNORE if (Hunt.CurrentTestFailed || Hunt.CurrentTestIgnored) return

struct HUNT_STORAGE_T Hunt;

#ifdef HUNT_OUTPUT_COLOR
const char PROGMEM HuntStrOk[]                            = "\033[42mOK\033[00m";
const char PROGMEM HuntStrPass[]                          = "\033[42mPASS\033[00m";
const char PROGMEM HuntStrFail[]                          = "\033[41mFAIL\033[00m";
const char PROGMEM HuntStrIgnore[]                        = "\033[43mIGNORE\033[00m";
#else
const char PROGMEM HuntStrOk[] = "OK";
const char PROGMEM HuntStrPass[] = "PASS";
const char PROGMEM HuntStrFail[] = "FAIL";
const char PROGMEM HuntStrIgnore[] = "IGNORE";
#endif
static const char PROGMEM HuntStrNull[] = "NULL";
static const char PROGMEM HuntStrSpacer[] = ". ";
static const char PROGMEM HuntStrExpected[] = " Expected ";
static const char PROGMEM HuntStrWas[] = " Was ";
static const char PROGMEM HuntStrGt[] = " to be greater than ";
static const char PROGMEM HuntStrLt[] = " to be less than ";
static const char PROGMEM HuntStrOrEqual[] = "or equal to ";
static const char PROGMEM HuntStrElement[] = " Element ";
static const char PROGMEM HuntStrByte[] = " Byte ";
static const char PROGMEM HuntStrMemory[] = " Memory Mismatch.";
static const char PROGMEM HuntStrDelta[] = " Values Not Within Delta ";
static const char PROGMEM HuntStrPointless[] = " You Asked Me To Compare Nothing, Which Was Pointless.";
static const char PROGMEM HuntStrNullPointerForExpected[] = " Expected pointer to be NULL";
static const char PROGMEM HuntStrNullPointerForActual[] = " Actual pointer was NULL";
#ifndef HUNT_EXCLUDE_FLOAT
static const char PROGMEM HuntStrNot[] = "Not ";
static const char PROGMEM HuntStrInf[] = "Infinity";
static const char PROGMEM HuntStrNegInf[] = "Negative Infinity";
static const char PROGMEM HuntStrNaN[] = "NaN";
static const char PROGMEM HuntStrDet[] = "Determinate";
static const char PROGMEM HuntStrInvalidFloatTrait[] = "Invalid Float Trait";
#endif
const char PROGMEM HuntStrErrShorthand[] = "Hunt Shorthand Support Disabled";
const char PROGMEM HuntStrErrFloat[] = "Hunt Floating Point Disabled";
const char PROGMEM HuntStrErrDouble[] = "Hunt Double Precision Disabled";
const char PROGMEM HuntStrErr64[] = "Hunt 64-bit Support Disabled";
static const char PROGMEM HuntStrBreaker[] = "-----------------------";
static const char PROGMEM HuntStrResultsTests[] = " Tests ";
static const char PROGMEM HuntStrResultsFailures[] = " Failures ";
static const char PROGMEM HuntStrResultsIgnored[] = " Ignored ";
static const char PROGMEM HuntStrDetail1Name[] = HUNT_DETAIL1_NAME " ";
static const char PROGMEM HuntStrDetail2Name[] = " " HUNT_DETAIL2_NAME " ";

/*-----------------------------------------------
 * Pretty Printers & Test Result Output Handlers
 *-----------------------------------------------*/

/*-----------------------------------------------*/
/* Local helper function to print characters. */
static void HuntPrintChar(const char *pch) {
  /* printable characters plus CR & LF are printed */
  if ((*pch <= 126) && (*pch >= 32)) {
    HUNT_OUTPUT_CHAR(*pch);
  }
    /* write escaped carriage returns */
  else if (*pch == 13) {
    HUNT_OUTPUT_CHAR('\\');
    HUNT_OUTPUT_CHAR('r');
  }
    /* write escaped line feeds */
  else if (*pch == 10) {
    HUNT_OUTPUT_CHAR('\\');
    HUNT_OUTPUT_CHAR('n');
  }
    /* unprintable characters are shown as codes */
  else {
    HUNT_OUTPUT_CHAR('\\');
    HUNT_OUTPUT_CHAR('x');
    HuntPrintNumberHex((HUNT_UINT) *pch, 2);
  }
}

/*-----------------------------------------------*/
/* Local helper function to print ANSI escape strings e.g. "\033[42m". */
#ifdef HUNT_OUTPUT_COLOR
static HUNT_UINT HuntPrintAnsiEscapeString(const char* string)
{
    const char* pch = string;
    HUNT_UINT count = 0;

    while (*pch && (*pch != 'm'))
    {
        HUNT_OUTPUT_CHAR(*pch);
        pch++;
        count++;
    }
    HUNT_OUTPUT_CHAR('m');
    count++;

    return count;
}
#endif

/*-----------------------------------------------*/
void HuntPrint(const char *string) {
  const char *pch = string;

  if (pch != NULL) {
    while (*pch) {
#ifdef HUNT_OUTPUT_COLOR
      /* print ANSI escape code */
      if ((*pch == 27) && (*(pch + 1) == '['))
      {
          pch += HuntPrintAnsiEscapeString(pch);
          continue;
      }
#endif
      HuntPrintChar(pch);
      pch++;
    }
  }
}

/*-----------------------------------------------*/
#ifdef HUNT_INCLUDE_PRINT_FORMATTED
void HuntPrintFormatted(const char* format, ...)
{
    const char* pch = format;
    va_list va;
    va_start(va, format);

    if (pch != NULL)
    {
        while (*pch)
        {
            /* format identification character */
            if (*pch == '%')
            {
                pch++;

                if (pch != NULL)
                {
                    switch (*pch)
                    {
                        case 'd':
                        case 'i':
                            {
                                const int number = va_arg(va, int);
                                HuntPrintNumber((HUNT_INT)number);
                                break;
                            }
#ifndef HUNT_EXCLUDE_FLOAT_PRINT
                        case 'f':
                        case 'g':
                            {
                                const double number = va_arg(va, double);
                                HuntPrintFloat((HUNT_DOUBLE)number);
                                break;
                            }
#endif
                        case 'u':
                            {
                                const unsigned int number = va_arg(va, unsigned int);
                                HuntPrintNumberUnsigned((HUNT_UINT)number);
                                break;
                            }
                        case 'b':
                            {
                                const unsigned int number = va_arg(va, unsigned int);
                                const HUNT_UINT mask = (HUNT_UINT)0 - (HUNT_UINT)1;
                                HUNT_OUTPUT_CHAR('0');
                                HUNT_OUTPUT_CHAR('b');
                                HuntPrintMask(mask, (HUNT_UINT)number);
                                break;
                            }
                        case 'x':
                        case 'X':
                        case 'p':
                            {
                                const unsigned int number = va_arg(va, unsigned int);
                                HUNT_OUTPUT_CHAR('0');
                                HUNT_OUTPUT_CHAR('x');
                                HuntPrintNumberHex((HUNT_UINT)number, 8);
                                break;
                            }
                        case 'c':
                            {
                                const int ch = va_arg(va, int);
                                HuntPrintChar((const char *)&ch);
                                break;
                            }
                        case 's':
                            {
                                const char * string = va_arg(va, const char *);
                                HuntPrint(string);
                                break;
                            }
                        case '%':
                            {
                                HuntPrintChar(pch);
                                break;
                            }
                        default:
                            {
                                /* print the unknown format character */
                                HUNT_OUTPUT_CHAR('%');
                                HuntPrintChar(pch);
                                break;
                            }
                    }
                }
            }
#ifdef HUNT_OUTPUT_COLOR
            /* print ANSI escape code */
            else if ((*pch == 27) && (*(pch + 1) == '['))
            {
                pch += HuntPrintAnsiEscapeString(pch);
                continue;
            }
#endif
            else if (*pch == '\n')
            {
                HUNT_PRINT_EOL();
            }
            else
            {
                HuntPrintChar(pch);
            }

            pch++;
        }
    }

    va_end(va);
}
#endif /* ! HUNT_INCLUDE_PRINT_FORMATTED */

/*-----------------------------------------------*/
void HuntPrintLen(const char *string, const HUNT_UINT32 length) {
  const char *pch = string;

  if (pch != NULL) {
    while (*pch && ((HUNT_UINT32) (pch - string) < length)) {
      /* printable characters plus CR & LF are printed */
      if ((*pch <= 126) && (*pch >= 32)) {
        HUNT_OUTPUT_CHAR(*pch);
      }
        /* write escaped carriage returns */
      else if (*pch == 13) {
        HUNT_OUTPUT_CHAR('\\');
        HUNT_OUTPUT_CHAR('r');
      }
        /* write escaped line feeds */
      else if (*pch == 10) {
        HUNT_OUTPUT_CHAR('\\');
        HUNT_OUTPUT_CHAR('n');
      }
        /* unprintable characters are shown as codes */
      else {
        HUNT_OUTPUT_CHAR('\\');
        HUNT_OUTPUT_CHAR('x');
        HuntPrintNumberHex((HUNT_UINT) *pch, 2);
      }
      pch++;
    }
  }
}

/*-----------------------------------------------*/
void HuntPrintNumberByStyle(const HUNT_INT number, const HUNT_DISPLAY_STYLE_T style) {
  if ((style & HUNT_DISPLAY_RANGE_INT) == HUNT_DISPLAY_RANGE_INT) {
    HuntPrintNumber(number);
  } else if ((style & HUNT_DISPLAY_RANGE_UINT) == HUNT_DISPLAY_RANGE_UINT) {
    HuntPrintNumberUnsigned((HUNT_UINT) number);
  } else {
    HUNT_OUTPUT_CHAR('0');
    HUNT_OUTPUT_CHAR('x');
    HuntPrintNumberHex((HUNT_UINT) number, (char) ((style & 0xF) * 2));
  }
}

/*-----------------------------------------------*/
void HuntPrintNumber(const HUNT_INT number_to_print) {
  HUNT_UINT number = (HUNT_UINT) number_to_print;

  if (number_to_print < 0) {
    /* A negative number, including MIN negative */
    HUNT_OUTPUT_CHAR('-');
    number = (~number) + 1;
  }
  HuntPrintNumberUnsigned(number);
}

/*-----------------------------------------------
 * basically do an itoa using as little ram as possible */
void HuntPrintNumberUnsigned(const HUNT_UINT number) {
  HUNT_UINT divisor = 1;

  /* figure out initial divisor */
  while (number / divisor > 9) {
    divisor *= 10;
  }

  /* now mod and print, then divide divisor */
  do {
    HUNT_OUTPUT_CHAR((char) ('0' + (number / divisor % 10)));
    divisor /= 10;
  } while (divisor > 0);
}

/*-----------------------------------------------*/
void HuntPrintNumberHex(const HUNT_UINT number, const char nibbles_to_print) {
  int nibble;
  char nibbles = nibbles_to_print;

  if ((unsigned) nibbles > HUNT_MAX_NIBBLES) {
    nibbles = HUNT_MAX_NIBBLES;
  }

  while (nibbles > 0) {
    nibbles--;
    nibble = (int) (number >> (nibbles * 4)) & 0x0F;
    if (nibble <= 9) {
      HUNT_OUTPUT_CHAR((char) ('0' + nibble));
    } else {
      HUNT_OUTPUT_CHAR((char) ('A' - 10 + nibble));
    }
  }
}

/*-----------------------------------------------*/
void HuntPrintMask(const HUNT_UINT mask, const HUNT_UINT number) {
  HUNT_UINT current_bit = (HUNT_UINT) 1 << (HUNT_INT_WIDTH - 1);
  HUNT_INT32 i;

  for (i = 0; i < HUNT_INT_WIDTH; i++) {
    if (current_bit & mask) {
      if (current_bit & number) {
        HUNT_OUTPUT_CHAR('1');
      } else {
        HUNT_OUTPUT_CHAR('0');
      }
    } else {
      HUNT_OUTPUT_CHAR('X');
    }
    current_bit = current_bit >> 1;
  }
}

/*-----------------------------------------------*/
#ifndef HUNT_EXCLUDE_FLOAT_PRINT

/*
 * This function prints a floating-point value in a format similar to
 * printf("%.7g") on a single-precision machine or printf("%.9g") on a
 * double-precision machine.  The 7th digit won't always be totally correct
 * in single-precision operation (for that level of accuracy, a more
 * complicated algorithm would be needed).
 */
void HuntPrintFloat(const HUNT_DOUBLE input_number) {
#ifdef HUNT_INCLUDE_DOUBLE
  static const int sig_digits = 9;
  static const HUNT_INT32 min_scaled = 100000000;
  static const HUNT_INT32 max_scaled = 1000000000;
#else
  static const int sig_digits = 7;
  static const HUNT_INT32 min_scaled = 1000000;
  static const HUNT_INT32 max_scaled = 10000000;
#endif

  HUNT_DOUBLE number = input_number;

  /* print minus sign (including for negative zero) */
  if ((number < 0.0f) || ((number == 0.0f) && ((1.0f / number) < 0.0f))) {
    HUNT_OUTPUT_CHAR('-');
    number = -number;
  }

  /* handle zero, NaN, and +/- infinity */
  if (number == 0.0f) {
    HuntPrint("0");
  } else if (isnan(number)) {
    HuntPrint("nan");
  } else if (isinf(number)) {
    HuntPrint("inf");
  } else {
    HUNT_INT32 n_int = 0, n;
    int exponent = 0;
    int decimals, digits;
    char buf[16] = {0};

    /*
     * Scale up or down by powers of 10.  To minimize rounding error,
     * start with a factor/divisor of 10^10, which is the largest
     * power of 10 that can be represented exactly.  Finally, compute
     * (exactly) the remaining power of 10 and perform one more
     * multiplication or division.
     */
    if (number < 1.0f) {
      HUNT_DOUBLE factor = 1.0f;

      while (number < (HUNT_DOUBLE) max_scaled / 1e10f) {
        number *= 1e10f;
        exponent -= 10;
      }
      while (number * factor < (HUNT_DOUBLE) min_scaled) {
        factor *= 10.0f;
        exponent--;
      }

      number *= factor;
    } else if (number > (HUNT_DOUBLE) max_scaled) {
      HUNT_DOUBLE divisor = 1.0f;

      while (number > (HUNT_DOUBLE) min_scaled * 1e10f) {
        number /= 1e10f;
        exponent += 10;
      }
      while (number / divisor > (HUNT_DOUBLE) max_scaled) {
        divisor *= 10.0f;
        exponent++;
      }

      number /= divisor;
    } else {
      /*
       * In this range, we can split off the integer part before
       * doing any multiplications.  This reduces rounding error by
       * freeing up significant bits in the fractional part.
       */
      HUNT_DOUBLE factor = 1.0f;
      n_int = (HUNT_INT32) number;
      number -= (HUNT_DOUBLE) n_int;

      while (n_int < min_scaled) {
        n_int *= 10;
        factor *= 10.0f;
        exponent--;
      }

      number *= factor;
    }

    /* round to nearest integer */
    n = ((HUNT_INT32) (number + number) + 1) / 2;

#ifndef HUNT_ROUND_TIES_AWAY_FROM_ZERO
    /* round to even if exactly between two integers */
    if ((n & 1) && (((HUNT_DOUBLE) n - number) == 0.5f))
      n--;
#endif

    n += n_int;

    if (n >= max_scaled) {
      n = min_scaled;
      exponent++;
    }

    /* determine where to place decimal point */
    decimals = ((exponent <= 0) && (exponent >= -(sig_digits + 3))) ? (-exponent) : (sig_digits - 1);
    exponent += decimals;

    /* truncate trailing zeroes after decimal point */
    while ((decimals > 0) && ((n % 10) == 0)) {
      n /= 10;
      decimals--;
    }

    /* build up buffer in reverse order */
    digits = 0;
    while ((n != 0) || (digits < (decimals + 1))) {
      buf[digits++] = (char) ('0' + n % 10);
      n /= 10;
    }
    while (digits > 0) {
      if (digits == decimals) { HUNT_OUTPUT_CHAR('.'); }
      HUNT_OUTPUT_CHAR(buf[--digits]);
    }

    /* print exponent if needed */
    if (exponent != 0) {
      HUNT_OUTPUT_CHAR('e');

      if (exponent < 0) {
        HUNT_OUTPUT_CHAR('-');
        exponent = -exponent;
      } else {
        HUNT_OUTPUT_CHAR('+');
      }

      digits = 0;
      while ((exponent != 0) || (digits < 2)) {
        buf[digits++] = (char) ('0' + exponent % 10);
        exponent /= 10;
      }
      while (digits > 0) {
        HUNT_OUTPUT_CHAR(buf[--digits]);
      }
    }
  }
}

#endif /* ! HUNT_EXCLUDE_FLOAT_PRINT */

/*-----------------------------------------------*/
static void HuntTestResultsBegin(const char *file, const HUNT_LINE_TYPE line) {
  HuntPrint(file);
  HUNT_OUTPUT_CHAR(':');
  HuntPrintNumber((HUNT_INT) line);
  HUNT_OUTPUT_CHAR(':');
  HuntPrint(Hunt.CurrentTestName);
  HUNT_OUTPUT_CHAR(':');
}

/*-----------------------------------------------*/
static void HuntTestResultsFailBegin(const HUNT_LINE_TYPE line) {
  HuntTestResultsBegin(Hunt.TestFile, line);
  HuntPrint(HuntStrFail);
  HUNT_OUTPUT_CHAR(':');
}

/*-----------------------------------------------*/
void HuntConcludeTest(void) {
  if (Hunt.CurrentTestIgnored) {
    Hunt.TestIgnores++;
  } else if (!Hunt.CurrentTestFailed) {
    HuntTestResultsBegin(Hunt.TestFile, Hunt.CurrentTestLineNumber);
    HuntPrint(HuntStrPass);
  } else {
    Hunt.TestFailures++;
  }

  Hunt.CurrentTestFailed = 0;
  Hunt.CurrentTestIgnored = 0;
  HUNT_PRINT_EXEC_TIME();
  HUNT_PRINT_EOL();
  HUNT_FLUSH_CALL();
}

/*-----------------------------------------------*/
static void HuntAddMsgIfSpecified(const char *msg) {
  if (msg) {
    HuntPrint(HuntStrSpacer);
#ifndef HUNT_EXCLUDE_DETAILS
    if (Hunt.CurrentDetail1) {
      HuntPrint(HuntStrDetail1Name);
      HuntPrint(Hunt.CurrentDetail1);
      if (Hunt.CurrentDetail2) {
        HuntPrint(HuntStrDetail2Name);
        HuntPrint(Hunt.CurrentDetail2);
      }
      HuntPrint(HuntStrSpacer);
    }
#endif
    HuntPrint(msg);
  }
}

/*-----------------------------------------------*/
static void HuntPrintExpectedAndActualStrings(const char *expected, const char *actual) {
  HuntPrint(HuntStrExpected);
  if (expected != NULL) {
    HUNT_OUTPUT_CHAR('\'');
    HuntPrint(expected);
    HUNT_OUTPUT_CHAR('\'');
  } else {
    HuntPrint(HuntStrNull);
  }
  HuntPrint(HuntStrWas);
  if (actual != NULL) {
    HUNT_OUTPUT_CHAR('\'');
    HuntPrint(actual);
    HUNT_OUTPUT_CHAR('\'');
  } else {
    HuntPrint(HuntStrNull);
  }
}

/*-----------------------------------------------*/
static void HuntPrintExpectedAndActualStringsLen(const char *expected,
                                                 const char *actual,
                                                 const HUNT_UINT32 length) {
  HuntPrint(HuntStrExpected);
  if (expected != NULL) {
    HUNT_OUTPUT_CHAR('\'');
    HuntPrintLen(expected, length);
    HUNT_OUTPUT_CHAR('\'');
  } else {
    HuntPrint(HuntStrNull);
  }
  HuntPrint(HuntStrWas);
  if (actual != NULL) {
    HUNT_OUTPUT_CHAR('\'');
    HuntPrintLen(actual, length);
    HUNT_OUTPUT_CHAR('\'');
  } else {
    HuntPrint(HuntStrNull);
  }
}

/*-----------------------------------------------
 * Assertion & Control Helpers
 *-----------------------------------------------*/

/*-----------------------------------------------*/
static int HuntIsOneArrayNull(HUNT_INTERNAL_PTR expected,
                              HUNT_INTERNAL_PTR actual,
                              const HUNT_LINE_TYPE lineNumber,
                              const char *msg) {
  /* Both are NULL or same pointer */
  if (expected == actual) { return 0; }

  /* print and return true if just expected is NULL */
  if (expected == NULL) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrNullPointerForExpected);
    HuntAddMsgIfSpecified(msg);
    return 1;
  }

  /* print and return true if just actual is NULL */
  if (actual == NULL) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrNullPointerForActual);
    HuntAddMsgIfSpecified(msg);
    return 1;
  }

  return 0; /* return false if neither is NULL */
}

/*-----------------------------------------------
 * Assertion Functions
 *-----------------------------------------------*/

/*-----------------------------------------------*/
void HuntAssertBits(const HUNT_INT mask,
                    const HUNT_INT expected,
                    const HUNT_INT actual,
                    const char *msg,
                    const HUNT_LINE_TYPE lineNumber) {
  RETURN_IF_FAIL_OR_IGNORE;

  if ((mask & expected) != (mask & actual)) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrExpected);
    HuntPrintMask((HUNT_UINT) mask, (HUNT_UINT) expected);
    HuntPrint(HuntStrWas);
    HuntPrintMask((HUNT_UINT) mask, (HUNT_UINT) actual);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/
void HuntAssertEqualNumber(const HUNT_INT expected,
                           const HUNT_INT actual,
                           const char *msg,
                           const HUNT_LINE_TYPE lineNumber,
                           const HUNT_DISPLAY_STYLE_T style) {
  RETURN_IF_FAIL_OR_IGNORE;

  if (expected != actual) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrExpected);
    HuntPrintNumberByStyle(expected, style);
    HuntPrint(HuntStrWas);
    HuntPrintNumberByStyle(actual, style);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/
void HuntAssertGreaterOrLessOrEqualNumber(const HUNT_INT threshold,
                                          const HUNT_INT actual,
                                          const HUNT_COMPARISON_T compare,
                                          const char *msg,
                                          const HUNT_LINE_TYPE lineNumber,
                                          const HUNT_DISPLAY_STYLE_T style) {
  int failed = 0;
  RETURN_IF_FAIL_OR_IGNORE;

  if ((threshold == actual) && (compare & HUNT_EQUAL_TO)) { return; }
  if ((threshold == actual)) { failed = 1; }

  if ((style & HUNT_DISPLAY_RANGE_INT) == HUNT_DISPLAY_RANGE_INT) {
    if ((actual > threshold) && (compare & HUNT_SMALLER_THAN)) { failed = 1; }
    if ((actual < threshold) && (compare & HUNT_GREATER_THAN)) { failed = 1; }
  } else /* UINT or HEX */
  {
    if (((HUNT_UINT) actual > (HUNT_UINT) threshold) && (compare & HUNT_SMALLER_THAN)) { failed = 1; }
    if (((HUNT_UINT) actual < (HUNT_UINT) threshold) && (compare & HUNT_GREATER_THAN)) { failed = 1; }
  }

  if (failed) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrExpected);
    HuntPrintNumberByStyle(actual, style);
    if (compare & HUNT_GREATER_THAN) { HuntPrint(HuntStrGt); }
    if (compare & HUNT_SMALLER_THAN) { HuntPrint(HuntStrLt); }
    if (compare & HUNT_EQUAL_TO) { HuntPrint(HuntStrOrEqual); }
    HuntPrintNumberByStyle(threshold, style);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

#define HuntPrintPointlessAndBail()       \
{                                          \
    HuntTestResultsFailBegin(lineNumber); \
    HuntPrint(HuntStrPointless);         \
    HuntAddMsgIfSpecified(msg);           \
    HUNT_FAIL_AND_BAIL; }

/*-----------------------------------------------*/
void HuntAssertEqualIntArray(HUNT_INTERNAL_PTR expected,
                             HUNT_INTERNAL_PTR actual,
                             const HUNT_UINT32 num_elements,
                             const char *msg,
                             const HUNT_LINE_TYPE lineNumber,
                             const HUNT_DISPLAY_STYLE_T style,
                             const HUNT_FLAGS_T flags) {
  HUNT_UINT32 elements = num_elements;
  unsigned int length = style & 0xF;
  unsigned int increment = 0;

  RETURN_IF_FAIL_OR_IGNORE;

  if (num_elements == 0) {
    HuntPrintPointlessAndBail();
  }

  if (expected == actual) {
    return; /* Both are NULL or same pointer */
  }

  if (HuntIsOneArrayNull(expected, actual, lineNumber, msg)) {
    HUNT_FAIL_AND_BAIL;
  }

  while ((elements > 0) && (elements--)) {
    HUNT_INT expect_val;
    HUNT_INT actual_val;

    switch (length) {
      case 1:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT8 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT8 *) actual;
        increment = sizeof(HUNT_INT8);
        break;

      case 2:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT16 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT16 *) actual;
        increment = sizeof(HUNT_INT16);
        break;

#ifdef HUNT_SUPPORT_64
      case 8:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT64 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT64 *) actual;
        increment = sizeof(HUNT_INT64);
        break;
#endif

      default: /* default is length 4 bytes */
      case 4:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT32 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT32 *) actual;
        increment = sizeof(HUNT_INT32);
        length = 4;
        break;
    }

    if (expect_val != actual_val) {
      if ((style & HUNT_DISPLAY_RANGE_UINT) && (length < (HUNT_INT_WIDTH /
                                                          8))) {   /* For UINT, remove sign extension (padding 1's) from signed type casts above */
        HUNT_INT mask = 1;
        mask = (mask << 8 * length) - 1;
        expect_val &= mask;
        actual_val &= mask;
      }
      HuntTestResultsFailBegin(lineNumber);
      HuntPrint(HuntStrElement);
      HuntPrintNumberUnsigned(num_elements - elements - 1);
      HuntPrint(HuntStrExpected);
      HuntPrintNumberByStyle(expect_val, style);
      HuntPrint(HuntStrWas);
      HuntPrintNumberByStyle(actual_val, style);
      HuntAddMsgIfSpecified(msg);
      HUNT_FAIL_AND_BAIL;
    }
    /* Walk through array by incrementing the pointers */
    if (flags == HUNT_ARRAY_TO_ARRAY) {
      expected = (HUNT_INTERNAL_PTR) ((const char *) expected + increment);
    }
    actual = (HUNT_INTERNAL_PTR) ((const char *) actual + increment);
  }
}

/*-----------------------------------------------*/
#ifndef HUNT_EXCLUDE_FLOAT
/* Wrap this define in a function with variable types as float or double */
#define HUNT_FLOAT_OR_DOUBLE_WITHIN(delta, expected, actual, diff)                           \
    if (isinf(expected) && isinf(actual) && (((expected) < 0) == ((actual) < 0))) return 1;   \
    if (HUNT_NAN_CHECK) return 1;                                                            \
    (diff) = (actual) - (expected);                                                           \
    if ((diff) < 0) (diff) = -(diff);                                                         \
    if ((delta) < 0) (delta) = -(delta);                                                      \
    return !(isnan(diff) || isinf(diff) || ((diff) > (delta)))
/* This first part of this condition will catch any NaN or Infinite values */
#ifndef HUNT_NAN_NOT_EQUAL_NAN
#define HUNT_NAN_CHECK isnan(expected) && isnan(actual)
#else
#define HUNT_NAN_CHECK 0
#endif

#ifndef HUNT_EXCLUDE_FLOAT_PRINT
#define HUNT_PRINT_EXPECTED_AND_ACTUAL_FLOAT(expected, actual) \
  {                                                               \
    HuntPrint(HuntStrExpected);                                 \
    HuntPrintFloat(expected);                                    \
    HuntPrint(HuntStrWas);                                      \
    HuntPrintFloat(actual); }
#else
#define HUNT_PRINT_EXPECTED_AND_ACTUAL_FLOAT(expected, actual) \
    HuntPrint(HuntStrDelta)
#endif /* HUNT_EXCLUDE_FLOAT_PRINT */

/*-----------------------------------------------*/
static int HuntFloatsWithin(HUNT_FLOAT delta, HUNT_FLOAT expected, HUNT_FLOAT actual) {
  HUNT_FLOAT diff;
  HUNT_FLOAT_OR_DOUBLE_WITHIN(delta, expected, actual, diff);
}

/*-----------------------------------------------*/
void HuntAssertEqualFloatArray(HUNT_PTR_ATTRIBUTE const HUNT_FLOAT *expected,
                               HUNT_PTR_ATTRIBUTE const HUNT_FLOAT *actual,
                               const HUNT_UINT32 num_elements,
                               const char *msg,
                               const HUNT_LINE_TYPE lineNumber,
                               const HUNT_FLAGS_T flags) {
  HUNT_UINT32 elements = num_elements;
  HUNT_PTR_ATTRIBUTE const HUNT_FLOAT *ptr_expected = expected;
  HUNT_PTR_ATTRIBUTE const HUNT_FLOAT *ptr_actual = actual;

  RETURN_IF_FAIL_OR_IGNORE;

  if (elements == 0) {
    HuntPrintPointlessAndBail();
  }

  if (expected == actual) {
    return; /* Both are NULL or same pointer */
  }

  if (HuntIsOneArrayNull((HUNT_INTERNAL_PTR) expected, (HUNT_INTERNAL_PTR) actual, lineNumber, msg)) {
    HUNT_FAIL_AND_BAIL;
  }

  while (elements--) {
    if (!HuntFloatsWithin(*ptr_expected * HUNT_FLOAT_PRECISION, *ptr_expected, *ptr_actual)) {
      HuntTestResultsFailBegin(lineNumber);
      HuntPrint(HuntStrElement);
      HuntPrintNumberUnsigned(num_elements - elements - 1);
      HUNT_PRINT_EXPECTED_AND_ACTUAL_FLOAT((HUNT_DOUBLE) *ptr_expected, (HUNT_DOUBLE) *ptr_actual);
      HuntAddMsgIfSpecified(msg);
      HUNT_FAIL_AND_BAIL;
    }
    if (flags == HUNT_ARRAY_TO_ARRAY) {
      ptr_expected++;
    }
    ptr_actual++;
  }
}

/*-----------------------------------------------*/
void HuntAssertFloatsWithin(const HUNT_FLOAT delta,
                            const HUNT_FLOAT expected,
                            const HUNT_FLOAT actual,
                            const char *msg,
                            const HUNT_LINE_TYPE lineNumber) {
  RETURN_IF_FAIL_OR_IGNORE;


  if (!HuntFloatsWithin(delta, expected, actual)) {
    HuntTestResultsFailBegin(lineNumber);
    HUNT_PRINT_EXPECTED_AND_ACTUAL_FLOAT((HUNT_DOUBLE) expected, (HUNT_DOUBLE) actual);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/
void HuntAssertFloatSpecial(const HUNT_FLOAT actual,
                            const char *msg,
                            const HUNT_LINE_TYPE lineNumber,
                            const HUNT_FLOAT_TRAIT_T style) {
  const char *trait_names[] = {HuntStrInf, HuntStrNegInf, HuntStrNaN, HuntStrDet};
  HUNT_INT should_be_trait = ((HUNT_INT) style & 1);
  HUNT_INT is_trait = !should_be_trait;
  HUNT_INT trait_index = (HUNT_INT) (style >> 1);

  RETURN_IF_FAIL_OR_IGNORE;

  switch (style) {
    case HUNT_FLOAT_IS_INF:
    case HUNT_FLOAT_IS_NOT_INF:
      is_trait = isinf(actual) && (actual > 0);
      break;
    case HUNT_FLOAT_IS_NEG_INF:
    case HUNT_FLOAT_IS_NOT_NEG_INF:
      is_trait = isinf(actual) && (actual < 0);
      break;

    case HUNT_FLOAT_IS_NAN:
    case HUNT_FLOAT_IS_NOT_NAN:
      is_trait = isnan(actual) ? 1 : 0;
      break;

    case HUNT_FLOAT_IS_DET: /* A determinate number is non infinite and not NaN. */
    case HUNT_FLOAT_IS_NOT_DET:
      is_trait = !isinf(actual) && !isnan(actual);
      break;

    default:
      trait_index = 0;
      trait_names[0] = HuntStrInvalidFloatTrait;
      break;
  }

  if (is_trait != should_be_trait) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrExpected);
    if (!should_be_trait) {
      HuntPrint(HuntStrNot);
    }
    HuntPrint(trait_names[trait_index]);
    HuntPrint(HuntStrWas);
#ifndef HUNT_EXCLUDE_FLOAT_PRINT
    HuntPrintFloat((HUNT_DOUBLE) actual);
#else
    if (should_be_trait)
    {
        HuntPrint(HuntStrNot);
    }
    HuntPrint(trait_names[trait_index]);
#endif
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

#endif /* not HUNT_EXCLUDE_FLOAT */

/*-----------------------------------------------*/
#ifndef HUNT_EXCLUDE_DOUBLE
static int HuntDoublesWithin(HUNT_DOUBLE delta, HUNT_DOUBLE expected, HUNT_DOUBLE actual)
{
    HUNT_DOUBLE diff;
    HUNT_FLOAT_OR_DOUBLE_WITHIN(delta, expected, actual, diff);
}

/*-----------------------------------------------*/
void HuntAssertEqualDoubleArray(HUNT_PTR_ATTRIBUTE const HUNT_DOUBLE* expected,
                                 HUNT_PTR_ATTRIBUTE const HUNT_DOUBLE* actual,
                                 const HUNT_UINT32 num_elements,
                                 const char* msg,
                                 const HUNT_LINE_TYPE lineNumber,
                                 const HUNT_FLAGS_T flags)
{
    HUNT_UINT32 elements = num_elements;
    HUNT_PTR_ATTRIBUTE const HUNT_DOUBLE* ptr_expected = expected;
    HUNT_PTR_ATTRIBUTE const HUNT_DOUBLE* ptr_actual = actual;

    RETURN_IF_FAIL_OR_IGNORE;

    if (elements == 0)
    {
        HuntPrintPointlessAndBail();
    }

    if (expected == actual)
    {
        return; /* Both are NULL or same pointer */
    }

    if (HuntIsOneArrayNull((HUNT_INTERNAL_PTR)expected, (HUNT_INTERNAL_PTR)actual, lineNumber, msg))
    {
        HUNT_FAIL_AND_BAIL;
    }

    while (elements--)
    {
        if (!HuntDoublesWithin(*ptr_expected * HUNT_DOUBLE_PRECISION, *ptr_expected, *ptr_actual))
        {
            HuntTestResultsFailBegin(lineNumber);
            HuntPrint(HuntStrElement);
            HuntPrintNumberUnsigned(num_elements - elements - 1);
            HUNT_PRINT_EXPECTED_AND_ACTUAL_FLOAT(*ptr_expected, *ptr_actual);
            HuntAddMsgIfSpecified(msg);
            HUNT_FAIL_AND_BAIL;
        }
        if (flags == HUNT_ARRAY_TO_ARRAY)
        {
            ptr_expected++;
        }
        ptr_actual++;
    }
}

/*-----------------------------------------------*/
void HuntAssertDoublesWithin(const HUNT_DOUBLE delta,
                              const HUNT_DOUBLE expected,
                              const HUNT_DOUBLE actual,
                              const char* msg,
                              const HUNT_LINE_TYPE lineNumber)
{
    RETURN_IF_FAIL_OR_IGNORE;

    if (!HuntDoublesWithin(delta, expected, actual))
    {
        HuntTestResultsFailBegin(lineNumber);
        HUNT_PRINT_EXPECTED_AND_ACTUAL_FLOAT(expected, actual);
        HuntAddMsgIfSpecified(msg);
        HUNT_FAIL_AND_BAIL;
    }
}

/*-----------------------------------------------*/
void HuntAssertDoubleSpecial(const HUNT_DOUBLE actual,
                              const char* msg,
                              const HUNT_LINE_TYPE lineNumber,
                              const HUNT_FLOAT_TRAIT_T style)
{
    const char* trait_names[] = {HuntStrInf, HuntStrNegInf, HuntStrNaN, HuntStrDet};
    HUNT_INT should_be_trait = ((HUNT_INT)style & 1);
    HUNT_INT is_trait        = !should_be_trait;
    HUNT_INT trait_index     = (HUNT_INT)(style >> 1);

    RETURN_IF_FAIL_OR_IGNORE;

    switch (style)
    {
        case HUNT_FLOAT_IS_INF:
        case HUNT_FLOAT_IS_NOT_INF:
            is_trait = isinf(actual) && (actual > 0);
            break;
        case HUNT_FLOAT_IS_NEG_INF:
        case HUNT_FLOAT_IS_NOT_NEG_INF:
            is_trait = isinf(actual) && (actual < 0);
            break;

        case HUNT_FLOAT_IS_NAN:
        case HUNT_FLOAT_IS_NOT_NAN:
            is_trait = isnan(actual) ? 1 : 0;
            break;

        case HUNT_FLOAT_IS_DET: /* A determinate number is non infinite and not NaN. */
        case HUNT_FLOAT_IS_NOT_DET:
            is_trait = !isinf(actual) && !isnan(actual);
            break;

        default:
            trait_index = 0;
            trait_names[0] = HuntStrInvalidFloatTrait;
            break;
    }

    if (is_trait != should_be_trait)
    {
        HuntTestResultsFailBegin(lineNumber);
        HuntPrint(HuntStrExpected);
        if (!should_be_trait)
        {
            HuntPrint(HuntStrNot);
        }
        HuntPrint(trait_names[trait_index]);
        HuntPrint(HuntStrWas);
#ifndef HUNT_EXCLUDE_FLOAT_PRINT
        HuntPrintFloat(actual);
#else
        if (should_be_trait)
        {
            HuntPrint(HuntStrNot);
        }
        HuntPrint(trait_names[trait_index]);
#endif
        HuntAddMsgIfSpecified(msg);
        HUNT_FAIL_AND_BAIL;
    }
}

#endif /* not HUNT_EXCLUDE_DOUBLE */

/*-----------------------------------------------*/
void HuntAssertNumbersWithin(const HUNT_UINT delta,
                             const HUNT_INT expected,
                             const HUNT_INT actual,
                             const char *msg,
                             const HUNT_LINE_TYPE lineNumber,
                             const HUNT_DISPLAY_STYLE_T style) {
  RETURN_IF_FAIL_OR_IGNORE;

  if ((style & HUNT_DISPLAY_RANGE_INT) == HUNT_DISPLAY_RANGE_INT) {
    if (actual > expected) {
      Hunt.CurrentTestFailed = (((HUNT_UINT) actual - (HUNT_UINT) expected) > delta);
    } else {
      Hunt.CurrentTestFailed = (((HUNT_UINT) expected - (HUNT_UINT) actual) > delta);
    }
  } else {
    if ((HUNT_UINT) actual > (HUNT_UINT) expected) {
      Hunt.CurrentTestFailed = (((HUNT_UINT) actual - (HUNT_UINT) expected) > delta);
    } else {
      Hunt.CurrentTestFailed = (((HUNT_UINT) expected - (HUNT_UINT) actual) > delta);
    }
  }

  if (Hunt.CurrentTestFailed) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrint(HuntStrDelta);
    HuntPrintNumberByStyle((HUNT_INT) delta, style);
    HuntPrint(HuntStrExpected);
    HuntPrintNumberByStyle(expected, style);
    HuntPrint(HuntStrWas);
    HuntPrintNumberByStyle(actual, style);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/
void HuntAssertNumbersArrayWithin(const HUNT_UINT delta,
                                  HUNT_INTERNAL_PTR expected,
                                  HUNT_INTERNAL_PTR actual,
                                  const HUNT_UINT32 num_elements,
                                  const char *msg,
                                  const HUNT_LINE_TYPE lineNumber,
                                  const HUNT_DISPLAY_STYLE_T style,
                                  const HUNT_FLAGS_T flags) {
  HUNT_UINT32 elements = num_elements;
  unsigned int length = style & 0xF;
  unsigned int increment = 0;

  RETURN_IF_FAIL_OR_IGNORE;

  if (num_elements == 0) {
    HuntPrintPointlessAndBail();
  }

  if (expected == actual) {
    return; /* Both are NULL or same pointer */
  }

  if (HuntIsOneArrayNull(expected, actual, lineNumber, msg)) {
    HUNT_FAIL_AND_BAIL;
  }

  while ((elements > 0) && (elements--)) {
    HUNT_INT expect_val;
    HUNT_INT actual_val;

    switch (length) {
      case 1:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT8 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT8 *) actual;
        increment = sizeof(HUNT_INT8);
        break;

      case 2:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT16 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT16 *) actual;
        increment = sizeof(HUNT_INT16);
        break;

#ifdef HUNT_SUPPORT_64
      case 8:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT64 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT64 *) actual;
        increment = sizeof(HUNT_INT64);
        break;
#endif

      default: /* default is length 4 bytes */
      case 4:
        expect_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT32 *) expected;
        actual_val = *(HUNT_PTR_ATTRIBUTE const HUNT_INT32 *) actual;
        increment = sizeof(HUNT_INT32);
        length = 4;
        break;
    }

    if ((style & HUNT_DISPLAY_RANGE_INT) == HUNT_DISPLAY_RANGE_INT) {
      if (actual_val > expect_val) {
        Hunt.CurrentTestFailed = (((HUNT_UINT) actual_val - (HUNT_UINT) expect_val) > delta);
      } else {
        Hunt.CurrentTestFailed = (((HUNT_UINT) expect_val - (HUNT_UINT) actual_val) > delta);
      }
    } else {
      if ((HUNT_UINT) actual_val > (HUNT_UINT) expect_val) {
        Hunt.CurrentTestFailed = (((HUNT_UINT) actual_val - (HUNT_UINT) expect_val) > delta);
      } else {
        Hunt.CurrentTestFailed = (((HUNT_UINT) expect_val - (HUNT_UINT) actual_val) > delta);
      }
    }

    if (Hunt.CurrentTestFailed) {
      if ((style & HUNT_DISPLAY_RANGE_UINT) && (length < (HUNT_INT_WIDTH /
                                                          8))) {   /* For UINT, remove sign extension (padding 1's) from signed type casts above */
        HUNT_INT mask = 1;
        mask = (mask << 8 * length) - 1;
        expect_val &= mask;
        actual_val &= mask;
      }
      HuntTestResultsFailBegin(lineNumber);
      HuntPrint(HuntStrDelta);
      HuntPrintNumberByStyle((HUNT_INT) delta, style);
      HuntPrint(HuntStrElement);
      HuntPrintNumberUnsigned(num_elements - elements - 1);
      HuntPrint(HuntStrExpected);
      HuntPrintNumberByStyle(expect_val, style);
      HuntPrint(HuntStrWas);
      HuntPrintNumberByStyle(actual_val, style);
      HuntAddMsgIfSpecified(msg);
      HUNT_FAIL_AND_BAIL;
    }
    /* Walk through array by incrementing the pointers */
    if (flags == HUNT_ARRAY_TO_ARRAY) {
      expected = (HUNT_INTERNAL_PTR) ((const char *) expected + increment);
    }
    actual = (HUNT_INTERNAL_PTR) ((const char *) actual + increment);
  }
}

/*-----------------------------------------------*/
void HuntAssertEqualString(const char *expected,
                           const char *actual,
                           const char *msg,
                           const HUNT_LINE_TYPE lineNumber) {
  HUNT_UINT32 i;

  RETURN_IF_FAIL_OR_IGNORE;

  /* if both pointers not null compare the strings */
  if (expected && actual) {
    for (i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        Hunt.CurrentTestFailed = 1;
        break;
      }
    }
  } else { /* handle case of one pointers being null (if both null, test should pass) */
    if (expected != actual) {
      Hunt.CurrentTestFailed = 1;
    }
  }

  if (Hunt.CurrentTestFailed) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrintExpectedAndActualStrings(expected, actual);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/
void HuntAssertEqualStringLen(const char *expected,
                              const char *actual,
                              const HUNT_UINT32 length,
                              const char *msg,
                              const HUNT_LINE_TYPE lineNumber) {
  HUNT_UINT32 i;

  RETURN_IF_FAIL_OR_IGNORE;

  /* if both pointers not null compare the strings */
  if (expected && actual) {
    for (i = 0; (i < length) && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        Hunt.CurrentTestFailed = 1;
        break;
      }
    }
  } else { /* handle case of one pointers being null (if both null, test should pass) */
    if (expected != actual) {
      Hunt.CurrentTestFailed = 1;
    }
  }

  if (Hunt.CurrentTestFailed) {
    HuntTestResultsFailBegin(lineNumber);
    HuntPrintExpectedAndActualStringsLen(expected, actual, length);
    HuntAddMsgIfSpecified(msg);
    HUNT_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/
void HuntAssertEqualStringArray(HUNT_INTERNAL_PTR expected,
                                const char **actual,
                                const HUNT_UINT32 num_elements,
                                const char *msg,
                                const HUNT_LINE_TYPE lineNumber,
                                const HUNT_FLAGS_T flags) {
  HUNT_UINT32 i = 0;
  HUNT_UINT32 j = 0;
  const char *expd = NULL;
  const char *act = NULL;

  RETURN_IF_FAIL_OR_IGNORE;

  /* if no elements, it's an error */
  if (num_elements == 0) {
    HuntPrintPointlessAndBail();
  }

  if ((const void *) expected == (const void *) actual) {
    return; /* Both are NULL or same pointer */
  }

  if (HuntIsOneArrayNull((HUNT_INTERNAL_PTR) expected, (HUNT_INTERNAL_PTR) actual, lineNumber, msg)) {
    HUNT_FAIL_AND_BAIL;
  }

  if (flags != HUNT_ARRAY_TO_ARRAY) {
    expd = (const char *) expected;
  }

  do {
    act = actual[j];
    if (flags == HUNT_ARRAY_TO_ARRAY) {
      expd = ((const char *const *) expected)[j];
    }

    /* if both pointers not null compare the strings */
    if (expd && act) {
      for (i = 0; expd[i] || act[i]; i++) {
        if (expd[i] != act[i]) {
          Hunt.CurrentTestFailed = 1;
          break;
        }
      }
    } else { /* handle case of one pointers being null (if both null, test should pass) */
      if (expd != act) {
        Hunt.CurrentTestFailed = 1;
      }
    }

    if (Hunt.CurrentTestFailed) {
      HuntTestResultsFailBegin(lineNumber);
      if (num_elements > 1) {
        HuntPrint(HuntStrElement);
        HuntPrintNumberUnsigned(j);
      }
      HuntPrintExpectedAndActualStrings(expd, act);
      HuntAddMsgIfSpecified(msg);
      HUNT_FAIL_AND_BAIL;
    }
  } while (++j < num_elements);
}

/*-----------------------------------------------*/
void HuntAssertEqualMemory(HUNT_INTERNAL_PTR expected,
                           HUNT_INTERNAL_PTR actual,
                           const HUNT_UINT32 length,
                           const HUNT_UINT32 num_elements,
                           const char *msg,
                           const HUNT_LINE_TYPE lineNumber,
                           const HUNT_FLAGS_T flags) {
  HUNT_PTR_ATTRIBUTE const unsigned char *ptr_exp = (HUNT_PTR_ATTRIBUTE const unsigned char *) expected;
  HUNT_PTR_ATTRIBUTE const unsigned char *ptr_act = (HUNT_PTR_ATTRIBUTE const unsigned char *) actual;
  HUNT_UINT32 elements = num_elements;
  HUNT_UINT32 bytes;

  RETURN_IF_FAIL_OR_IGNORE;

  if ((elements == 0) || (length == 0)) {
    HuntPrintPointlessAndBail();
  }

  if (expected == actual) {
    return; /* Both are NULL or same pointer */
  }

  if (HuntIsOneArrayNull(expected, actual, lineNumber, msg)) {
    HUNT_FAIL_AND_BAIL;
  }

  while (elements--) {
    bytes = length;
    while (bytes--) {
      if (*ptr_exp != *ptr_act) {
        HuntTestResultsFailBegin(lineNumber);
        HuntPrint(HuntStrMemory);
        if (num_elements > 1) {
          HuntPrint(HuntStrElement);
          HuntPrintNumberUnsigned(num_elements - elements - 1);
        }
        HuntPrint(HuntStrByte);
        HuntPrintNumberUnsigned(length - bytes - 1);
        HuntPrint(HuntStrExpected);
        HuntPrintNumberByStyle(*ptr_exp, HUNT_DISPLAY_STYLE_HEX8);
        HuntPrint(HuntStrWas);
        HuntPrintNumberByStyle(*ptr_act, HUNT_DISPLAY_STYLE_HEX8);
        HuntAddMsgIfSpecified(msg);
        HUNT_FAIL_AND_BAIL;
      }
      ptr_exp++;
      ptr_act++;
    }
    if (flags == HUNT_ARRAY_TO_VAL) {
      ptr_exp = (HUNT_PTR_ATTRIBUTE const unsigned char *) expected;
    }
  }
}

/*-----------------------------------------------*/

static union {
  HUNT_INT8 i8;
  HUNT_INT16 i16;
  HUNT_INT32 i32;
#ifdef HUNT_SUPPORT_64
  HUNT_INT64 i64;
#endif
#ifndef HUNT_EXCLUDE_FLOAT
  float f;
#endif
#ifndef HUNT_EXCLUDE_DOUBLE
  double d;
#endif
} HuntQuickCompare;

HUNT_INTERNAL_PTR HuntNumToPtr(const HUNT_INT num, const HUNT_UINT8 size) {
  switch (size) {
    case 1:
      HuntQuickCompare.i8 = (HUNT_INT8) num;
      return (HUNT_INTERNAL_PTR) (&HuntQuickCompare.i8);

    case 2:
      HuntQuickCompare.i16 = (HUNT_INT16) num;
      return (HUNT_INTERNAL_PTR) (&HuntQuickCompare.i16);

#ifdef HUNT_SUPPORT_64
    case 8:
      HuntQuickCompare.i64 = (HUNT_INT64) num;
      return (HUNT_INTERNAL_PTR) (&HuntQuickCompare.i64);
#endif

    default: /* 4 bytes */
      HuntQuickCompare.i32 = (HUNT_INT32) num;
      return (HUNT_INTERNAL_PTR) (&HuntQuickCompare.i32);
  }
}

#ifndef HUNT_EXCLUDE_FLOAT
/*-----------------------------------------------*/
HUNT_INTERNAL_PTR HuntFloatToPtr(const float num) {
  HuntQuickCompare.f = num;
  return (HUNT_INTERNAL_PTR) (&HuntQuickCompare.f);
}

#endif

#ifndef HUNT_EXCLUDE_DOUBLE
/*-----------------------------------------------*/
HUNT_INTERNAL_PTR HuntDoubleToPtr(const double num)
{
    HuntQuickCompare.d = num;
    return (HUNT_INTERNAL_PTR)(&HuntQuickCompare.d);
}
#endif

/*-----------------------------------------------
 * Control Functions
 *-----------------------------------------------*/

/*-----------------------------------------------*/
void HuntFail(const char *msg, const HUNT_LINE_TYPE line) {
  RETURN_IF_FAIL_OR_IGNORE;

  HuntTestResultsBegin(Hunt.TestFile, line);
  HuntPrint(HuntStrFail);
  if (msg != NULL) {
    HUNT_OUTPUT_CHAR(':');

#ifndef HUNT_EXCLUDE_DETAILS
    if (Hunt.CurrentDetail1) {
      HuntPrint(HuntStrDetail1Name);
      HuntPrint(Hunt.CurrentDetail1);
      if (Hunt.CurrentDetail2) {
        HuntPrint(HuntStrDetail2Name);
        HuntPrint(Hunt.CurrentDetail2);
      }
      HuntPrint(HuntStrSpacer);
    }
#endif
    if (msg[0] != ' ') {
      HUNT_OUTPUT_CHAR(' ');
    }
    HuntPrint(msg);
  }

  HUNT_FAIL_AND_BAIL;
}

/*-----------------------------------------------*/
void HuntIgnore(const char *msg, const HUNT_LINE_TYPE line) {
  RETURN_IF_FAIL_OR_IGNORE;

  HuntTestResultsBegin(Hunt.TestFile, line);
  HuntPrint(HuntStrIgnore);
  if (msg != NULL) {
    HUNT_OUTPUT_CHAR(':');
    HUNT_OUTPUT_CHAR(' ');
    HuntPrint(msg);
  }
  HUNT_IGNORE_AND_BAIL;
}

/*-----------------------------------------------*/
void HuntMessage(const char *msg, const HUNT_LINE_TYPE line) {
  HuntTestResultsBegin(Hunt.TestFile, line);
  HuntPrint("INFO");
  if (msg != NULL) {
    HUNT_OUTPUT_CHAR(':');
    HUNT_OUTPUT_CHAR(' ');
    HuntPrint(msg);
  }
  HUNT_PRINT_EOL();
}

/*-----------------------------------------------*/
void HuntDefaultTestRun(HuntTestFunction Func, const char *FuncName, const int FuncLineNum) {
  Hunt.CurrentTestName = FuncName;
  Hunt.CurrentTestLineNumber = (HUNT_LINE_TYPE) FuncLineNum;
  Hunt.NumberOfTests++;
  HUNT_CLR_DETAILS();
  HUNT_EXEC_TIME_START();
  if (TEST_PROTECT()) {
    setUp();
    Func();
  }
  if (TEST_PROTECT()) {
    tearDown();
  }
  HUNT_EXEC_TIME_STOP();
  HuntConcludeTest();
}

/*-----------------------------------------------*/
void HuntBegin(const char *filename) {
  Hunt.TestFile = filename;
  Hunt.CurrentTestName = NULL;
  Hunt.CurrentTestLineNumber = 0;
  Hunt.NumberOfTests = 0;
  Hunt.TestFailures = 0;
  Hunt.TestIgnores = 0;
  Hunt.CurrentTestFailed = 0;
  Hunt.CurrentTestIgnored = 0;

  HUNT_CLR_DETAILS();
  HUNT_OUTPUT_START();
}

/*-----------------------------------------------*/
int HuntEnd(void) {
  HUNT_PRINT_EOL();
  HuntPrint(HuntStrBreaker);
  HUNT_PRINT_EOL();
  HuntPrintNumber((HUNT_INT) (Hunt.NumberOfTests));
  HuntPrint(HuntStrResultsTests);
  HuntPrintNumber((HUNT_INT) (Hunt.TestFailures));
  HuntPrint(HuntStrResultsFailures);
  HuntPrintNumber((HUNT_INT) (Hunt.TestIgnores));
  HuntPrint(HuntStrResultsIgnored);
  HUNT_PRINT_EOL();
  if (Hunt.TestFailures == 0U) {
    HuntPrint(HuntStrOk);
  } else {
    HuntPrint(HuntStrFail);
#ifdef HUNT_DIFFERENTIATE_FINAL_FAIL
    HUNT_OUTPUT_CHAR('E'); HUNT_OUTPUT_CHAR('D');
#endif
  }
  HUNT_PRINT_EOL();
  HUNT_FLUSH_CALL();
  HUNT_OUTPUT_COMPLETE();
  return (int) (Hunt.TestFailures);
}

/*-----------------------------------------------
 * Command Line Argument Support
 *-----------------------------------------------*/
#ifdef HUNT_USE_COMMAND_LINE_ARGS

char* HuntOptionIncludeNamed = NULL;
char* HuntOptionExcludeNamed = NULL;
int HuntVerbosity            = 1;

/*-----------------------------------------------*/
int HuntParseOptions(int argc, char** argv)
{
    HuntOptionIncludeNamed = NULL;
    HuntOptionExcludeNamed = NULL;
    int i;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'l': /* list tests */
                    return -1;
                case 'n': /* include tests with name including this string */
                case 'f': /* an alias for -n */
                    if (argv[i][2] == '=')
                    {
                        HuntOptionIncludeNamed = &argv[i][3];
                    }
                    else if (++i < argc)
                    {
                        HuntOptionIncludeNamed = argv[i];
                    }
                    else
                    {
                        HuntPrint("ERROR: No Test String to Include Matches For");
                        HUNT_PRINT_EOL();
                        return 1;
                    }
                    break;
                case 'q': /* quiet */
                    HuntVerbosity = 0;
                    break;
                case 'v': /* verbose */
                    HuntVerbosity = 2;
                    break;
                case 'x': /* exclude tests with name including this string */
                    if (argv[i][2] == '=')
                    {
                        HuntOptionExcludeNamed = &argv[i][3];
                    }
                    else if (++i < argc)
                    {
                        HuntOptionExcludeNamed = argv[i];
                    }
                    else
                    {
                        HuntPrint("ERROR: No Test String to Exclude Matches For");
                        HUNT_PRINT_EOL();
                        return 1;
                    }
                    break;
                default:
                    HuntPrint("ERROR: Unknown Option ");
                    HUNT_OUTPUT_CHAR(argv[i][1]);
                    HUNT_PRINT_EOL();
                    return 1;
            }
        }
    }

    return 0;
}

/*-----------------------------------------------*/
int IsStringInBiggerString(const char* longstring, const char* shortstring)
{
    const char* lptr = longstring;
    const char* sptr = shortstring;
    const char* lnext = lptr;

    if (*sptr == '*')
    {
        return 1;
    }

    while (*lptr)
    {
        lnext = lptr + 1;

        /* If they current bytes match, go on to the next bytes */
        while (*lptr && *sptr && (*lptr == *sptr))
        {
            lptr++;
            sptr++;

            /* We're done if we match the entire string or up to a wildcard */
            if (*sptr == '*')
                return 1;
            if (*sptr == ',')
                return 1;
            if (*sptr == '"')
                return 1;
            if (*sptr == '\'')
                return 1;
            if (*sptr == ':')
                return 2;
            if (*sptr == 0)
                return 1;
        }

        /* Otherwise we start in the long pointer 1 character further and try again */
        lptr = lnext;
        sptr = shortstring;
    }

    return 0;
}

/*-----------------------------------------------*/
int HuntStringArgumentMatches(const char* str)
{
    int retval;
    const char* ptr1;
    const char* ptr2;
    const char* ptrf;

    /* Go through the options and get the substrings for matching one at a time */
    ptr1 = str;
    while (ptr1[0] != 0)
    {
        if ((ptr1[0] == '"') || (ptr1[0] == '\''))
        {
            ptr1++;
        }

        /* look for the start of the next partial */
        ptr2 = ptr1;
        ptrf = 0;
        do
        {
            ptr2++;
            if ((ptr2[0] == ':') && (ptr2[1] != 0) && (ptr2[0] != '\'') && (ptr2[0] != '"') && (ptr2[0] != ','))
            {
                ptrf = &ptr2[1];
            }
        } while ((ptr2[0] != 0) && (ptr2[0] != '\'') && (ptr2[0] != '"') && (ptr2[0] != ','));

        while ((ptr2[0] != 0) && ((ptr2[0] == ':') || (ptr2[0] == '\'') || (ptr2[0] == '"') || (ptr2[0] == ',')))
        {
            ptr2++;
        }

        /* done if complete filename match */
        retval = IsStringInBiggerString(Hunt.TestFile, ptr1);
        if (retval == 1)
        {
            return retval;
        }

        /* done if testname match after filename partial match */
        if ((retval == 2) && (ptrf != 0))
        {
            if (IsStringInBiggerString(Hunt.CurrentTestName, ptrf))
            {
                return 1;
            }
        }

        /* done if complete testname match */
        if (IsStringInBiggerString(Hunt.CurrentTestName, ptr1) == 1)
        {
            return 1;
        }

        ptr1 = ptr2;
    }

    /* we couldn't find a match for any substrings */
    return 0;
}

/*-----------------------------------------------*/
int HuntTestMatches(void)
{
    /* Check if this test name matches the included test pattern */
    int retval;
    if (HuntOptionIncludeNamed)
    {
        retval = HuntStringArgumentMatches(HuntOptionIncludeNamed);
    }
    else
    {
        retval = 1;
    }

    /* Check if this test name matches the excluded test pattern */
    if (HuntOptionExcludeNamed)
    {
        if (HuntStringArgumentMatches(HuntOptionExcludeNamed))
        {
            retval = 0;
        }
    }

    return retval;
}

#endif /* HUNT_USE_COMMAND_LINE_ARGS */
/*-----------------------------------------------*/
