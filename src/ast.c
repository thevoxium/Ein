#include "ast.h"

static void print_indent(int indent) {
  for (int i = 0; i < indent; i++) {
    printf("  ");
  }
}

static const char *token_type_name(TokenType tokenType) {
  switch (tokenType) {
  case IDENTIFIER:
    return "IDENTIFIER";
  case FUNC:
    return "FUNC";
  case FOR:
    return "FOR";
  case IN:
    return "IN";
  case RANGE:
    return "RANGE";
  case IF:
    return "IF";
  case ELSE:
    return "ELSE";
  case RETURN:
    return "RETURN";
  case TENSOR:
    return "TENSOR";
  case INT:
    return "INT";
  case FLOAT:
    return "FLOAT";
  case EQUAL:
    return "EQUAL";
  case PLUS_EQUAL:
    return "PLUS_EQUAL";
  case MINUS_EQUAL:
    return "MINUS_EQUAL";
  case PLUS:
    return "PLUS";
  case MINUS:
    return "MINUS";
  case STAR:
    return "STAR";
  case EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case BANG_EQUAL:
    return "BANG_EQUAL";
  case LESS:
    return "LESS";
  case LESS_EQUAL:
    return "LESS_EQUAL";
  case GREATER:
    return "GREATER";
  case GREATER_EQUAL:
    return "GREATER_EQUAL";
  case AND:
    return "AND";
  case OR:
    return "OR";
  case BANG:
    return "BANG";
  case ARROW:
    return "ARROW";
  case LEFT_PAREN:
    return "LEFT_PAREN";
  case RIGHT_PAREN:
    return "RIGHT_PAREN";
  case LEFT_BRACE:
    return "LEFT_BRACE";
  case RIGHT_BRACE:
    return "RIGHT_BRACE";
  case COMMA:
    return "COMMA";
  case COLON:
    return "COLON";
  case SEMICOLON:
    return "SEMICOLON";
  case UNKNOWN:
    return "UNKNOWN";
  case LEFT_BRACKET:
    return "LEFT_BRACKET";
  case RIGHT_BRACKET:
    return "RIGHT_BRACKET";
  default:
    return "TOKEN_UNKNOWN";
  }
}

ASTNode *create_node(NodeType nodeType, int line) {
  ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
  if (!node)
    return NULL;

  node->nodeType = nodeType;
  node->line = line;
  return node;
}

ASTNode *ast_node_int_literal(long value, int line) {
  ASTNode *node = create_node(NODE_INT_LITERAL, line);
  if (!node)
    return NULL;

  node->data.int_literal.value = value;
  return node;
}

ASTNode *ast_node_float_literal(double value, int line) {
  ASTNode *node = create_node(NODE_FLOAT_LITERAL, line);
  if (!node)
    return NULL;

  node->data.float_literal.value = value;
  return node;
}

ASTNode *ast_node_program(ASTNode **functions, int function_count, int line) {
  ASTNode *node = create_node(NODE_PROGRAM, line);
  if (!node)
    return NULL;

  node->data.program.functions = functions;
  node->data.program.function_count = function_count;
  return node;
}

ASTNode *ast_node_function_decl(char *name, ASTNode **params, int count_params,
                                ASTNode *return_type, ASTNode *body, int line) {

  ASTNode *node = create_node(NODE_FUNC_DEF, line);
  if (!node)
    return NULL;

  node->data.function_decl.name = strdup(name);
  node->data.function_decl.params = params;
  node->data.function_decl.count_params = count_params;
  node->data.function_decl.return_type = return_type;
  node->data.function_decl.body = body;
  return node;
}

ASTNode *ast_node_block(ASTNode **statements, int count_statements, int line) {

  ASTNode *node = create_node(NODE_BLOCK, line);
  if (!node)
    return NULL;

  node->data.block.count_statements = count_statements;
  node->data.block.statements = statements;
  return node;
}

ASTNode *ast_var_decl(char *name, ASTNode *type, ASTNode *initializer,
                      int line) {
  ASTNode *node = create_node(NODE_VAR_DECL, line);
  if (!node)
    return NULL;

  node->data.var_decl.name = strdup(name);
  node->data.var_decl.type = type;
  node->data.var_decl.initializer = initializer;

  return node;
}

ASTNode *ast_node_var_decl(char *name, ASTNode *type, ASTNode *initializer,
                           int line) {
  return ast_var_decl(name, type, initializer, line);
}

ASTNode *ast_node_assignment(ASTNode *target, ASTNode *value, int line) {
  ASTNode *node = create_node(NODE_ASSIGNMENT, line);
  if (!node)
    return NULL;

  node->data.assignment.target = target;
  node->data.assignment.value = value;
  return node;
}

ASTNode *ast_node_for(ASTNode *variable, ASTNode *iterable, ASTNode *body,
                      int line) {
  ASTNode *node = create_node(NODE_FOR, line);
  if (!node)
    return NULL;

  node->data.for_loop.variable = variable;
  node->data.for_loop.iterable = iterable;
  node->data.for_loop.body = body;
  return node;
}

ASTNode *ast_node_if(ASTNode *condition, ASTNode *then_block,
                     ASTNode *else_block, int line) {
  ASTNode *node = create_node(NODE_IF, line);
  if (!node)
    return NULL;

  node->data.if_else.condition = condition;
  node->data.if_else.then = then_block;
  node->data.if_else.else_block = else_block;
  return node;
}

ASTNode *ast_node_return(ASTNode *return_val, int line) {
  ASTNode *node = create_node(NODE_RETURN, line);
  if (!node)
    return NULL;

  node->data.return_value.return_val = return_val;
  return node;
}

ASTNode *ast_node_identifier(char *name, int line) {
  ASTNode *node = create_node(NODE_IDENTIFIER, line);
  if (!node)
    return NULL;

  node->data.identifier.name = strdup(name);
  return node;
}

ASTNode *ast_node_binary_expr(TokenType op, ASTNode *left, ASTNode *right,
                              int line) {
  ASTNode *node = create_node(NODE_BINARY_EXPR, line);
  if (!node)
    return NULL;

  node->data.binary_op.op = op;
  node->data.binary_op.left = left;
  node->data.binary_op.right = right;
  return node;
}

ASTNode *ast_node_unary_expr(TokenType op, ASTNode *operand, int line) {
  ASTNode *node = create_node(NODE_UNARY_EXPR, line);
  if (!node)
    return NULL;

  node->data.unary_op.op = op;
  node->data.unary_op.operand = operand;
  return node;
}

ASTNode *ast_node_index_expr(ASTNode *object, ASTNode **indices, int index_count,
                             int line) {
  ASTNode *node = create_node(NODE_INDEX_EXPR, line);
  if (!node)
    return NULL;

  node->data.index_expression.object = object;
  node->data.index_expression.indices = indices;
  node->data.index_expression.index_count = index_count;
  return node;
}

ASTNode *ast_node_func_call(char *func_name, ASTNode **args, int arg_count,
                            int line) {
  ASTNode *node = create_node(NODE_FUNC_CALL, line);
  if (!node)
    return NULL;

  node->data.func_call.func_name = strdup(func_name);
  node->data.func_call.args = args;
  node->data.func_call.arg_count = arg_count;
  return node;
}

ASTNode *ast_node_tensor_type(char **dims, int dim_count, char *data_type,
                              int line) {
  ASTNode *node = create_node(NODE_TENSOR_TYPE, line);
  if (!node)
    return NULL;

  node->data.tensor_type.dims = (char **)malloc(sizeof(char *) * dim_count);
  for (int i = 0; i < dim_count; i++) {
    node->data.tensor_type.dims[i] = strdup(dims[i]);
  }
  node->data.tensor_type.dim_count = dim_count;
  node->data.tensor_type.data_type = strdup(data_type);
  return node;
}

void print_ast(ASTNode *node, int indent) {
  if (!node) {
    print_indent(indent);
    printf("(null)\n");
    return;
  }

  switch (node->nodeType) {
  case NODE_PROGRAM:
    print_indent(indent);
    printf("Program (functions=%d)\n", node->data.program.function_count);
    for (int i = 0; i < node->data.program.function_count; i++) {
      print_ast(node->data.program.functions[i], indent + 1);
    }
    break;
  case NODE_FUNC_DEF:
    print_indent(indent);
    printf("FunctionDef name=%s\n", node->data.function_decl.name);
    print_indent(indent + 1);
    printf("Params (%d)\n", node->data.function_decl.count_params);
    for (int i = 0; i < node->data.function_decl.count_params; i++) {
      print_ast(node->data.function_decl.params[i], indent + 2);
    }
    print_indent(indent + 1);
    printf("ReturnType\n");
    print_ast(node->data.function_decl.return_type, indent + 2);
    print_indent(indent + 1);
    printf("Body\n");
    print_ast(node->data.function_decl.body, indent + 2);
    break;
  case NODE_BLOCK:
    print_indent(indent);
    printf("Block (statements=%d)\n", node->data.block.count_statements);
    for (int i = 0; i < node->data.block.count_statements; i++) {
      print_ast(node->data.block.statements[i], indent + 1);
    }
    break;
  case NODE_VAR_DECL:
    print_indent(indent);
    printf("VarDecl name=%s\n", node->data.var_decl.name);
    print_indent(indent + 1);
    printf("Type\n");
    print_ast(node->data.var_decl.type, indent + 2);
    print_indent(indent + 1);
    printf("Initializer\n");
    print_ast(node->data.var_decl.initializer, indent + 2);
    break;
  case NODE_ASSIGNMENT:
    print_indent(indent);
    printf("Assignment\n");
    print_indent(indent + 1);
    printf("Target\n");
    print_ast(node->data.assignment.target, indent + 2);
    print_indent(indent + 1);
    printf("Value\n");
    print_ast(node->data.assignment.value, indent + 2);
    break;
  case NODE_FOR:
    print_indent(indent);
    printf("For\n");
    print_indent(indent + 1);
    printf("Variable\n");
    print_ast(node->data.for_loop.variable, indent + 2);
    print_indent(indent + 1);
    printf("Iterable\n");
    print_ast(node->data.for_loop.iterable, indent + 2);
    print_indent(indent + 1);
    printf("Body\n");
    print_ast(node->data.for_loop.body, indent + 2);
    break;
  case NODE_IF:
    print_indent(indent);
    printf("If\n");
    print_indent(indent + 1);
    printf("Condition\n");
    print_ast(node->data.if_else.condition, indent + 2);
    print_indent(indent + 1);
    printf("Then\n");
    print_ast(node->data.if_else.then, indent + 2);
    print_indent(indent + 1);
    printf("Else\n");
    print_ast(node->data.if_else.else_block, indent + 2);
    break;
  case NODE_RETURN:
    print_indent(indent);
    printf("Return\n");
    print_ast(node->data.return_value.return_val, indent + 1);
    break;
  case NODE_INT_LITERAL:
    print_indent(indent);
    printf("IntLiteral value=%ld\n", node->data.int_literal.value);
    break;
  case NODE_FLOAT_LITERAL:
    print_indent(indent);
    printf("FloatLiteral value=%f\n", node->data.float_literal.value);
    break;
  case NODE_IDENTIFIER:
    print_indent(indent);
    printf("Identifier name=%s\n", node->data.identifier.name);
    break;
  case NODE_BINARY_EXPR:
    print_indent(indent);
    printf("BinaryExpr op=%s\n", token_type_name(node->data.binary_op.op));
    print_indent(indent + 1);
    printf("Left\n");
    print_ast(node->data.binary_op.left, indent + 2);
    print_indent(indent + 1);
    printf("Right\n");
    print_ast(node->data.binary_op.right, indent + 2);
    break;
  case NODE_UNARY_EXPR:
    print_indent(indent);
    printf("UnaryExpr op=%s\n", token_type_name(node->data.unary_op.op));
    print_indent(indent + 1);
    printf("Operand\n");
    print_ast(node->data.unary_op.operand, indent + 2);
    break;
  case NODE_INDEX_EXPR:
    print_indent(indent);
    printf("IndexExpr (indices=%d)\n", node->data.index_expression.index_count);
    print_indent(indent + 1);
    printf("Object\n");
    print_ast(node->data.index_expression.object, indent + 2);
    print_indent(indent + 1);
    printf("Indices\n");
    for (int i = 0; i < node->data.index_expression.index_count; i++) {
      print_ast(node->data.index_expression.indices[i], indent + 2);
    }
    break;
  case NODE_FUNC_CALL:
    print_indent(indent);
    printf("FuncCall name=%s args=%d\n", node->data.func_call.func_name,
           node->data.func_call.arg_count);
    for (int i = 0; i < node->data.func_call.arg_count; i++) {
      print_ast(node->data.func_call.args[i], indent + 1);
    }
    break;
  case NODE_TENSOR_TYPE:
    print_indent(indent);
    printf("TensorType dtype=%s dims=%d\n", node->data.tensor_type.data_type,
           node->data.tensor_type.dim_count);
    for (int i = 0; i < node->data.tensor_type.dim_count; i++) {
      print_indent(indent + 1);
      printf("Dim[%d]=%s\n", i, node->data.tensor_type.dims[i]);
    }
    break;
  }
}

void free_ast(ASTNode *node) {
  if (!node)
    return;

  switch (node->nodeType) {
  case NODE_PROGRAM:
    for (int i = 0; i < node->data.program.function_count; i++)
      free_ast(node->data.program.functions[i]);
    free(node->data.program.functions);
    break;
  case NODE_FUNC_DEF:
    free(node->data.function_decl.name);
    for (int i = 0; i < node->data.function_decl.count_params; i++)
      free_ast(node->data.function_decl.params[i]);
    free(node->data.function_decl.params);
    free_ast(node->data.function_decl.return_type);
    free_ast(node->data.function_decl.body);
    break;
  case NODE_BLOCK:
    for (int i = 0; i < node->data.block.count_statements; i++)
      free_ast(node->data.block.statements[i]);
    free(node->data.block.statements);
    break;
  case NODE_VAR_DECL:
    free(node->data.var_decl.name);
    free_ast(node->data.var_decl.type);
    free_ast(node->data.var_decl.initializer);
    break;
  case NODE_ASSIGNMENT:
    free_ast(node->data.assignment.target);
    free_ast(node->data.assignment.value);
    break;
  case NODE_FOR:
    free_ast(node->data.for_loop.variable);
    free_ast(node->data.for_loop.iterable);
    free_ast(node->data.for_loop.body);
    break;
  case NODE_IF:
    free_ast(node->data.if_else.condition);
    free_ast(node->data.if_else.then);
    free_ast(node->data.if_else.else_block);
    break;
  case NODE_RETURN:
    free_ast(node->data.return_value.return_val);
    break;
  case NODE_INT_LITERAL:
    break;
  case NODE_FLOAT_LITERAL:
    break;
  case NODE_IDENTIFIER:
    free(node->data.identifier.name);
    break;
  case NODE_BINARY_EXPR:
    free_ast(node->data.binary_op.left);
    free_ast(node->data.binary_op.right);
    break;
  case NODE_UNARY_EXPR:
    free_ast(node->data.unary_op.operand);
    break;
  case NODE_INDEX_EXPR:
    free_ast(node->data.index_expression.object);
    for (int i = 0; i < node->data.index_expression.index_count; i++)
      free_ast(node->data.index_expression.indices[i]);
    free(node->data.index_expression.indices);
    break;
  case NODE_FUNC_CALL:
    free(node->data.func_call.func_name);
    for (int i = 0; i < node->data.func_call.arg_count; i++)
      free_ast(node->data.func_call.args[i]);
    free(node->data.func_call.args);
    break;
  case NODE_TENSOR_TYPE:
    for (int i = 0; i < node->data.tensor_type.dim_count; i++)
      free(node->data.tensor_type.dims[i]);
    free(node->data.tensor_type.dims);
    free(node->data.tensor_type.data_type);
    break;
  }

  free(node);
}
