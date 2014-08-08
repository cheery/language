" Vim syntax file
" Language: Snakelisp
" Maintainer: Henri Tuhola
" Latest Revision: 8 Aug 2014

if exists("b:current_syntax")
    finish
endif

syn match  snakeComment "#.*$"
syn region snakeString start=/\v"/ skip=/\v\\./ end=/\v"/
syn region snakeString start=/\v'/ skip=/\v\\./ end=/\v'/
syn match  snakeSymbol "[a-zA-Z_][a-zA-Z_0-9\-]*"
syn match  snakeDelimiter "("
syn match  snakeDelimiter ")"
syn match  snakeDelimiter "="
syn match  snakeNumber "[0-9][0-9]*"
syn match  snakeNumber "[0-9]x[0-9a-fA-F]*"
syn match  snakeFloat  "[0-9][0-9]*\.[0-9]*"
syn keyword snakeSyntax func if cond while

hi def link snakeComment     Comment
hi def link snakeString      String
hi def link snakeNumber      Number
hi def link snakeFloat       Float
hi def link snakeDelimiter   Delimiter
hi def link snakeSyntax      Keyword

let b:current_syntax = "snakelisp"
