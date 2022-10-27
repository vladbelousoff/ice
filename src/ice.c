#include <ice_ast.h>

//static const char source[] = "function main() : int { log.debug('Hello World!'); return 0; }";
//static const char source[] = "variable 100 \n300.1  200  123.75 'Hello'\t_World 'Hello World' 56.";
static const char source[] = "11 + (32 * 2.4)";

int main(int argc, char* argv[]) {
   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   iceAstExprT* expr = iceAstExpr(&lexer);
   if (expr != NULL) {
      printf("EXPR %d +\n", expr->binOp->lhs->lit->i32);
      printf("EXPR   %d *\n", expr->binOp->rhs->binOp->lhs->lit->i32);
      printf("EXPR      %f\n", expr->binOp->rhs->binOp->rhs->lit->f32);
      iceMemTerm(expr);
   }

   iceLexerTerm(&lexer);

   return 0;
}
