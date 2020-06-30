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


#define CRITICAL_ERROR(str)   \
	{printf("!!err!! -----%s: %s\n", __FUNCTION__, str); exit(0);}

#endif
