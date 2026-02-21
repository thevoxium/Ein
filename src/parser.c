#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <stdlib.h>

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
  // if (check(p, LEFT_PAREN)) {
  //   Token t = advance(p);
  //   ASTNode *expr = parse_expression(p);
  //   expect(p, RIGHT_PAREN);
  //   return expr;
  // }

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
      // indices[index_count++] = parse_expression(p);
      indices[index_count++] = parse_primary(p);

      while (check(p, COMMA)) {
        Token comma = advance(p);
        if (index_count >= capacity) {
          capacity *= 2;
          indices = (ASTNode **)realloc(indices, sizeof(ASTNode *) * capacity);
        }

        // indices[index_count++] = parse_expression(p);
        indices[index_count++] = parse_primary(p);
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
      // args[args_count++] = parse_expression(p);

      if (!check(p, RIGHT_PAREN)) {
        args[args_count++] = parse_primary(p);
        while (check(p, COMMA)) {
          Token comma = advance(p);
          if (args_count >= capacity) {
            capacity *= 2;
            args = (ASTNode **)realloc(args, sizeof(ASTNode *) * capacity);
          }

          // args[args_count++] = parse_expression(p);
          args[args_count++] = parse_primary(p);
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
