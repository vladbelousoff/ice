#include "ice_ast.h"

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