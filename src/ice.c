#include <ice_ast.h>

//static const char source[] = "function main() : int { log.debug('Hello World!'); return 0; }";
//static const char source[] = "variable 100 \n300.1  200  123.75 'Hello'\t_World 'Hello World' 56.";
static const char source[] = "call(11) * (3.2 - 2.4)";

int main(int argc, char* argv[]) {
   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   iceListEntryT* entry;
   iceListEntryT* safe;

   iceListForEachSafe(entry, safe, &lexer.tokens) {
      iceTokenT* token = iceListRecord(entry, iceTokenT, link);
      //printf("++ Token id:\t%-3d\tname: %s\n", token->id, iceTokens[token->id]);
   }

   iceAstExprT* expr = iceAstExpr(&lexer);
   if (expr != NULL) {
      printf("EXPR Type: %d\n", expr->type);
      iceMemTerm(expr);
   }

   iceLexerTerm(&lexer);

   return 0;
}
