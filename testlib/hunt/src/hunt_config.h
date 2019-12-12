#ifndef HUNT_CONFIG_H
#define HUNT_CONFIG_H

#include "hunt_output.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define HUNT_OUTPUT_CHAR(a)    output_char(a)
#define HUNT_OUTPUT_FLUSH()    output_flush()
#define HUNT_OUTPUT_START()    output_start(115200)
#define HUNT_OUTPUT_COMPLETE() output_complete()


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* HUNT_CONFIG_H */
