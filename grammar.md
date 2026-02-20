program         ::= function_decl* EOF ;

function_decl   ::= "func" IDENTIFIER "(" param_list? ")" "->" type block ;
param_list      ::= param ( "," param )* ;
param           ::= IDENTIFIER ":" type ;

block           ::= "{" statement* "}" ;

statement       ::= var_decl
                  | assignment_stmt
                  | for_stmt
                  | if_stmt
                  | return_stmt
                  | expr_stmt ;

var_decl        ::= IDENTIFIER ":" type ( "=" expression )? ;
assignment_stmt ::= assignable "=" expression ;
assignable      ::= IDENTIFIER index_suffix* ;

for_stmt        ::= "for" IDENTIFIER "in" expression block ;
if_stmt         ::= "if" expression block ( "else" block )? ;
return_stmt     ::= "return" expression? ;
expr_stmt       ::= expression ;

expression      ::= assignment ;
assignment      ::= logic_or ( "=" assignment )? ;

logic_or        ::= logic_and ( "or" logic_and )* ;
logic_and       ::= equality ( "and" equality )* ;

equality        ::= comparison ( ( "==" | "!=" ) comparison )* ;
comparison      ::= term ( ( "<" | "<=" | ">" | ">=" ) term )* ;
term            ::= factor ( ( "+" | "-" ) factor )* ;
factor          ::= unary ( "*" unary )* ;

unary           ::= ( "!" | "-" ) unary
                  | postfix ;

postfix         ::= primary ( index_suffix | call_suffix )* ;

index_suffix    ::= "[" expression ( "," expression )* "]" ;
call_suffix     ::= "(" argument_list? ")" ;
argument_list   ::= expression ( "," expression )* ;

primary         ::= INT
                  | FLOAT
                  | IDENTIFIER
                  | "range"
                  | "(" expression ")" ;

type            ::= tensor_type | IDENTIFIER ;
tensor_type     ::= "tensor" "<" tensor_spec ">" ;
tensor_spec     ::= IDENTIFIER ;
