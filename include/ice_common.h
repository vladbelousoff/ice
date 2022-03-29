#pragma once

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ICE_ID_MAX_LEN 32

static int
iceFindSymbol(const char* source, char symbol)
{
    const char* pSymbol = strchr(source, symbol);
    return pSymbol ? (int) (pSymbol - source) : -1;
}

static void*
iceMemInit(unsigned long size)
{
    return malloc(size);
}

static void
iceMemTerm(void* buf)
{
    free(buf);
}
