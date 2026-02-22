#include "src/ast.h"
#include "src/lexer.h"
#include "src/parser.h"
#include "src/utils.h"
#include <string.h>

int main() {

  // long len;
  // char file_name[] = "examples/matmul.ein";
  // char *input = read_ein_file(file_name, &len);
  char input[] = "{ C: tensor<MxNxf32> = 0.0 for i in range(0, M) { for j in "
                 "range(0, N) { for k in range(0, K) { C[i, j] = C[i, j] + "
                 "A[i, k] * B[k, j] } } } return C }";
  Lexer *lexer = init_lexer(input, strlen(input));
  scan(lexer);

  Parser *p = init_parser(lexer);
  ASTNode *node = parse_block(p);
  print_ast(node, 0);

  free_lexer(lexer);
  free_parser(p);
  free_ast(node);
}
