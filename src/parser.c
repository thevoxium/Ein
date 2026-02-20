#include "parser.h"
#include "lexer.h"

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

Token peek(Parser *p) { return p->tokens[p->current]; }

Token previous(Parser *p) {
  assert(p->current > 0);
  return p->tokens[p->current - 1];
}

bool is_at_end(Parser *p) { return (p->current >= p->token_count); }

bool check(Parser *p, TokenType tokenType) {
  if (is_at_end(p))
    return false;
  return (p->tokens[p->current].tokenType == tokenType);
}

Token advance(Parser *p) {
  Token t = p->tokens[p->current];
  p->current++;
  return t;
}

bool match(Parser *p, TokenType *types, int count) {
  for (int i = 0; i < count; i++) {
    if (check(p, types[i])) {
      advance(p);
      return true;
    }
  }
  return false;
}

Token expect(Parser *p, TokenType tokenType) {
  if (check(p, tokenType)) {
    return advance(p);
  }

  fprintf(stderr, "Parse error at line %d: expected token type %d, got '%s'\n",
          peek(p).line, tokenType, peek(p).literal);
  exit(1);
}
