# Ein

A programming language designed for tensor computations. Currently implements a
lexer, parser, and AST printer.

## Building

ein is written in C with no external dependencies. Compile with:

```
cc -o out main.c src/lexer.c src/parser.c src/ast.c src/utils.c
```

Run:

```
./out
```

This parses `examples/matmul.ein` and prints the AST.

## Language Features

**Functions** -- Defined with `func`, typed parameters, and a return type:

```
func add(a: f32, b: f32) -> f32 {
  return a + b
}
```

**Tensor types** -- First-class tensor type annotations with dimensions and
data types:

```
A: tensor<MxKxf32>
```

**Variable declarations** -- Typed variables with optional initializers:

```
C: tensor<MxNxf32> = 0.0
```

**For loops** -- Iterate over ranges:

```
for i in range(0, N) {
  ...
}
```

**If/else** -- Conditional branching:

```
if x > 0 {
  ...
} else {
  ...
}
```

**Tensor indexing** -- Multi-dimensional indexing:

```
C[i, j] = A[i, k] * B[k, j]
```

**Operators** -- Arithmetic (`+`, `-`, `*`), comparison (`<`, `<=`, `>`, `>=`,
`==`, `!=`), logical (`and`, `or`, `!`), and compound assignment (`+=`, `-=`).

**Function calls**:

```
range(0, M)
```

## Example

Matrix multiplication (`examples/matmul.ein`):

```
func matmul(A: tensor<MxKxf32>, B: tensor<KxNxf32>) -> tensor<MxNxf32> {
  C: tensor<MxNxf32> = 0.0

  for i in range(0, M) {
    for j in range(0, N) {
      for k in range(0, K) {
        C[i, j] = C[i, j] + A[i, k] * B[k, j]
      }
    }
  }

  return C
}
```
