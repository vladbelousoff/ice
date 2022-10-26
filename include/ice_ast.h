#pragma once

#include "ice_common.h"
#include "ice_lexer.h"
#include "ice_list.h"

typedef enum iceAstLitTypeT {
   ICE_AST_LIT_TYPE_I32,
   ICE_AST_LIT_TYPE_F32,
   ICE_AST_LIT_TYPE_BOOLEAN,
   ICE_AST_LIT_TYPE_STRING,
} iceAstLitTypeT;

typedef struct iceAstLitT {
   iceAstLitTypeT type;
   union {
      int i32;
      float f32;
      char* string;
   };
} iceAstLitT;

static iceAstLitT*
iceAstLit(iceLexerT* lexer) {
   iceTokenIdT args[] = {
      ICE_TOKEN_ID_INTEGER,
      ICE_TOKEN_ID_REAL,
      ICE_TOKEN_ID_STRING,
   };

   iceTokenT* token = iceLexerOne(lexer, args, iceCountOf(args));
   if (token == NULL) {
      return NULL;
   }

   iceAstLitT* lit;
   char* endPtr;

   switch (token->id) {
      case ICE_TOKEN_ID_INTEGER:
         lit = iceMemInit(sizeof(*lit));
         lit->type = ICE_AST_LIT_TYPE_I32;
         lit->i32 = (int)strtol(token->buf, &endPtr, 10);
         break;
      case ICE_TOKEN_ID_REAL:
         lit = iceMemInit(sizeof(*lit));
         lit->type = ICE_AST_LIT_TYPE_F32;
         lit->f32 = strtof(token->buf, &endPtr);
         break;
      case ICE_TOKEN_ID_STRING:
         lit = iceMemInit(sizeof(*lit));
         lit->type = ICE_AST_LIT_TYPE_STRING;
         lit->string = strdup(token->buf);
         break;
      default:
         iceMemTerm(token);
         return NULL;
   }

   iceMemTerm(token);
   return lit;
}

typedef struct iceAstIdentT {
   char* val;
} iceAstIdentT;

static iceAstIdentT*
iceAstIdent(iceLexerT* lexer) {
   iceTokenIdT args[] = {
      ICE_TOKEN_ID_IDENTIFIER,
   };

   iceTokenT* token = iceLexerOne(lexer, args, iceCountOf(args));
   if (token == NULL) {
      return NULL;
   }

   iceAstIdentT* ident = iceMemInit(sizeof(*ident));
   ident->val = strdup(token->buf);

   iceMemTerm(token);
   return ident;
}

typedef enum iceAstExprTypeT {
   ICE_AST_EXPR_TYPE_LITERAL,
   ICE_AST_EXPR_TYPE_IDENT,
   ICE_AST_EXPR_TYPE_PAREN_EXPR,
} iceAstExprTypeT;

typedef struct iceAstExprT {
   iceAstExprTypeT type;
   union {
      iceAstLitT* lit;
      struct iceAstExprT* parenExpr;
      iceAstIdentT* ident;
   };
} iceAstExprT;

iceAstExprT* iceAstTerm(iceLexerT* lexer);
iceAstExprT* iceAstParenExpr(iceLexerT* lexer);
