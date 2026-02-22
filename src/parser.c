#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

bool check_next(Parser *p, TokenType tokenType) {
  if (is_at_end(p)) {
    return false;
  }
  return p->tokens[p->current + 1].tokenType == tokenType;
}

Token advance(Parser *p) {
  Token t = p->tokens[p->current];
  p->current++;
  return t;
}

Token expect(Parser *p, TokenType tokenType) {
  if (check(p, tokenType)) {
    return advance(p);
  }

  fprintf(stderr, "Parse error at line %d: expected token type %d, got '%s'\n",
          peek(p).line, tokenType, peek(p).literal);
  exit(1);
}

// primary         ::= INT
//                   | FLOAT
//                   | IDENTIFIER
//                   | "range"
//                   | "(" expression ")" ;
ASTNode *parse_primary(Parser *p) {
  if (check(p, INT)) {
    Token t = advance(p);
    long value = atol(t.literal);
    return ast_node_int_literal(value, t.line);
  }
  if (check(p, FLOAT)) {
    Token t = advance(p);
    double value = atof(t.literal);
    return ast_node_float_literal(value, t.line);
  }
  if (check(p, IDENTIFIER)) {
    Token t = advance(p);
    return ast_node_identifier(t.literal, t.line);
  }
  if (check(p, RANGE)) {
    Token t = advance(p);
    return ast_node_identifier(t.literal, t.line);
  }
  if (check(p, LEFT_PAREN)) {
    Token t = advance(p);
    ASTNode *expr = parse_expression(p);
    expect(p, RIGHT_PAREN);
    return expr;
  }

  fprintf(stderr, "Parse error at line %d: unexpected token '%s'\n",
          peek(p).line, peek(p).literal);
  exit(1);
}

// postfix ::= primary ( index_suffix | call_suffix )*
ASTNode *parse_postfix(Parser *p) {
  ASTNode *node = parse_primary(p);

  while (!is_at_end(p)) {
    if (check(p, LEFT_BRACKET)) {
      Token bracket = advance(p);
      int capacity = 4;
      int index_count = 0;
      ASTNode **indices = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);
      indices[index_count++] = parse_expression(p);

      while (check(p, COMMA)) {
        Token comma = advance(p);
        if (index_count >= capacity) {
          capacity *= 2;
          indices = (ASTNode **)realloc(indices, sizeof(ASTNode *) * capacity);
        }

        indices[index_count++] = parse_expression(p);
      }

      expect(p, RIGHT_BRACKET);
      int line = node->line;
      node = ast_node_index_expr(node, indices, index_count, line);
      return node;

    } else if (check(p, LEFT_PAREN)) {
      Token parantheses = advance(p);

      char *name = node->data.identifier.name;
      int capacity = 4;
      int args_count = 0;
      ASTNode **args = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);

      if (!check(p, RIGHT_PAREN)) {
        args[args_count++] = parse_expression(p);
        while (check(p, COMMA)) {
          Token comma = advance(p);
          if (args_count >= capacity) {
            capacity *= 2;
            args = (ASTNode **)realloc(args, sizeof(ASTNode *) * capacity);
          }

          args[args_count++] = parse_expression(p);
        }
      }

      expect(p, RIGHT_PAREN);
      int line = node->line;
      free(node);
      node = ast_node_func_call(name, args, args_count, line);
      return node;

    } else {
      break;
    }
  }
  return node;
}

// unary ::= ( MINUS | BANG ) unary
//          | postfix
ASTNode *parse_unary(Parser *p) {
  if (check(p, BANG) || check(p, MINUS)) {
    Token op = advance(p);
    ASTNode *operand = parse_unary(p);
    return ast_node_unary_expr(op.tokenType, operand, op.line);
  } else
    return parse_postfix(p);
}

// factor ::= unary ( STAR unary )*
ASTNode *parse_factor(Parser *p) {
  ASTNode *left = parse_unary(p);
  while (check(p, STAR)) {
    Token t = advance(p);
    ASTNode *right = parse_unary(p);
    left = ast_node_binary_expr(STAR, left, right, t.line);
  }
  return left;
}

// term ::= factor ( ( PLUS | MINUS ) factor )*
ASTNode *parse_term(Parser *p) {
  ASTNode *left = parse_factor(p);
  while (check(p, PLUS) || check(p, MINUS)) {
    Token t = advance(p);
    ASTNode *right = parse_factor(p);
    left = ast_node_binary_expr(t.tokenType, left, right, t.line);
  }
  return left;
}

// comparison ::= term ( ( LESS | LESS_EQUAL | GREATER | GREATER_EQUAL ) term )*
ASTNode *parse_comparison(Parser *p) {
  ASTNode *left = parse_term(p);
  while (check(p, LESS) || check(p, LESS_EQUAL) || check(p, GREATER_EQUAL) ||
         check(p, GREATER)) {
    Token t = advance(p);
    ASTNode *right = parse_term(p);
    left = ast_node_binary_expr(t.tokenType, left, right, t.line);
  }
  return left;
}

// equality ::= comparison ( ( EQUAL_EQUAL | BANG_EQUAL ) comparison )*
ASTNode *parse_equality(Parser *p) {
  ASTNode *left = parse_comparison(p);
  while (check(p, EQUAL_EQUAL) || check(p, BANG_EQUAL)) {
    Token t = advance(p);
    ASTNode *right = parse_comparison(p);
    left = ast_node_binary_expr(t.tokenType, left, right, t.line);
  }
  return left;
}

// logic_and ::= equality ( AND equality )*
ASTNode *parse_logic_and(Parser *p) {
  ASTNode *left = parse_equality(p);
  while (check(p, AND)) {
    Token t = advance(p);
    ASTNode *right = parse_equality(p);
    left = ast_node_binary_expr(t.tokenType, left, right, t.line);
  }
  return left;
}

// logic_or ::= logic_and ( OR logic_and )*
ASTNode *parse_logic_or(Parser *p) {
  ASTNode *left = parse_logic_and(p);
  while (check(p, OR)) {
    Token t = advance(p);
    ASTNode *right = parse_logic_and(p);
    left = ast_node_binary_expr(t.tokenType, left, right, t.line);
  }
  return left;
}

ASTNode *parse_expression(Parser *p) { return parse_logic_or(p); }

// type ::= TENSOR LESS IDENTIFIER GREATER
//  | IDENTIFIER
ASTNode *parse_type(Parser *p) {
  if (check(p, TENSOR)) {
    Token tensor = advance(p);
    expect(p, LESS);
    Token identifier = expect(p, IDENTIFIER);
    expect(p, GREATER);

    char *s = identifier.literal;
    int segs = 0;
    while (*s != '\0') {
      if (*s == 'x')
        segs++;
      s++;
    }
    segs++;

    char **dims = (char **)malloc(segs * sizeof(char *));
    assert(dims != NULL);

    s = identifier.literal;
    for (int i = 0; i < segs; i++) {
      if (*s == 'x') {
        s++;
      }
      int len = 0;
      while (*s != '\0' && *s != 'x') {
        len++;
        s++;
      }
      char *segment = strndup(s - len, len);
      dims[i] = segment;
    }

    ASTNode *result =
        ast_node_tensor_type(dims, segs - 1, dims[segs - 1], tensor.line);

    for (int i = 0; i < segs; i++) {
      free(dims[i]);
    }
    free(dims);
    return result;

  } else if (check(p, IDENTIFIER)) {
    Token t = advance(p);
    return ast_node_identifier(t.literal, t.line);
  } else {
    fprintf(stderr,
            "Parse error at line %d: expected token type %d, got '%s'\n",
            p->tokens[p->current].line, p->tokens[p->current].tokenType,
            p->tokens[p->current].literal);
    exit(1);
  }
}
