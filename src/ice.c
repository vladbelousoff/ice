#include <ice_ast.h>
#include <ice_lexer.h>

//static const char source[] = "function main() : int { log.debug('Hello World!'); return 0; }";
static const char source[] = "variable 100 \n300.1  200  123.75 'Hello'\t_World 'Hello World' 56.";

int main(int argc, char* argv[]) {
   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   iceAstExprT* expr = iceAstTerm(&lexer);
   if (expr != NULL) {
      printf("EXPR %d\n", expr->type);
      printf("EXPR %s\n", expr->ident->val);
   }

   iceMemTerm(expr);

   for (;;) {
      iceAstLitT* lit = iceAstLit(&lexer);
      if (lit == NULL)
         break;
      switch (lit->type) {
         case ICE_AST_LIT_TYPE_I32:
            printf("%d\n", lit->i32);
            break;
         case ICE_AST_LIT_TYPE_F32:
            printf("%f\n", lit->f32);
            break;
         case ICE_AST_LIT_TYPE_STRING:
            printf("%s\n", lit->string);
            break;
         case ICE_AST_LIT_TYPE_BOOLEAN:
            break;
      }

      iceMemTerm(lit);
   }

   iceLexerTerm(&lexer);

   return 0;
}
