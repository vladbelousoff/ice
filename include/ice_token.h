#pragma once

#include "ice_list.h"

typedef enum iceTokenIdT {
   ICE_TOKEN_ID_UNKNOWN,   // 0
   ICE_TOKEN_ID_FUNCTION,  // 1
   ICE_TOKEN_ID_LBRACE,    // 2
   ICE_TOKEN_ID_RBRACE,    // 3
   ICE_TOKEN_ID_LPAREN,    // 4
   ICE_TOKEN_ID_RPAREN,    // 5
   ICE_TOKEN_ID_LBRACKET,  // 6
   ICE_TOKEN_ID_RBRACKET,  // 7
   ICE_TOKEN_ID_IDENTIFIER,// 8
   ICE_TOKEN_ID_INTEGER,   // 9
   ICE_TOKEN_ID_REAL,      // 10
   ICE_TOKEN_ID_STRING,    // 11
   ICE_TOKEN_ID_MUL,       // 11
   ICE_TOKEN_ID_DIV,       // 11
   ICE_TOKEN_ID_MOD,       // 11
   ICE_TOKEN_ID_ADD,       // 11
   ICE_TOKEN_ID_SUB,       // 11
} iceTokenIdT;

static char* iceTokens[] = {
   "",
   "function",
   "{",
   "}",
   "(",
   ")",
   "[",
   "]",
   "",
   "",
   "",
   "",
   "*",
   "/",
   "%",
   "+",
   "-",
   0,
};

typedef struct iceTokenT {
   iceListEntryT link;
   iceTokenIdT id;
   unsigned long line;
   unsigned long sPos;
   unsigned long lPos;
   unsigned long bufSize;
   char buf[0];
} iceTokenT;
