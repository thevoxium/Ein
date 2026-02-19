#include "src/lexer.h"
#include "src/utils.h"

int main() {

  long len;
  char file_name[] = "examples/matmul.ein";
  char *input = read_ein_file(file_name, &len);

  Lexer *lexer = init_lexer(input, len);

  scan(lexer);

  print_tokens(lexer);
}
