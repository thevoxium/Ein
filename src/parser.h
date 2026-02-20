#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct Parser {
  Token *tokens;
  int token_count;
  int current;
} Parser;

Parser *init_parser(Lexer *lexer);
void free_parser(Parser *p);

#endif // !PARSER_H
