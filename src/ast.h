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
      int function_count;
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
      ASTNode *variable;
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
      char *name;
    } identifier;

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
      int index_count;
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

ASTNode *create_node(NodeType nodeType, int line);
ASTNode *ast_node_program(ASTNode **functions, int function_count, int line);
ASTNode *ast_node_function_decl(char *name, ASTNode **params, int count_params,
                                ASTNode *return_type, ASTNode *body, int line);
ASTNode *ast_node_block(ASTNode **statements, int count_statements, int line);
ASTNode *ast_var_decl(char *name, ASTNode *type, ASTNode *initializer,
                      int line);
ASTNode *ast_node_var_decl(char *name, ASTNode *type, ASTNode *initializer,
                           int line);
ASTNode *ast_node_assignment(ASTNode *target, ASTNode *value, int line);
ASTNode *ast_node_for(ASTNode *variable, ASTNode *iterable, ASTNode *body,
                      int line);
ASTNode *ast_node_if(ASTNode *condition, ASTNode *then_block,
                     ASTNode *else_block, int line);
ASTNode *ast_node_return(ASTNode *return_val, int line);
ASTNode *ast_node_int_literal(long value, int line);
ASTNode *ast_node_float_literal(double value, int line);
ASTNode *ast_node_identifier(char *name, int line);
ASTNode *ast_node_binary_expr(TokenType op, ASTNode *left, ASTNode *right,
                              int line);
ASTNode *ast_node_unary_expr(TokenType op, ASTNode *operand, int line);
ASTNode *ast_node_index_expr(ASTNode *object, ASTNode **indices, int index_count,
                             int line);
ASTNode *ast_node_func_call(char *func_name, ASTNode **args, int arg_count,
                            int line);
ASTNode *ast_node_tensor_type(char **dims, int dim_count, char *data_type,
                              int line);
void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif // !AST_H
