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

typedef struct iceAstIdentT {
   char* val;
} iceAstIdentT;

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

iceAstLitT* iceAstLit(iceLexerT* lexer);
iceAstIdentT* iceAstIdent(iceLexerT* lexer);
iceAstExprT* iceAstTerm(iceLexerT* lexer);
iceAstExprT* iceAstParenExpr(iceLexerT* lexer);
