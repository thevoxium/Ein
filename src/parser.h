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

Token peek(Parser *p);
Token previous(Parser *p);
bool is_at_end(Parser *p);
bool check(Parser *p, TokenType tokenType);
Token advance(Parser *p);
bool match(Parser *p, TokenType *types, int count);
Token expect(Parser *p, TokenType tokenType);

#endif // !PARSER_H
