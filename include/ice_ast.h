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

typedef enum iceAstBinOpTypeT {
   ICE_AST_BIN_OP_TYPE_EQUAL,
   ICE_AST_BIN_OP_TYPE_NOT_EQUAL,
   ICE_AST_BIN_OP_TYPE_ADD,
   ICE_AST_BIN_OP_TYPE_SUB,
   ICE_AST_BIN_OP_TYPE_MUL,
   ICE_AST_BIN_OP_TYPE_DIV,
   ICE_AST_BIN_OP_TYPE_MOD,
} iceAstBinOpTypeT;

struct iceAstExprT;

typedef struct iceAstBinOpT {
   iceAstBinOpTypeT type;
   struct iceAstExprT* lhs;
   struct iceAstExprT* rhs;
} iceAstBinOpT;

typedef enum iceAstExprTypeT {
   ICE_AST_EXPR_TYPE_LITERAL,
   ICE_AST_EXPR_TYPE_IDENT,
   ICE_AST_EXPR_TYPE_PAREN_EXPR,
   ICE_AST_EXPR_TYPE_BINARY_OPERATOR,
   ICE_AST_EXPR_TYPE_FUNCTION_CALL,
   ICE_AST_EXPR_TYPE_NEW,
} iceAstExprTypeT;

typedef struct iceAstExprT {
   iceAstExprTypeT type;
   union {
      iceAstLitT* lit;
      struct iceAstExprT* parenExpr;
      iceAstIdentT* ident;
      iceAstBinOpT* binOp;
   };
} iceAstExprT;

iceAstLitT* iceAstLit(iceLexerT* lexer);
iceAstIdentT* iceAstIdent(iceLexerT* lexer);
iceAstExprT* iceAstTerm(iceLexerT* lexer);
iceAstExprT* iceAstParenExpr(iceLexerT* lexer);
iceAstExprT* iceAstFactor(iceLexerT* lexer);
