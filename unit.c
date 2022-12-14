#include "ice_ast.h"
#include "munit/munit.h"

//static const char source[] = "function main() : int { log.debug('Hello World!'); return 0; }";
//static const char source[] = "variable 100 \n300.1  200  123.75 'Hello'\t_World 'Hello World' 56.";

static void*
iceTestParserInit(const MunitParameter params[], void* data) {
   (void)params;
   (void)data;

   return NULL;
}

static void
iceTestParserTerm(void* data) {
   (void)data;
}

static MunitResult
iceTestCompoundExpressionInsideFuncCall(const MunitParameter params[], void* data) {
   (void)params;
   (void)data;

   static const char source[] = "functionCall(( (x) + ( y ) * 777 ))";

   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   iceAstExprT* expr = iceAstExpr(&lexer);
   if (expr != NULL) {
      iceMemTerm(expr);
      return MUNIT_OK;
   }

   return MUNIT_FAIL;
}

static MunitResult
iceTestCompoundExpressionWithManyBinaryOperators(const MunitParameter params[], void* data) {
   (void)params;
   (void)data;

   static const char source[] = "functionCall(( (x) + ( y ) * 777 - 1000 * ( 5 - 6 ) - 7 + 8 * 999 ))";

   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   iceAstExprT* expr = iceAstExpr(&lexer);
   if (expr != NULL) {
      munit_assert_int(expr->type, ==, ICE_AST_EXPR_TYPE_FUNCTION_CALL);
      iceAstFuncCallT* funcCall = expr->funcCall;
      munit_assert_not_null(funcCall);

      iceAstFuncCallArgT* arg = iceListRecord(funcCall->args.next, iceAstFuncCallArgT, link);
      iceAstExprT* exprArg = arg->expr;

      munit_assert_int(exprArg->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);
      iceAstBinOpT* expr7add8mul999 = exprArg->binOp;

      munit_assert_int(expr7add8mul999->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);

      munit_assert_int(expr7add8mul999->rhs->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);
      iceAstBinOpT* expr8mul999 = expr7add8mul999->rhs->binOp;

      //munit_assert_int(expr7add8mul999->lhs->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);
      //iceAstBinOpT* expr7add8mul999lhs = expr7add8mul999->lhs->binOp;

      munit_assert_int(expr8mul999->lhs->lit->i32, ==, 8);
      munit_assert_int(expr8mul999->rhs->lit->i32, ==, 999);

      iceMemTerm(expr);
      return MUNIT_OK;
   }

   return MUNIT_FAIL;
}

static MunitResult
iceTestPrettySimpleExpression(const MunitParameter params[], void* data) {
   (void)params;
   (void)data;

   static const char source[] = "call(11) * (3.2 - 2.4)";

   iceLexerT lexer;
   iceLexerInit(&lexer, source);
   iceLexerTokenize(&lexer);

   iceAstExprT* expr = iceAstExpr(&lexer);
   if (expr != NULL) {
      munit_assert_int(expr->type, ==, ICE_AST_EXPR_TYPE_BINARY_OPERATOR);
      munit_assert_int(expr->binOp->lhs->type, ==, ICE_AST_EXPR_TYPE_FUNCTION_CALL);
      munit_assert_string_equal(expr->binOp->lhs->funcCall->ident->val, "call");
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
      "/parse_1",
      iceTestPrettySimpleExpression,
      iceTestParserInit,
      iceTestParserTerm,
      MUNIT_TEST_OPTION_NONE,
      NULL,
   },
   {
      "/parse_2",
      iceTestCompoundExpressionInsideFuncCall,
      iceTestParserInit,
      iceTestParserTerm,
      MUNIT_TEST_OPTION_NONE,
      NULL,
   },
#if 0
   {
      "/parse_3",
      iceTestCompoundExpressionWithManyBinaryOperators,
      iceTestParserInit,
      iceTestParserTerm,
      MUNIT_TEST_OPTION_NONE,
      NULL,
   },
#endif
   {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

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
