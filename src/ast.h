#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum NodeType {
  // Top-level
  NODE_PROGRAM,
  NODE_FUNC_DEF,

  // Statements
  NODE_BLOCK,
  NODE_VAR_DECL,
  NODE_ASSIGNMENT,
  NODE_FOR,
  NODE_IF,
  NODE_RETURN,

  // Expressions
  NODE_INT_LITERAL,
  NODE_FLOAT_LITERAL,
  NODE_IDENTIFIER,
  NODE_BINARY_EXPR,
  NODE_UNARY_EXPR,
  NODE_INDEX_EXPR,
  NODE_FUNC_CALL,

  // Types
  NODE_TENSOR_TYPE,
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
  NodeType nodeType;
  int line;

  union {
    struct {
      ASTNode **functions;
      int function;
    } program;

    struct {
      char *name;
      ASTNode **params;
      int count_params;
      ASTNode *return_type;
      ASTNode *body;
    } function_decl;

    struct {
      ASTNode **statements;
      int count_statements;
    } block;

    struct {
      char *name;
      ASTNode *type;
      ASTNode *initializer;
    } var_decl;

    struct {
      ASTNode *target;
      ASTNode *value;
    } assignment;

    struct {
      ASTNode *varible;
      ASTNode *iterable;
      ASTNode *body;
    } for_loop;

    struct {
      ASTNode *condition;
      ASTNode *then;
      ASTNode *else_block;
    } if_else;

    struct {
      ASTNode *return_val;
    } return_value;

    struct {
      long value;
    } int_literal;

    struct {
      double value;
    } float_literal;

    struct {
      TokenType op;
      ASTNode *left;
      ASTNode *right;
    } binary_op;

    struct {
      TokenType op;
      ASTNode *operand;
    } unary_op;

    struct {
      ASTNode *object;
      ASTNode **indices;
    } index_expression;

    struct {
      char *func_name;
      ASTNode **args;
      int arg_count;
    } func_call;

    struct {
      char **dims;
      int dim_count;
      char *data_type;
    } tensor_type;

  } data;
};

#endif // !AST_H
