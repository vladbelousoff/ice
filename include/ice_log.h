#pragma once

#include <stdio.h>
#include <string.h>

#ifndef ICE_DEBUG_LEVEL
#define ICE_DEBUG_LEVEL 0
#endif

#ifdef _WIN32
#define ICE_SEPARATOR '\\'
#else
#define ICE_SEPARATOR '/'
#endif

#define ICE_FILENAME ((char*)(strrchr(__FILE__, ICE_SEPARATOR) + 1)) 

#define iceLog printf

#define __LogPrint(lvl, file, line, func, fmt, ...)\
    iceLog("[%s] %s:%-3u (%s)" fmt, lvl, file, line, func, ##__VA_ARGS__)

#if ICE_DEBUG_LEVEL >= 4
#define iceLogI iceLog
#else
#define iceLogI(_fmt, ...) do {} while (0)
#endif

#if ICE_DEBUG_LEVEL >= 3
#define iceLogD(_fmt, ...)\
    __LogPrint("D", ICE_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define iceLogD(_fmt, ...) do {} while (0)
#endif

#if ICE_DEBUG_LEVEL >= 2
#define iceLogW(_fmt, ...)\
    __LogPrint("W", ICE_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define iceLogW(_fmt, ...) do {} while (0)
#endif

#if ICE_DEBUG_LEVEL >= 1
#define iceLogE(_fmt, ...)\
    __LogPrint("E", ICE_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define iceLogE(_fmt, ...) do {} while (0)
#endif
