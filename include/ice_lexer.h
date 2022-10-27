#pragma once

#include "ice_common.h"
#include "ice_list.h"
#include "ice_log.h"
#include "ice_token.h"

typedef struct iceLexerT {
   const char* source;
   unsigned long sPos;
   unsigned long lPos;
   unsigned long line;
   iceListHeadT tokens;
} iceLexerT;

static void
iceLexerInit(iceLexerT* self, const char source[]) {
   iceListInit(&self->tokens);

   self->source = source;
   self->line = 1;
   self->sPos = 0;
   self->lPos = 0;
}

static void
iceLexerTerm(iceLexerT* self) {
   iceListEntryT* entry;
   iceListEntryT* safe;

   iceListForEachSafe(entry, safe, &self->tokens) {
      iceTokenT* token = iceListRecord(entry, iceTokenT, link);
      iceListRemove(entry);
      iceMemTerm(token);
   }
}

static char
iceGetSymbol(iceLexerT* self) {
   return self->source[self->sPos];
}

static int
iceIsEndOfLine(char symbol) {
   return symbol == '\n';
}

static char
iceEatSymbol(iceLexerT* self) {
   char symbol = self->source[self->sPos++];
   if (iceIsEndOfLine(symbol)) {
      self->lPos = 0;
      self->line++;
   } else {
      self->lPos++;
   }

   return symbol;
}

typedef enum iceSymbolT {
   ICE_SYMBOL_UNKNOWN,
   ICE_SYMBOL_SPACE,
   ICE_SYMBOL_NEWLINE,
   ICE_SYMBOL_DIGIT,
   ICE_SYMBOL_ALPHA,
   ICE_SYMBOL_QUOTE,
   ICE_SYMBOL_SERVICE,
} iceSymbolT;

static int
iceIsEndOfStream(char symbol) {
   return symbol == 0;
}

static int
iceIsSpace(char symbol) {
   return symbol == ' ';
}

static int
iceIsWhitespace(char symbol) {
   return iceIsSpace(symbol) || iceIsEndOfLine(symbol) || symbol == '\t' || symbol == '\r';
}

static int
iceIsAlpha(char symbol) {
   return symbol >= 'A' && symbol <= 'z';
}

static int
iceIsDigit(char symbol) {
   return symbol >= '0' && symbol <= '9';
}

static int
iceIsQuote(char symbol) {
   return symbol == '\'';
}

static int
iceIsServiceSymbol(char symbol) {
   return iceFindSymbol("~!@#$%^&*/%()-+=", symbol) != -1;
}

static iceSymbolT
iceGetSymbolType(char symbol) {
   if (iceIsEndOfLine(symbol))
      return ICE_SYMBOL_NEWLINE;

   if (iceIsWhitespace(symbol))
      return ICE_SYMBOL_SPACE;

   if (iceIsDigit(symbol))
      return ICE_SYMBOL_DIGIT;

   if (iceIsAlpha(symbol))
      return ICE_SYMBOL_ALPHA;

   if (iceIsQuote(symbol))
      return ICE_SYMBOL_QUOTE;

   if (iceIsServiceSymbol(symbol))
      return ICE_SYMBOL_SERVICE;

   return ICE_SYMBOL_UNKNOWN;
}

static int
iceLexerProcessDigit(iceLexerT* self) {
   unsigned long bufPos = 0;
   unsigned long dotCnt = 0;

   char buf[ICE_ID_MAX_LEN];
   memset(buf, 0, ICE_ID_MAX_LEN);

   for (;; iceEatSymbol(self)) {
      char symbol = iceGetSymbol(self);

      if (iceIsWhitespace(symbol)) {
         bufPos++;
         break;
      }

      if (iceIsEndOfLine(symbol)) {
         bufPos++;
         break;
      }

      if (symbol == '_') {
         buf[bufPos++] = symbol;
         continue;
      }

      if (symbol == '-') {
         if (bufPos != 0) {
            return -1;
         }
         buf[bufPos++] = symbol;
         continue;
      }

      if (symbol == '.') {
         if (dotCnt == 1) {
            return -1;
         }
         buf[bufPos++] = symbol;
         dotCnt++;
         continue;
      }

      if (iceIsDigit(symbol)) {
         buf[bufPos++] = symbol;
         continue;
      }

      if (iceIsEndOfStream(symbol)) {
         bufPos++;
         break;
      }
   }

   iceTokenT* token = iceMemInit(sizeof(*token) + bufPos);
   if (token) {
      memcpy(token->buf, buf, bufPos);
      token->bufSize = bufPos;
      token->sPos = self->sPos;
      token->lPos = self->lPos;
      token->line = self->line;
      if (dotCnt == 0) {
         token->id = ICE_TOKEN_ID_INTEGER;
      } else {
         token->id = ICE_TOKEN_ID_REAL;
      }

      iceListPushBack(&self->tokens, &token->link);
      return 0;
   }

   return -1;
}

static int
iceLexerProcessIdentifier(iceLexerT* self) {
   unsigned long bufPos = 0;

   char buf[ICE_ID_MAX_LEN];
   memset(buf, 0, ICE_ID_MAX_LEN);

   for (;; iceEatSymbol(self)) {
      char symbol = iceGetSymbol(self);

      if (symbol == '_' || iceIsAlpha(symbol)) {
         buf[bufPos++] = symbol;
         continue;
      }

      if (iceIsWhitespace(symbol)) {
         bufPos++;
         break;
      }

      if (iceIsEndOfStream(symbol)) {
         bufPos++;
         break;
      }

      return -1;
   }

   iceTokenT* token = iceMemInit(sizeof(*token) + bufPos);
   if (token) {
      memcpy(token->buf, buf, bufPos);
      token->bufSize = bufPos;
      token->sPos = self->sPos;
      token->lPos = self->lPos;
      token->line = self->line;
      token->id = ICE_TOKEN_ID_IDENTIFIER;

      iceListPushBack(&self->tokens, &token->link);
      return 0;
   }

   return -1;
}

static int
iceLexerProcessString(iceLexerT* self) {
   unsigned long bufPos = 0;

   char buf[ICE_ID_MAX_LEN];
   memset(buf, 0, ICE_ID_MAX_LEN);

   iceEatSymbol(self);
   for (;;) {
      char symbol = iceEatSymbol(self);

      if (iceIsQuote(symbol)) {
         bufPos++;
         break;
      }

      if (iceIsEndOfStream(symbol)) {
         return -1;
      }

      buf[bufPos++] = symbol;
   }

   iceTokenT* token = iceMemInit(sizeof(*token) + bufPos);
   if (token) {
      memcpy(token->buf, buf, bufPos);
      token->bufSize = bufPos;
      token->sPos = self->sPos;
      token->lPos = self->lPos;
      token->line = self->line;
      token->id = ICE_TOKEN_ID_STRING;

      iceListPushBack(&self->tokens, &token->link);
      return 0;
   }

   return -1;
}

static int
iceLexerProcessOperator(iceLexerT* self) {
   iceTokenIdT tokenId;

   char symbol = iceEatSymbol(self);
   switch (symbol) {
      case '*':
         tokenId = ICE_TOKEN_ID_MUL;
         break;
      case '/':
         tokenId = ICE_TOKEN_ID_DIV;
         break;
      case '%':
         tokenId = ICE_TOKEN_ID_MOD;
         break;
      default:
         return -1;
   }

   iceTokenT* token = iceMemInit(sizeof(*token));
   if (token) {
      token->id = tokenId;
      token->bufSize = 0;
      token->sPos = self->sPos;
      token->lPos = self->lPos;
      token->line = self->line;

      iceListPushBack(&self->tokens, &token->link);
      return 0;
   }

   return -1;
}

static int
iceLexerProcessSymbol(iceLexerT* self) {
   iceSymbolT symbolType = iceGetSymbolType(iceGetSymbol(self));
   switch (symbolType) {
      case ICE_SYMBOL_UNKNOWN:
         iceLogE("Unknown symbol '%c' line: %d", iceGetSymbol(self), (int)self->line);
         return -1;
      case ICE_SYMBOL_DIGIT:
         return iceLexerProcessDigit(self);
      case ICE_SYMBOL_ALPHA:
         return iceLexerProcessIdentifier(self);
      case ICE_SYMBOL_QUOTE:
         return iceLexerProcessString(self);
      case ICE_SYMBOL_SERVICE:
         return iceLexerProcessOperator(self);
      case ICE_SYMBOL_SPACE:
      case ICE_SYMBOL_NEWLINE:
         iceEatSymbol(self);
         break;
      default:
         break;
   }

   return 0;
}

static void
iceLexerTokenize(iceLexerT* self) {
   while (iceGetSymbol(self) != 0) {
      iceLexerProcessSymbol(self);
   }
}

static iceTokenT*
iceLexerOne(iceLexerT* lexer, iceTokenIdT ids[], int count) {
   iceListEntryT* entry;
   iceListEntryT* safe;

   iceListForEachSafe(entry, safe, &lexer->tokens) {
      iceTokenT* token = iceListRecord(entry, iceTokenT, link);

      //printf(">> Token Type %d\n", token->id);
      //printf(">> Token %s\n\n", token->buf);

      for (int i = 0; i < count; ++i) {
         if (token->id != ids[i]) {
            continue;
         }

         iceListRemove(entry);
         return token;
      }

      break;
   }

   return NULL;
}
