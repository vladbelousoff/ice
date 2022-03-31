#include <ice_lexer.h>

static const char source[] = "100 200\n300.0  123.75 Hello\t_World 'Hello World' 56.";

static struct {
   const char* const data;
   iceTokenIdT id;
} result[] = {
   {"100", ICE_TOKEN_ID_INTEGER},
   {"200", ICE_TOKEN_ID_INTEGER},
   {"300.0", ICE_TOKEN_ID_REAL},
   {"123.75", ICE_TOKEN_ID_REAL},
   {"Hello", ICE_TOKEN_ID_IDENTIFIER},
   {"_World", ICE_TOKEN_ID_IDENTIFIER},
   {"Hello World", ICE_TOKEN_ID_STRING},
   {"56.", ICE_TOKEN_ID_REAL},
};

int main(void) {
   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   unsigned long idx = 0;

   iceListEntryT* entry;
   iceListForEach(entry, &lexer.tokens) {
      iceTokenT* token = iceListRecord(entry, iceTokenT, link);
      if (token->id != result[idx].id) {
         return -1;
      }
      if (strcmp(token->buf, result[idx].data) != 0) {
         return -1;
      }

      idx++;
   }

   iceLexerTerm(&lexer);
   return 0;
}