/*
 * file : project_defines.h
 *
 */

#ifndef _project_defines_H
#define _project_defines_H

#include "project_config.h"
#include <stddef.h> // include for NULL
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
	#define  EXTERN_C_FUNCTION    extern "C"
#else
	#define  EXTERN_C_FUNCTION
#endif

#include <execinfo.h>
#define MAX_STACK_LEVELS  50

// helper-function to print the current stack trace
static void print_stacktrace()
{
  void *buffer[MAX_STACK_LEVELS];
  int levels = backtrace(buffer, MAX_STACK_LEVELS);

  // print to stderr (fd = 2), and remove this function from the trace
  backtrace_symbols_fd(buffer + 1, levels - 1, 2);
}

#define CRITICAL_ERROR(str)   \
	{printf("!!err!! -----%s: %s\n", __FUNCTION__, str);\
	print_stacktrace(); exit(0);}

#endif
