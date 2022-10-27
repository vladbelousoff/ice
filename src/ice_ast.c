#include "ice_ast.h"

iceAstLitT*
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

iceAstIdentT*
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

iceAstExprT*
iceAstParenExpr(iceLexerT* lexer) {
   iceTokenIdT args1[] = {
      ICE_TOKEN_ID_LPAREN,
   };

   iceTokenIdT args2[] = {
      ICE_TOKEN_ID_RPAREN,
   };

   iceTokenT* token;

   token = iceLexerOne(lexer, args1, iceCountOf(args1));
   if (token == NULL) {
      return NULL;
   }

   iceMemTerm(token);

   iceAstExprT* expr = iceAstTerm(lexer);
   if (expr == NULL) {
      return NULL;
   }

   token = iceLexerOne(lexer, args2, iceCountOf(args2));
   if (token == NULL) {
      iceMemTerm(expr);
      return NULL;
   }

   iceMemTerm(token);

   return expr;
}

iceAstExprT*
iceAstTerm(iceLexerT* lexer) {
   iceAstLitT* lit = iceAstLit(lexer);
   if (lit != NULL) {
      iceAstExprT* expr = iceMemInit(sizeof(*expr));
      expr->type = ICE_AST_EXPR_TYPE_LITERAL;
      expr->lit = lit;
      return expr;
   }

   iceAstExprT* parenExpr = iceAstParenExpr(lexer);
   if (parenExpr != NULL) {
      iceAstExprT* expr = iceMemInit(sizeof(*expr));
      expr->type = ICE_AST_EXPR_TYPE_PAREN_EXPR;
      expr->parenExpr = parenExpr;
      return expr;
   }

   iceAstIdentT* ident = iceAstIdent(lexer);
   if (ident != NULL) {
      iceAstExprT* expr = iceMemInit(sizeof(*expr));
      expr->type = ICE_AST_EXPR_TYPE_IDENT;
      expr->ident = ident;
      return expr;
   }

   return NULL;
}