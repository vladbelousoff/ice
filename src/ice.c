#include <ice_lexer.h>

//static const char source[] = "function main() : int { log.debug('Hello World!'); return 0; }";
static const char source[] = "100 200\n300.0   123.75 Hello\t_World 'Hello World' 56.";

int main(int argc, char* argv[])
{
    iceLexerT lexer;
    iceLexerInit(&lexer, source);
    iceLexerTokenize(&lexer);
    iceLexerTerm(&lexer);

    return 0;
}
