#ifndef HUNT_OUTPUT_H
#define HUNT_OUTPUT_H

#ifndef NULL
#ifndef __cplusplus
#define NULL (void*)0
#else
#define NULL 0
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void output_char(int a);

void output_flush(void);

void output_start(unsigned int baudrate);

void output_complete(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* HUNT_OUTPUT_H */