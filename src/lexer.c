#include "lexer.h"

bool isalphanumeric(char *str) {
  if (str == NULL || *str == '\0') {
    return false;
  }
  while (*str) {
    char c = *str;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9'))) {
      return false;
    }
    str++;
  }
  return true;
}

bool isint(const char *str) {
  if (str == NULL || *str == '\0') {
    return false;
  }
  if (*str == '+' || *str == '-') {
    str++;
  }
  if (*str == '\0') {
    return false;
  }
  while (*str) {
    if (*str < '0' || *str > '9') {
      return false;
    }
    str++;
  }
  return true;
}

bool isfloat(const char *str) {
  if (str == NULL || *str == '\0')
    return false;

  if (*str == '+' || *str == '-')
    str++;

  bool has_digit = false;
  bool has_dot = false;
  while (*str) {
    if (*str >= '0' && *str <= '9') {
      has_digit = true;
    } else if (*str == '.') {
      if (has_dot)
        return false;
      has_dot = true;
    } else {
      return false;
    }
    str++;
  }

  return has_digit;
}

Lexer *init_lexer(char *input, int total_length) {
  if (input == NULL || total_length < 0) {
    return NULL;
  }

  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  if (lexer == NULL) {
    return NULL;
  }

  lexer->input = input;
  lexer->total_length = total_length;
  lexer->line = 0;
  lexer->position = 0;

  lexer->token_capacity = 50;
  lexer->token_list = (Token *)malloc(lexer->token_capacity * sizeof(Token));
  if (lexer->token_list == NULL) {
    free(lexer);
    return NULL;
  }
  lexer->token_count = 0;

  return lexer;
}

void free_lexer(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }

  free(lexer->token_list);
  free(lexer);
}

void skip_whitespace(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }
  while (lexer->position < lexer->total_length &&
         lexer->input[lexer->position] == ' ') {
    lexer->position++;
  }
}

void grow_lexer(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }
  lexer->token_capacity = 2 * lexer->token_capacity;
  lexer->token_list =
      realloc(lexer->token_list, lexer->token_capacity * sizeof(Token));
  if (lexer->token_list == NULL) {
    free_lexer(lexer);
    return;
  }
}
Token *create_token(char *literal, TokenType tokenType, int line) {
  Token *t = (Token *)malloc(sizeof(Token));
  if (!t)
    return NULL;
  t->line = line;
  t->tokenType = tokenType;
  t->literal = literal;
  return t;
}

void add_token(Lexer *lexer, Token *token) {
  if (!lexer || !token) {
    return;
  }
  if (lexer->token_count >= lexer->token_capacity) {
    grow_lexer(lexer);
  }
  lexer->token_list[lexer->token_count++] = *token;
}

void scan(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }

  Token *t = NULL;

  while (lexer->position < lexer->total_length &&
         lexer->input[lexer->position] != '\0') {
    skip_whitespace(lexer);
    t = NULL;

    char c = lexer->input[lexer->position];
    switch (c) {
    case '(':
      t = create_token("(", LEFT_PAREN, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case ')':
      t = create_token(")", RIGHT_PAREN, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case '{':
      t = create_token("{", LEFT_BRACE, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case '}':
      t = create_token("}", RIGHT_BRACE, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case ',':
      t = create_token(",", COMMA, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case ':':
      t = create_token(":", COLON, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case '*':
      t = create_token("*", STAR, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case ';':
      t = create_token(";", SEMICOLON, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    case '-':
      if (lexer->position + 1 < lexer->total_length &&
          lexer->input[lexer->position + 1] == '>') {
        t = create_token("->", ARROW, lexer->line);
        lexer->position += 2;
      } else if (lexer->position + 1 < lexer->total_length &&
                 lexer->input[lexer->position + 1] == '=') {
        t = create_token("-=", MINUS_EQUAL, lexer->line);
        lexer->position += 2;
      } else {
        t = create_token("-", MINUS, lexer->line);
        lexer->position++;
      }
      add_token(lexer, t);
      break;
    case '+':
      if (lexer->position + 1 < lexer->total_length &&
          lexer->input[lexer->position + 1] == '=') {
        t = create_token("+=", PLUS_EQUAL, lexer->line);
        lexer->position += 2;
      } else {
        t = create_token("+", PLUS, lexer->line);
        lexer->position++;
      }
      add_token(lexer, t);
      break;
    case '=':
      if (lexer->position + 1 < lexer->total_length &&
          lexer->input[lexer->position + 1] == '=') {
        t = create_token("==", EQUAL_EQUAL, lexer->line);
        lexer->position += 2;
      } else {
        t = create_token("=", EQUAL, lexer->line);
        lexer->position++;
      }
      add_token(lexer, t);
      break;
    case '<':
      if (lexer->position + 1 < lexer->total_length &&
          lexer->input[lexer->position + 1] == '=') {
        t = create_token("<=", LESS_EQUAL, lexer->line);
        lexer->position += 2;
      } else {
        t = create_token("<", LESS, lexer->line);
        lexer->position++;
      }
      add_token(lexer, t);
      break;
    case '>':
      if (lexer->position + 1 < lexer->total_length &&
          lexer->input[lexer->position + 1] == '=') {
        t = create_token(">=", GREATER_EQUAL, lexer->line);
        lexer->position += 2;
      } else {
        t = create_token(">", GREATER, lexer->line);
        lexer->position++;
      }
      add_token(lexer, t);
      break;
    case '!':
      if (lexer->position + 1 < lexer->total_length &&
          lexer->input[lexer->position + 1] == '=') {
        t = create_token("!=", BANG_EQUAL, lexer->line);
        lexer->position += 2;
      } else {
        t = create_token("!", BANG, lexer->line);
        lexer->position++;
      }
      add_token(lexer, t);
      break;
    default: {
      int current_position = lexer->position;
      while (current_position < lexer->total_length &&
             lexer->input[current_position] != ' ') {
        char c = lexer->input[current_position];
        if (c == '+' || c == '-' || c == '(' || c == ')' || c == '{' ||
            c == '}' || c == '!' || c == '>' || c == '<' || c == ',' ||
            c == ':' || c == ';' || c == '=' || c == '*') {
          break;
        }

        current_position++;
      }

      if (current_position == lexer->position) {
        lexer->position++;
        break;
      }

      char *literal = strndup(lexer->input + lexer->position,
                              current_position - lexer->position);
      if (literal == NULL) {
        return;
      }

      if (strcmp(literal, "func") == 0) {
        t = create_token(literal, FUNC, lexer->line);
      } else if (strcmp(literal, "for") == 0) {
        t = create_token(literal, FOR, lexer->line);
      } else if (strcmp(literal, "if") == 0) {
        t = create_token(literal, IF, lexer->line);
      } else if (strcmp(literal, "else") == 0) {
        t = create_token(literal, ELSE, lexer->line);
      } else if (strcmp(literal, "return") == 0) {
        t = create_token(literal, RETURN, lexer->line);
      } else if (strcmp(literal, "tensor") == 0) {
        t = create_token(literal, TENSOR, lexer->line);
      } else if (strcmp(literal, "OR") == 0) {
        t = create_token(literal, OR, lexer->line);
      } else if (strcmp(literal, "and") == 0) {
        t = create_token(literal, AND, lexer->line);
      } else if (isalphanumeric(literal)) {
        t = create_token(literal, IDENTIFIER, lexer->line);
      } else if (isint(literal)) {
        t = create_token(literal, INT, lexer->line);
      } else if (isfloat(literal)) {
        t = create_token(literal, FLOAT, lexer->line);
      } else {
        t = create_token(literal, UNKNOWN, lexer->line);
      }

      if (t != NULL) {
        add_token(lexer, t);
      } else {
        free(literal);
      }

      lexer->position = current_position;
    } break;
    }
  }
}

void print_tokens(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }
  if (lexer->token_list == NULL || lexer->token_count == 0) {
    printf("No tokens to print.\n");
    return;
  }
  char *token_type_names[] = {
      "IDENTIFIER",  "FUNC",    "FOR",           "IN",          "RANGE",
      "IF",          "ELSE",    "RETURN",        "TENSOR",      "INT",
      "FLOAT",       "EQUAL",   "PLUS_EQUAL",    "MINUS_EQUAL", "PLUS",
      "MINUS",       "STAR",    "EQUAL_EQUAL",   "BANG_EQUAL",  "LESS",
      "LESS_EQUAL",  "GREATER", "GREATER_EQUAL", "AND",         "OR",
      "BANG",        "ARROW",   "LEFT_PAREN",    "RIGHT_PAREN", "LEFT_BRACE",
      "RIGHT_BRACE", "COMMA",   "COLON",         "SEMICOLON",   "UNKNOWN"};
  size_t token_type_count =
      sizeof(token_type_names) / sizeof(token_type_names[0]);
  for (int i = 0; i < lexer->token_count; i++) {
    char *literal = lexer->token_list[i].literal != NULL
                        ? lexer->token_list[i].literal
                        : "NULL";
    TokenType token_type = lexer->token_list[i].tokenType;
    char *token_type_name =
        (token_type >= 0 && (size_t)token_type < token_type_count)
            ? token_type_names[token_type]
            : "INVALID_TOKEN_TYPE";
    printf("literal: %s | type: %s\n", literal, token_type_name);
  }
}
