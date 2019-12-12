/* Copyright (c) 2010 James Grenning and Contributed to Hunt Project
 * ==========================================
 *  Hunt Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#ifndef HUNT_FIXTURE_INTERNALS_H_
#define HUNT_FIXTURE_INTERNALS_H_

#ifdef __cplusplus
extern "C"
{
#endif

struct HUNT_FIXTURE_T {
  int Verbose;
  int Silent;
  unsigned int RepeatCount;
  const char *NameFilter;
  const char *GroupFilter;
};
extern struct HUNT_FIXTURE_T HuntFixture;

typedef void huntfunction(void);

void HuntTestRunner(huntfunction *setup,
                    huntfunction *testBody,
                    huntfunction *teardown,
                    const char *printableName,
                    const char *group,
                    const char *name,
                    const char *file, unsigned int line);

void HuntIgnoreTest(const char *printableName, const char *group, const char *name);

void HuntMalloc_StartTest(void);

void HuntMalloc_EndTest(void);

int HuntGetCommandLineOptions(int argc, const char *argv[]);

void HuntConcludeFixtureTest(void);

void HuntPointer_Set(void **pointer, void *newValue, HUNT_LINE_TYPE line);

void HuntPointer_UndoAllSets(void);

void HuntPointer_Init(void);

#ifndef HUNT_MAX_POINTERS
#define HUNT_MAX_POINTERS 5
#endif

#ifdef __cplusplus
}
#endif

#endif /* HUNT_FIXTURE_INTERNALS_H_ */
