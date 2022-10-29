#include "munit/munit.h"
#include "ice_ast.h"

//static const char source[] = "function main() : int { log.debug('Hello World!'); return 0; }";
//static const char source[] = "variable 100 \n300.1  200  123.75 'Hello'\t_World 'Hello World' 56.";

static void*
iceTestParserInit(const MunitParameter params[], void* data) {
   (void) params;
   (void) data;

   iceLexerT* lexer = iceMemInit(sizeof(*lexer));
   return (void*)lexer;
}

static void
iceTestParserTerm(void* data) {
   iceLexerT* lexer = (iceLexerT*)data;
   iceLexerTerm(lexer);
   iceMemTerm(data);
}

static MunitResult
iceTestParser(const MunitParameter params[], void* data) {
   static const char source[] = "call(11) * (3.2 - 2.4)";
   (void) params;

   iceLexerT* lexer = (iceLexerT*)data;
   iceLexerInit(lexer, source);
   iceLexerTokenize(lexer);

   iceAstExprT* expr = iceAstExpr(lexer);
   if (expr != NULL) {
      munit_assert_int(expr->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);
      munit_assert_int(expr->binOp->lhs->type, ==, ICE_AST_EXPR_TYPE_FUNCTION_CALL);
      munit_assert_int(iceListRecord(expr->binOp->lhs->funcCall->args.next, iceAstFuncCallArgT, link)->expr->lit->i32, ==, 11);
      munit_assert_int(expr->binOp->rhs->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);
      munit_assert_float(expr->binOp->rhs->binOp->lhs->lit->f32, ==, 3.2f);
      munit_assert_float(expr->binOp->rhs->binOp->rhs->lit->f32, ==, 2.4f);
      munit_assert_int(expr->binOp->rhs->binOp->type, ==, ICE_AST_BIN_OP_TYPE_SUB);
      iceMemTerm(expr);
   }

   return MUNIT_OK;
}

static MunitTest cases[] = {
   {
      "/parser",
      iceTestParser,
      iceTestParserInit,
      iceTestParserTerm,
      MUNIT_TEST_OPTION_NONE,
      NULL,
   },
   { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
   "/ice",
   cases,
   NULL,
   1,
   MUNIT_SUITE_OPTION_NONE,
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
   return munit_suite_main(&suite, (void*)"ice", argc, argv);
}
