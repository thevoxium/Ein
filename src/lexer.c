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

  for (int i = 0; i < lexer->token_count; i++) {
    free(lexer->token_list[i].literal);
  }
  free(lexer->token_list);
  free(lexer);
}

void skip_whitespace(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }
  while (lexer->position < lexer->total_length) {
    char c = lexer->input[lexer->position];
    if (c == ' ' || c == '\t' || c == '\r') {
      lexer->position++;
    } else if (c == '\n') {
      lexer->line++;
      lexer->position++;
    } else {
      break;
    }
  }
}

bool grow_lexer(Lexer *lexer) {
  if (lexer == NULL) {
    return false;
  }
  int new_capacity = 2 * lexer->token_capacity;
  Token *new_list = realloc(lexer->token_list, new_capacity * sizeof(Token));
  if (new_list == NULL) {
    return false;
  }
  lexer->token_list = new_list;
  lexer->token_capacity = new_capacity;
  return true;
}
Token *create_token(const char *literal, TokenType tokenType, int line) {
  Token *t = (Token *)malloc(sizeof(Token));
  if (!t)
    return NULL;
  t->literal = strdup(literal);
  if (!t->literal) {
    free(t);
    return NULL;
  }
  t->line = line;
  t->tokenType = tokenType;
  return t;
}

void add_token(Lexer *lexer, Token *token) {
  if (!lexer || !token) {
    return;
  }
  if (lexer->token_count >= lexer->token_capacity) {
    if (!grow_lexer(lexer)) {
      free(token->literal);
      free(token);
      return;
    }
  }
  lexer->token_list[lexer->token_count++] = *token;
  free(token);
}

void scan(Lexer *lexer) {
  if (lexer == NULL) {
    return;
  }

  Token *t = NULL;

  while (lexer->position < lexer->total_length &&
         lexer->input[lexer->position] != '\0') {
    skip_whitespace(lexer);
    if (lexer->position >= lexer->total_length ||
        lexer->input[lexer->position] == '\0') {
      break;
    }
    t = NULL;

    char c = lexer->input[lexer->position];
    switch (c) {
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
    case ':':
    case '*':
    case ';': {
      char *literal = NULL;
      TokenType token_type = UNKNOWN;

      switch (c) {
      case '(':
        literal = "(";
        token_type = LEFT_PAREN;
        break;
      case ')':
        literal = ")";
        token_type = RIGHT_PAREN;
        break;
      case '[':
        literal = "[";
        token_type = LEFT_BRACKET;
        break;
      case ']':
        literal = "]";
        token_type = RIGHT_BRACKET;
        break;
      case '{':
        literal = "{";
        token_type = LEFT_BRACE;
        break;
      case '}':
        literal = "}";
        token_type = RIGHT_BRACE;
        break;
      case ',':
        literal = ",";
        token_type = COMMA;
        break;
      case ':':
        literal = ":";
        token_type = COLON;
        break;
      case '*':
        literal = "*";
        token_type = STAR;
        break;
      default:
        literal = ";";
        token_type = SEMICOLON;
        break;
      }

      t = create_token(literal, token_type, lexer->line);
      add_token(lexer, t);
      lexer->position++;
      break;
    }
    case '-':
    case '+':
    case '=':
    case '<':
    case '>':
    case '!': {
      char next = '\0';
      if (lexer->position + 1 < lexer->total_length) {
        next = lexer->input[lexer->position + 1];
      }

      int advance = 1;
      char *literal = NULL;
      TokenType token_type = UNKNOWN;

      if (c == '-' && next == '>') {
        literal = "->";
        token_type = ARROW;
        advance = 2;
      } else if (next == '=') {
        advance = 2;
        switch (c) {
        case '-':
          literal = "-=";
          token_type = MINUS_EQUAL;
          break;
        case '+':
          literal = "+=";
          token_type = PLUS_EQUAL;
          break;
        case '=':
          literal = "==";
          token_type = EQUAL_EQUAL;
          break;
        case '<':
          literal = "<=";
          token_type = LESS_EQUAL;
          break;
        case '>':
          literal = ">=";
          token_type = GREATER_EQUAL;
          break;
        default:
          literal = "!=";
          token_type = BANG_EQUAL;
          break;
        }
      } else {
        switch (c) {
        case '-':
          literal = "-";
          token_type = MINUS;
          break;
        case '+':
          literal = "+";
          token_type = PLUS;
          break;
        case '=':
          literal = "=";
          token_type = EQUAL;
          break;
        case '<':
          literal = "<";
          token_type = LESS;
          break;
        case '>':
          literal = ">";
          token_type = GREATER;
          break;
        default:
          literal = "!";
          token_type = BANG;
          break;
        }
      }

      t = create_token(literal, token_type, lexer->line);
      add_token(lexer, t);
      lexer->position += advance;
      break;
    }
    default: {
      int current_position = lexer->position;
      while (current_position < lexer->total_length) {
        char c = lexer->input[current_position];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
          break;
        }
        if (c == '+' || c == '-' || c == '(' || c == ')' || c == '{' ||
            c == '}' || c == '!' || c == '>' || c == '<' || c == ',' ||
            c == ':' || c == ';' || c == '=' || c == '*' || c == '[' ||
            c == ']') {
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
      } else if (strcmp(literal, "range") == 0) {
        t = create_token(literal, RANGE, lexer->line);
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
      } else if (strcmp(literal, "in") == 0) {
        t = create_token(literal, IN, lexer->line);
      } else if (strcmp(literal, "or") == 0 || strcmp(literal, "OR") == 0) {
        t = create_token(literal, OR, lexer->line);
      } else if (strcmp(literal, "and") == 0) {
        t = create_token(literal, AND, lexer->line);
      } else if (isint(literal)) {
        t = create_token(literal, INT, lexer->line);
      } else if (isfloat(literal)) {
        t = create_token(literal, FLOAT, lexer->line);
      } else if (isalphanumeric(literal)) {
        t = create_token(literal, IDENTIFIER, lexer->line);
      } else {
        t = create_token(literal, UNKNOWN, lexer->line);
      }

      if (t != NULL) {
        add_token(lexer, t);
      }
      free(literal);

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
      "IDENTIFIER",   "FUNC",        "FOR",           "IN",
      "RANGE",        "IF",          "ELSE",          "RETURN",
      "TENSOR",       "INT",         "FLOAT",         "EQUAL",
      "PLUS_EQUAL",   "MINUS_EQUAL", "PLUS",          "MINUS",
      "STAR",         "EQUAL_EQUAL", "BANG_EQUAL",    "LESS",
      "LESS_EQUAL",   "GREATER",     "GREATER_EQUAL", "AND",
      "OR",           "BANG",        "ARROW",         "LEFT_PAREN",
      "RIGHT_PAREN",  "LEFT_BRACE",  "RIGHT_BRACE",   "COMMA",
      "COLON",        "SEMICOLON",   "UNKNOWN",       "LEFT_BRACKET",
      "RIGHT_BRACKET"};
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
    printf("line: %d | literal: %s | type: %s\n", lexer->token_list[i].line,
           literal, token_type_name);
  }
}
