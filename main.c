#include "src/lexer.h"
#include <string.h>

int main() {
  char check[] =
      "func= += ! !=  > >= < <= == -> test(A: tensor<10xf32>) -> { return A; }";

  Lexer *lexer = init_lexer(check, strnlen(check, 100));

  scan(lexer);

  print_tokens(lexer);
}
