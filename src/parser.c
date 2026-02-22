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
    advance(p);
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
      advance(p);
      int capacity = 4;
      int index_count = 0;
      ASTNode **indices = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);
      indices[index_count++] = parse_expression(p);

      while (check(p, COMMA)) {
        advance(p);
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
      advance(p);

      char *name = node->data.identifier.name;
      int capacity = 4;
      int args_count = 0;
      ASTNode **args = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);

      if (!check(p, RIGHT_PAREN)) {
        args[args_count++] = parse_expression(p);
        while (check(p, COMMA)) {
          advance(p);
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

// return_stmt ::= RETURN expression?
ASTNode *parse_return_stmt(Parser *p) {
  Token return_t = expect(p, RETURN);
  ASTNode *return_val = NULL;
  if (is_at_end(p) == false && !check(p, RIGHT_BRACE)) {
    return_val = parse_expression(p);
  }
  return ast_node_return(return_val, return_t.line);
}

// var_decl ::= IDENTIFIER COLON type ( EQUAL expression )?
ASTNode *parse_var_decl(Parser *p) {
  Token identifier = expect(p, IDENTIFIER);
  expect(p, COLON);
  ASTNode *type = parse_type(p);
  ASTNode *initializer = NULL;

  if (check(p, EQUAL)) {
    advance(p);
    initializer = parse_expression(p);
  }
  char *name = strdup(identifier.literal);
  return ast_node_var_decl(name, type, initializer, identifier.line);
}

// assignment_or_expr ::= expression ( ( EQUAL | PLUS_EQUAL | MINUS_EQUAL )
// expression )?
ASTNode *parse_assignment_or_expr(Parser *p) {
  ASTNode *left = parse_expression(p);

  if (check(p, EQUAL)) {
    Token op = advance(p);
    ASTNode *right = parse_expression(p);
    return ast_node_assignment(left, right, op.line);
  }
  return left;
}

// statement ::= var_decl
//  | for_stmt
//  | if_stmt
//  | return_stmt
//  | assignment_or_expr
ASTNode *parse_stmt(Parser *p) {
  if (check(p, FOR)) {
    return parse_for(p);
  }
  if (check(p, IF)) {
    return parse_if(p);
  }
  if (check(p, RETURN)) {
    return parse_return_stmt(p);
  }
  if (check(p, IDENTIFIER) && check_next(p, COLON)) {
    return parse_var_decl(p);
  }
  return parse_assignment_or_expr(p);
}

// block ::= LEFT_BRACE statement* RIGHT_BRACE
ASTNode *parse_block(Parser *p) {
  Token left_brace = expect(p, LEFT_BRACE);
  int current_capacity = 8;
  int current_count = 0;
  ASTNode **statements =
      (ASTNode **)malloc(sizeof(ASTNode *) * current_capacity);

  while (check(p, RIGHT_BRACE) == false && is_at_end(p) == false) {
    if (current_count >= current_capacity) {
      current_capacity *= 2;
      statements =
          (ASTNode **)realloc(statements, sizeof(ASTNode *) * current_capacity);
    }
    statements[current_count++] = parse_stmt(p);
  }
  expect(p, RIGHT_BRACE);
  return ast_node_block(statements, current_count, left_brace.line);
}

// for_stmt ::= FOR IDENTIFIER IN expression block
ASTNode *parse_for(Parser *p) {
  Token for_token = expect(p, FOR);
  Token loop_var = expect(p, IDENTIFIER);
  char *name = strdup(loop_var.literal);
  ASTNode *identifier = ast_node_identifier(name, loop_var.line);

  expect(p, IN);
  ASTNode *iterable = parse_expression(p);
  ASTNode *block_body = parse_block(p);
  return ast_node_for(identifier, iterable, block_body, for_token.line);
}

// if_stmt ::= IF expression block ( ELSE block )?
ASTNode *parse_if(Parser *p) {
  Token if_token = expect(p, IF);
  ASTNode *condition = parse_expression(p);
  ASTNode *then_block = parse_block(p);
  ASTNode *else_block = NULL;

  if (check(p, ELSE)) {
    advance(p);
    else_block = parse_block(p);
  }

  return ast_node_if(condition, then_block, else_block, if_token.line);
}

// function_def ::= FUNC IDENTIFIER LEFT_PAREN param_list? RIGHT_PAREN ARROW
// type block
ASTNode *parse_function_def(Parser *p) {
  expect(p, FUNC);
  Token identifier = expect(p, IDENTIFIER);
  char *func_name = strdup(identifier.literal);

  expect(p, LEFT_PAREN);
  int current_capacity = 4;
  int current_count = 0;
  ASTNode **params = (ASTNode **)malloc(current_capacity * sizeof(ASTNode *));

  if (check(p, RIGHT_PAREN) == false) {
    params[current_count++] = parse_var_decl(p);
    while (check(p, COMMA) && is_at_end(p) == false) {
      advance(p);

      if (current_count >= current_capacity) {
        current_capacity *= 2;
        params =
            (ASTNode **)realloc(params, sizeof(ASTNode *) * current_capacity);
      }
      params[current_count++] = parse_var_decl(p);
    }
  }
  expect(p, RIGHT_PAREN);
  expect(p, ARROW);

  ASTNode *type = parse_type(p);
  ASTNode *body = parse_block(p);

  return ast_node_function_decl(func_name, params, current_count, type, body,
                                identifier.line);
}

// program ::= function_def*
ASTNode *parse_program(Parser *p) {
  int capacity = 4;
  int count = 0;
  ASTNode **functions = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);

  while (!is_at_end(p)) {
    if (count >= capacity) {
      capacity *= 2;
      functions = (ASTNode **)realloc(functions, sizeof(ASTNode *) * capacity);
    }
    functions[count++] = parse_function_def(p);
  }

  return ast_node_program(functions, count, 0);
}
