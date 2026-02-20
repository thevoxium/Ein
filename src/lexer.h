#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum TokenType {
  IDENTIFIER,
  FUNC,
  FOR,
  IN,
  RANGE,
  IF,
  ELSE,
  RETURN,
  TENSOR,
  INT,
  FLOAT,
  EQUAL,
  PLUS_EQUAL,
  MINUS_EQUAL,
  PLUS,
  MINUS,
  STAR,
  EQUAL_EQUAL,
  BANG_EQUAL,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL,
  AND,
  OR,
  BANG,
  ARROW,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  COLON,
  SEMICOLON,
  UNKNOWN,
  LEFT_BRACKET,
  RIGHT_BRACKET,
} TokenType;

typedef struct Token {
  TokenType tokenType;
  char *literal;
  int line;
} Token;

typedef struct Lexer {
  int total_length;
  int position;
  int line;
  char *input;

  Token *token_list;
  int token_count;
  int token_capacity;

} Lexer;

Lexer *init_lexer(char *input, int total_length);
void free_lexer(Lexer *lexer);

bool isalphanumeric(char *str);
bool isint(const char *str);
bool isfloat(const char *str);
void scan(Lexer *lexer);
void skip_whitespace(Lexer *lexer);
bool grow_lexer(Lexer *lexer);
Token *create_token(const char *literal, TokenType tokenType, int line);
void add_token(Lexer *lexer, Token *token);
void print_tokens(Lexer *lexer);

#endif // !LEXER_H
