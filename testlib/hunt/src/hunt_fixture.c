/* Copyright (c) 2010 James Grenning and Contributed to Hunt Project
 * ==========================================
 *  Hunt Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#include "hunt_fixture.h"
#include "hunt_internals.h"
#include <string.h>

struct HUNT_FIXTURE_T HuntFixture;

/* If you decide to use the function pointer approach.
 * Build with -D HUNT_OUTPUT_CHAR=outputChar and include <stdio.h>
 * int (*outputChar)(int) = putchar; */

void setUp(void) { /*does nothing*/ }

void tearDown(void) { /*does nothing*/ }

static void announceTestRun(unsigned int runNumber) {
  HuntPrint("Hunt test run ");
  HuntPrintNumberUnsigned(runNumber + 1);
  HuntPrint(" of ");
  HuntPrintNumberUnsigned(HuntFixture.RepeatCount);
  HUNT_PRINT_EOL();
}

int HuntMain(int argc, const char *argv[], void (*runAllTests)(void)) {
  int result = HuntGetCommandLineOptions(argc, argv);
  unsigned int r;
  if (result != 0)
    return result;

  for (r = 0; r < HuntFixture.RepeatCount; r++) {
    HuntBegin(argv[0]);
    announceTestRun(r);
    runAllTests();
    if (!HuntFixture.Verbose) HUNT_PRINT_EOL();
    HuntEnd();
  }

  return (int) Hunt.TestFailures;
}

static int selected(const char *filter, const char *name) {
  if (filter == 0)
    return 1;
  return strstr(name, filter) ? 1 : 0;
}

static int testSelected(const char *test) {
  return selected(HuntFixture.NameFilter, test);
}

static int groupSelected(const char *group) {
  return selected(HuntFixture.GroupFilter, group);
}

void HuntTestRunner(huntfunction *setup,
                    huntfunction *testBody,
                    huntfunction *teardown,
                    const char *printableName,
                    const char *group,
                    const char *name,
                    const char *file,
                    unsigned int line) {
  if (testSelected(name) && groupSelected(group)) {
    Hunt.TestFile = file;
    Hunt.CurrentTestName = printableName;
    Hunt.CurrentTestLineNumber = line;
    if (HuntFixture.Verbose) {
      HuntPrint(printableName);
      HuntPrint(" ...");
      HUNT_PRINT_EOL();
#ifndef HUNT_REPEAT_TEST_NAME
      Hunt.CurrentTestName = NULL;
#endif
    } else if (HuntFixture.Silent) {
      /* Do Nothing */
    } else {
      HUNT_OUTPUT_CHAR('.');
    }

    Hunt.NumberOfTests++;
    HuntMalloc_StartTest();
    HuntPointer_Init();

    HUNT_EXEC_TIME_START();

    if (TEST_PROTECT()) {
      setup();
      testBody();
    }
    if (TEST_PROTECT()) {
      teardown();
    }
    if (TEST_PROTECT()) {
      HuntPointer_UndoAllSets();
      if (!Hunt.CurrentTestFailed) {
        if (HuntFixture.Verbose) {
          HuntPrint(printableName);
        }
        HuntMalloc_EndTest();
      }
    }
    HuntConcludeFixtureTest();
  }
}

void HuntIgnoreTest(const char *printableName, const char *group, const char *name) {
  if (testSelected(name) && groupSelected(group)) {
    Hunt.NumberOfTests++;
    Hunt.TestIgnores++;
    if (HuntFixture.Verbose) {
      HuntPrint(printableName);
      HUNT_PRINT_EOL();
    } else if (HuntFixture.Silent) {
      /* Do Nothing */
    } else {
      HUNT_OUTPUT_CHAR('!');
    }
  }
}


/*------------------------------------------------- */
/* Malloc and free stuff */
#define MALLOC_DONT_FAIL -1
static int malloc_count;
static int malloc_fail_countdown = MALLOC_DONT_FAIL;

void HuntMalloc_StartTest(void) {
  malloc_count = 0;
  malloc_fail_countdown = MALLOC_DONT_FAIL;
}

void HuntMalloc_EndTest(void) {
  malloc_fail_countdown = MALLOC_DONT_FAIL;
  if (malloc_count != 0) {
    HUNT_TEST_FAIL(Hunt.CurrentTestLineNumber, "This test leaks!");
  }
}

void HuntMalloc_MakeMallocFailAfterCount(int countdown) {
  malloc_fail_countdown = countdown;
}

/* These definitions are always included from hunt_fixture_malloc_overrides.h */
/* We undef to use them or avoid conflict with <stdlib.h> per the C standard */
#undef malloc
#undef free
#undef calloc
#undef realloc

#ifdef HUNT_EXCLUDE_STDLIB_MALLOC
static unsigned char hunt_heap[HUNT_INTERNAL_HEAP_SIZE_BYTES];
static size_t heap_index;
#else

#include <stdlib.h>

#endif

typedef struct GuardBytes {
  size_t size;
  size_t guard_space;
} Guard;


#define HUNT_MALLOC_ALIGNMENT (HUNT_POINTER_WIDTH / 8)
static const char end[] = "END";


static size_t hunt_size_round_up(size_t size) {
  size_t rounded_size;

  rounded_size = ((size + HUNT_MALLOC_ALIGNMENT - 1) / HUNT_MALLOC_ALIGNMENT) * HUNT_MALLOC_ALIGNMENT;

  return rounded_size;
}

void *hunt_malloc(size_t size) {
  char *mem;
  Guard *guard;
  size_t total_size;

  total_size = sizeof(Guard) + hunt_size_round_up(size + sizeof(end));

  if (malloc_fail_countdown != MALLOC_DONT_FAIL) {
    if (malloc_fail_countdown == 0)
      return NULL;
    malloc_fail_countdown--;
  }

  if (size == 0) return NULL;
#ifdef HUNT_EXCLUDE_STDLIB_MALLOC
  if (heap_index + total_size > HUNT_INTERNAL_HEAP_SIZE_BYTES)
  {
      guard = NULL;
  }
  else
  {
      guard = (Guard*)&hunt_heap[heap_index];
      heap_index += total_size;
  }
#else
  guard = (Guard *) HUNT_FIXTURE_MALLOC(total_size);
#endif
  if (guard == NULL) return NULL;
  malloc_count++;
  guard->size = size;
  guard->guard_space = 0;
  mem = (char *) &(guard[1]);
  memcpy(&mem[size], end, sizeof(end));

  return (void *) mem;
}

static int isOverrun(void *mem) {
  Guard *guard = (Guard *) mem;
  char *memAsChar = (char *) mem;
  guard--;

  return guard->guard_space != 0 || strcmp(&memAsChar[guard->size], end) != 0;
}

static void release_memory(void *mem) {
  Guard *guard = (Guard *) mem;
  guard--;

  malloc_count--;
#ifdef HUNT_EXCLUDE_STDLIB_MALLOC
  {
      size_t block_size;

      block_size = hunt_size_round_up(guard->size + sizeof(end));

      if (mem == hunt_heap + heap_index - block_size)
      {
          heap_index -= (sizeof(Guard) + block_size);
      }
  }
#else
  HUNT_FIXTURE_FREE(guard);
#endif
}

void hunt_free(void *mem) {
  int overrun;

  if (mem == NULL) {
    return;
  }

  overrun = isOverrun(mem);
  release_memory(mem);
  if (overrun) {
    HUNT_TEST_FAIL(Hunt.CurrentTestLineNumber, "Buffer overrun detected during free()");
  }
}

void *hunt_calloc(size_t num, size_t size) {
  void *mem = hunt_malloc(num * size);
  if (mem == NULL) return NULL;
  memset(mem, 0, num * size);
  return mem;
}

void *hunt_realloc(void *oldMem, size_t size) {
  Guard *guard = (Guard *) oldMem;
  void *newMem;

  if (oldMem == NULL) return hunt_malloc(size);

  guard--;
  if (isOverrun(oldMem)) {
    release_memory(oldMem);
    HUNT_TEST_FAIL(Hunt.CurrentTestLineNumber, "Buffer overrun detected during realloc()");
  }

  if (size == 0) {
    release_memory(oldMem);
    return NULL;
  }

  if (guard->size >= size) return oldMem;

#ifdef HUNT_EXCLUDE_STDLIB_MALLOC /* Optimization if memory is expandable */
  {
      size_t old_total_size = hunt_size_round_up(guard->size + sizeof(end));

      if ((oldMem == hunt_heap + heap_index - old_total_size) &&
          ((heap_index - old_total_size + hunt_size_round_up(size + sizeof(end))) <= HUNT_INTERNAL_HEAP_SIZE_BYTES))
      {
          release_memory(oldMem);    /* Not thread-safe, like hunt_heap generally */
          return hunt_malloc(size); /* No memcpy since data is in place */
      }
  }
#endif
  newMem = hunt_malloc(size);
  if (newMem == NULL) return NULL; /* Do not release old memory */
  memcpy(newMem, oldMem, guard->size);
  release_memory(oldMem);
  return newMem;
}


/*-------------------------------------------------------- */
/*Automatic pointer restoration functions */
struct PointerPair {
  void **pointer;
  void *old_value;
};

static struct PointerPair pointer_store[HUNT_MAX_POINTERS];
static int pointer_index = 0;

void HuntPointer_Init(void) {
  pointer_index = 0;
}

void HuntPointer_Set(void **pointer, void *newValue, HUNT_LINE_TYPE line) {
  if (pointer_index >= HUNT_MAX_POINTERS) {
    HUNT_TEST_FAIL(line, "Too many pointers set");
  } else {
    pointer_store[pointer_index].pointer = pointer;
    pointer_store[pointer_index].old_value = *pointer;
    *pointer = newValue;
    pointer_index++;
  }
}

void HuntPointer_UndoAllSets(void) {
  while (pointer_index > 0) {
    pointer_index--;
    *(pointer_store[pointer_index].pointer) =
        pointer_store[pointer_index].old_value;
  }
}

int HuntGetCommandLineOptions(int argc, const char *argv[]) {
  int i;
  HuntFixture.Verbose = 0;
  HuntFixture.Silent = 0;
  HuntFixture.GroupFilter = 0;
  HuntFixture.NameFilter = 0;
  HuntFixture.RepeatCount = 1;

  if (argc == 1)
    return 0;

  for (i = 1; i < argc;) {
    if (strcmp(argv[i], "-v") == 0) {
      HuntFixture.Verbose = 1;
      i++;
    } else if (strcmp(argv[i], "-s") == 0) {
      HuntFixture.Silent = 1;
      i++;
    } else if (strcmp(argv[i], "-g") == 0) {
      i++;
      if (i >= argc)
        return 1;
      HuntFixture.GroupFilter = argv[i];
      i++;
    } else if (strcmp(argv[i], "-n") == 0) {
      i++;
      if (i >= argc)
        return 1;
      HuntFixture.NameFilter = argv[i];
      i++;
    } else if (strcmp(argv[i], "-r") == 0) {
      HuntFixture.RepeatCount = 2;
      i++;
      if (i < argc) {
        if (*(argv[i]) >= '0' && *(argv[i]) <= '9') {
          unsigned int digit = 0;
          HuntFixture.RepeatCount = 0;
          while (argv[i][digit] >= '0' && argv[i][digit] <= '9') {
            HuntFixture.RepeatCount *= 10;
            HuntFixture.RepeatCount += (unsigned int) argv[i][digit++] - '0';
          }
          i++;
        }
      }
    } else {
      /* ignore unknown parameter */
      i++;
    }
  }
  return 0;
}

void HuntConcludeFixtureTest(void) {
  if (Hunt.CurrentTestIgnored) {
    Hunt.TestIgnores++;
    HUNT_PRINT_EOL();
  } else if (!Hunt.CurrentTestFailed) {
    if (HuntFixture.Verbose) {
      HuntPrint(" ");
      HuntPrint(HuntStrPass);
      HUNT_EXEC_TIME_STOP();
      HUNT_PRINT_EXEC_TIME();
      HUNT_PRINT_EOL();
    }
  } else /* Hunt.CurrentTestFailed */
  {
    Hunt.TestFailures++;
    HUNT_PRINT_EOL();
  }

  Hunt.CurrentTestFailed = 0;
  Hunt.CurrentTestIgnored = 0;
}
