#include "parser.h"

Parser *init_parser(Lexer *lexer) {
  Parser *p = (Parser *)malloc(sizeof(Parser));
  if (p == NULL || lexer == NULL)
    return NULL;

  p->current = 0;
  p->token_count = lexer->token_count;
  p->tokens = (Token *)malloc(sizeof(Token) * lexer->token_count);
  if (p->tokens == NULL) {
    free(p);
    return NULL;
  }

  for (int i = 0; i < lexer->token_count; i++) {
    p->tokens[i] = lexer->token_list[i];
  }

  return p;
}

void free_parser(Parser *p) {
  if (p == NULL)
    return;

  free(p->tokens);
  free(p);
}
