# Snakelisp

The snakelisp is a lisp dialect by Henri Tuhola. It is a study language, to find out what is so damn special in lisp.

Code Sample:

    (factorial = (func (x)
        (cond 
            ((>= x 1)   (* x (factorial (- x 1)))) 
            (else       1))))
    (factorial 14)

## Syntax

The language is using a variation of the list notation, common for lisp family of the languages. The S-expression consist of a parenthesis pair and atoms between.

### Comments

Comments start with a hash character `#` and end on a line.

### Symbols

Symbols start with any letter except a parenthesis, string terminator or digit.

### Attributes

Attributes start with a dot, and they bind to a structure on their left side, if there is any.

### Numbers

There are three notations for numbers:

    1234
    3x12012
    5.2

The first digit in the notation containing an x is the base of the number. If it is 0, the number is interpreted as hexadecimal.

### Strings

Strings start either with a double quote or single quote character. Escape character notation is supported with a backslash. It's up to the evaluator to interpret the string's meaning.

## Lexical scope

The snakelisp is lexically scoped. Every function defines a boundary. Inside the boundary one is free to bind new values with `(symbol = value)` -special form.  One can access variables in higher scope with `(symbol := value)`, though this is restricted if the local scope shadows the value.

## Continuation passing as evaluation strategy

The snakelisp is compiling itself into continuation passing style as the intermediary form.
