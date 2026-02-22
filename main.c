#include "src/ast.h"
#include "src/lexer.h"
#include "src/parser.h"
#include "src/utils.h"

int main() {

  // long len;
  // char file_name[] = "examples/matmul.ein";
  // char *input = read_ein_file(file_name, &len);

  char input[] = "C[i, j] = C[i, j] + A[i, k] * B[k, j]";
  Lexer *lexer = init_lexer(input, 100);
  scan(lexer);

  Parser *p = init_parser(lexer);
  ASTNode *node = parse_assignment_or_expr(p);
  print_ast(node, 0);

  free_lexer(lexer);
  free_parser(p);
  free_ast(node);
}
