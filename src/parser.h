#ifndef PARSER_H
#define PARSER_H

#include "assert.h"
#include "ast.h"
#include "lexer.h"

typedef struct Parser {
  Token *tokens;
  int token_count;
  int current;
} Parser;

Parser *init_parser(Lexer *lexer);
void free_parser(Parser *p);

bool is_at_end(Parser *p);
bool check(Parser *p, TokenType tokenType);
bool check_next(Parser *p, TokenType tokenType);
Token peek(Parser *p);
Token previous(Parser *p);
Token advance(Parser *p);
Token expect(Parser *p, TokenType tokenType);

ASTNode *parse_primary(Parser *p);
ASTNode *parse_postfix(Parser *p);
ASTNode *parse_unary(Parser *p);
ASTNode *parse_factor(Parser *p);
ASTNode *parse_term(Parser *p);
ASTNode *parse_comparison(Parser *p);
ASTNode *parse_equality(Parser *p);
ASTNode *parse_logic_and(Parser *p);
ASTNode *parse_logic_or(Parser *p);
ASTNode *parse_expression(Parser *p);
ASTNode *parse_type(Parser *p);
ASTNode *parse_return_stmt(Parser *p);
ASTNode *parse_var_decl(Parser *p);
ASTNode *parse_assignment_or_expr(Parser *p);
ASTNode *parse_stmt(Parser *p);
ASTNode *parse_block(Parser *p);
#endif // !PARSER_H
