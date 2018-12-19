//
//  snprintf.h
//  SXXOS
//
//  Created by SXX on 26/11/2017.
//  Copyright © 2017 SXX. All rights reserved.
//

#ifndef snprintf_h
#define snprintf_h

#include <types.h>
#include <stdarg.h>

u32 vsprintf(char *str, const char *fmt, va_list args);

u32 vsnprintf(char *str, u32 count, const char *fmt, va_list args);

int vasprintf(char **ptr, const char *format, va_list ap);

u32 sprintf(char *str, const char *fmt, ...);

u32 snprintf(char *str, u32 size, const char *format, ...);

int printf(const char *fmt, ...);

#endif /* snprintf_h */
