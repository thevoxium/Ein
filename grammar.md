program             ::= function_def*

function_def        ::= FUNC IDENTIFIER LEFT_PAREN param_list? RIGHT_PAREN
                        ARROW type block

param_list          ::= param ( COMMA param )*
param               ::= IDENTIFIER COLON type

type                ::= TENSOR LESS IDENTIFIER GREATER
                      | IDENTIFIER

block               ::= LEFT_BRACE statement* RIGHT_BRACE

statement           ::= var_decl
                      | for_stmt
                      | if_stmt
                      | return_stmt
                      | assignment_or_expr

var_decl            ::= IDENTIFIER COLON type ( EQUAL expression )?
for_stmt            ::= FOR IDENTIFIER IN expression block
if_stmt             ::= IF expression block ( ELSE block )?
return_stmt         ::= RETURN expression?
assignment_or_expr  ::= expression
                        ( ( EQUAL | PLUS_EQUAL | MINUS_EQUAL ) expression )?

expression          ::= logic_or
logic_or            ::= logic_and ( OR logic_and )*
logic_and           ::= equality ( AND equality )*
equality            ::= comparison ( ( EQUAL_EQUAL | BANG_EQUAL ) comparison )*
comparison          ::= term ( ( LESS | LESS_EQUAL | GREATER | GREATER_EQUAL ) term )*
term                ::= factor ( ( PLUS | MINUS ) factor )*
factor              ::= unary ( STAR unary )*
unary               ::= ( MINUS | BANG ) unary
                      | postfix
postfix             ::= primary ( index_suffix | call_suffix )*
index_suffix        ::= LEFT_BRACKET expression ( COMMA expression )* RIGHT_BRACKET
call_suffix         ::= LEFT_PAREN ( expression ( COMMA expression )* )? RIGHT_PAREN
primary             ::= INT | FLOAT | IDENTIFIER | RANGE
                      | LEFT_PAREN expression RIGHT_PAREN
