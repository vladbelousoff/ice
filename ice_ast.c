#include "ice_ast.h"

iceAstLitT*
iceAstLit(iceLexerT* lexer) {
   iceTokenIdT args[] = {
      ICE_TOKEN_ID_INTEGER,
      ICE_TOKEN_ID_REAL,
      ICE_TOKEN_ID_STRING,
   };

   iceTokenT* token = iceLexerGetAnyOf(lexer, args, iceCountOf(args));
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
         lit->string = _strdup(token->buf);
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
   iceTokenT* token = iceLexerGetExact(lexer, ICE_TOKEN_ID_IDENTIFIER);
   if (token == NULL) {
      return NULL;
   }

   iceAstIdentT* ident = iceMemInit(sizeof(*ident));
   ident->val = _strdup(token->buf);

   iceMemTerm(token);
   return ident;
}

iceAstExprT*
iceAstParenExpr(iceLexerT* lexer) {
   iceTokenT* token;

   token = iceLexerGetExact(lexer, ICE_TOKEN_ID_LPAREN);
   if (token == NULL) {
      return NULL;
   }

   iceMemTerm(token);

   iceAstExprT* expr = iceAstExpr(lexer);
   if (expr == NULL) {
      return NULL;
   }

   token = iceLexerGetExact(lexer, ICE_TOKEN_ID_RPAREN);
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
      return parenExpr;
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

iceAstExprT*
iceAstFactor(iceLexerT* lexer) {
   iceAstExprT* lhs = iceAstTerm(lexer);
   if (lhs == NULL) {
      return NULL;
   }

   if (lhs->type == ICE_AST_EXPR_TYPE_IDENT) {
      iceAstFuncCallT* funcCall = iceAstFuncCall(lexer);
      if (funcCall != NULL) {
         funcCall->ident = lhs->ident;
         lhs->type = ICE_AST_EXPR_TYPE_FUNCTION_CALL;
         lhs->funcCall = funcCall;
      }
   }

   iceTokenIdT args[] = {
      ICE_TOKEN_ID_MUL,
      ICE_TOKEN_ID_DIV,
      ICE_TOKEN_ID_MOD,
   };

   iceTokenT* op = iceLexerGetAnyOf(lexer, args, iceCountOf(args));
   if (op == NULL) {
      return lhs;
   }

   iceAstExprT* rhs = iceAstExpr(lexer);
   assert(rhs != NULL);

   iceAstBinOpT* binOp = iceMemInit(sizeof(*binOp));

   switch (op->id) {
      case ICE_TOKEN_ID_MUL:
         binOp->type = ICE_AST_BIN_OP_TYPE_MUL;
         break;
      case ICE_TOKEN_ID_DIV:
         binOp->type = ICE_AST_BIN_OP_TYPE_DIV;
         break;
      case ICE_TOKEN_ID_MOD:
         binOp->type = ICE_AST_BIN_OP_TYPE_MOD;
         break;
      default:
         assert(0 && "Unknown operator!");
   }

   iceMemTerm(op);

   binOp->lhs = lhs;
   binOp->rhs = rhs;

   iceAstExprT* factor = iceMemInit(sizeof(*factor));
   factor->type = ICE_AST_EXPR_TYPE_BINARY_OPERATOR;
   factor->binOp = binOp;

   return factor;
}

iceAstExprT* iceAstExpr(iceLexerT* lexer) {
   iceAstExprT* lhs = iceAstFactor(lexer);
   if (lhs == NULL) {
      return NULL;
   }

   iceTokenIdT args[] = {
      ICE_TOKEN_ID_ADD,
      ICE_TOKEN_ID_SUB,
   };

   iceTokenT* op = iceLexerGetAnyOf(lexer, args, iceCountOf(args));
   if (op == NULL) {
      return lhs;
   }

   iceAstExprT* rhs = iceAstFactor(lexer);
   assert(rhs != NULL);

   iceAstBinOpT* binOp = iceMemInit(sizeof(*binOp));

   switch (op->id) {
      case ICE_TOKEN_ID_ADD:
         binOp->type = ICE_AST_BIN_OP_TYPE_ADD;
         break;
      case ICE_TOKEN_ID_SUB:
         binOp->type = ICE_AST_BIN_OP_TYPE_SUB;
         break;
      default:
         assert(0 && "Unknown operator!");
   }

   iceMemTerm(op);

   binOp->lhs = lhs;
   binOp->rhs = rhs;

   iceAstExprT* expr = iceMemInit(sizeof(*expr));
   expr->type = ICE_AST_EXPR_TYPE_BINARY_OPERATOR;
   expr->binOp = binOp;

   return expr;
}

iceAstFuncCallT* iceAstFuncCall(iceLexerT* lexer) {
   iceTokenT* token = iceLexerGetExact(lexer, ICE_TOKEN_ID_LPAREN);
   if (token == NULL) {
      return NULL;
   }

   iceAstFuncCallT* args = iceMemInit(sizeof(*args));
   iceListInit(&args->args);

   for (;;) {
      token = iceLexerGetExact(lexer, ICE_TOKEN_ID_RPAREN);
      if (token != NULL) {
         iceMemTerm(token);
         break;
      }

      iceMemTerm(token);

      iceAstExprT* expr = iceAstExpr(lexer);
      assert(expr != NULL);

      iceAstFuncCallArgT* arg = iceMemInit(sizeof(*arg));
      arg->expr = expr;

      iceListPushBack(&args->args, &arg->link);
   }

   return args;
}
