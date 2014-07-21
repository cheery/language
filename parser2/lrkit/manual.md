# LRKit User's Manual

Everything not described here isn't part of the API. If you need other
internals, tell about your needs in the github repository.

## Rule objects

Production rules that form a grammar when put into a list.

    rule = lrkit.Rule(lhs, prod)
    rule = lrkit.rule(lhs, *prod)

Has following operations defined:

    len(rule)
    rule[n]

## SnError

The syntax error exception.

    sn = SnError(message, start, stop)
    sn.message
    sn.start
    sn.stop

## Canonical parser table generator

Produces the canonical LR(1) parser tables for the parser in the lrkit.

    results = lrkit.canonical.simulate(rules, accept)

    for terminal in results.terminals:
        pass

    for symbol, info in results.nonterminals.items():
        for terminal in info.first:
            pass
        if info.empty:
            pass
        for rule in info.rules:
            pass

    for row in results.table:
        for symbol, obj in row:
            if isinstance(obj, lrkit.Rule):
                if obj.lhs == None:
                    pass # accept
                else:
                    pass # reduce
            elif isinstance(obj, set):
                pass # an item in results.conflicts
            else:
                pass # shift

    for kernelset in results.kernelset:
        for item in kernelset:
            item.rule
            item.index
            item.ahead

    for index, symbol, objects in results.conflicts:
        results.kernelset[index]
        results.table[index]
        for obj in objects:
            if isinstance(obj, lrkit.Rule):
                if obj.lhs == None:
                    pass # accept
                else:
                    pass # reduce
            else:
                results.kernelset[obj]
                results.table[obj]

Rules is a list of rules.
Accept is a symbol in that rulesheet.

Most of the results can be discarded after you've
studied the conflicts list. If you're explorative, you might
use this to implement GLR parser, or parse with ambiguous
grammars.

## Tokenizer

    tokens = lrkit.tokenize(fd, specials, index=0)
    for token in tokens:
        token.start
        token.stop
        token.group
        token.value

The specials is a dictionary of symbols accepted as special symbols.
Every special symbol must have a prefix. eg. {'=':"eqv", '==':"eq"}
The specials rewrite themselves as unique token groups.

Token groups that are recognised:

    indent
    dedent
    newline
    symbol      # starts with letters or _, can contain numbers.
    real        # starts with a number and contains a dot.
    integer     # starts with a number, if second character is x, the first number is interpreted as base sign. 0xF00 2x101 8x777
    string      # starts with " or ', ends with started symbol, may contain escaped characters \" or \'. Escape symbols not interpreted by tokenizer.
    attribute   # starts with a dot, continues with symbol.

Errors that may be produced:

    SnError("inconsistent indentation")
    SnError("unterminated string")
    SnError("unexpected character")