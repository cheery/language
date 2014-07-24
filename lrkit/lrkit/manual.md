# LRKit User's Manual

Everything not described here isn't part of the API. If you need other
internals, tell about your needs in the github repository.

## Rule objects

Production rules that form a grammar when put into a list.

    rule = lrkit.Rule(lhs, prod)
    rule = lrkit.rule(lhs, *prod)
    acc  = lrkit.Accept(prod)

Has following operations defined:

    len(rule)
    rule[n]
    rule.lhs

## Errors

The syntax error exception.

    sn = SnError(message, start, stop)
    sn.message
    sn.start
    sn.stop

The tokenizing error exception, subclass of `SnError`

    sn = TokenError(message, start, stop)

The parsing error exception, subclass of `SnError`

    sn = ParseError(message, start, stop)

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

## Parser

    def visitor(rule, pos, value, *args):
        pass
    parser = lrkit.Parser(results, visitor, *args)
    if parser.idle:
        pass
    try:
        index = 0
        for token in tokens:
            parser.step(token.start, token.stop, token.group, token.value)
            index = token.stop
        result = parser.step(index, index, None, None)
    except lrkit.SnError, sn:
        print sn
        parser.reset()

The parser starts in idle state. The `parser.reset()` can be used to
recover the parser back into the idle state. This is proposed after
a syntax error if the parsing won't continue.

## Tokenizer

    tokens = lrkit.tokenize(fd, specials, index=0)
    for token in tokens:
        token.start
        token.stop
        token.group
        token.value

The specials is a dictionary of symbols accepted as special symbols.
For example: {'+':"add", '==':"eq"} The specials rewrite themselves
as unique token groups.

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

    TokenError("inconsistent indentation")
    TokenError("unterminated string")
    TokenError("unexpected character")

## Grammar Language

This is a separate part of the remaining. It is a grammar language loader.

    import lrkit.grammar

    results, attach = lrkit.grammar.load(path, accept, cache=True)
    assert len(results.conflicts) == 0
    parser = attach(class_or_module, *args)

    # class_or_module.command(pos, value, *args)

The grammar has a separate cache saver and loader which is able to
store your grammars. Unfortunately the format cannot cache the conflicts
or item sets, so it's use is excluded for the lrkit itself for now.

The grammar binds the rules to commands, and has a mini-syntax for removing
and ordering the match to fit the command.
Here's the grammar understood by the grammar loader, written in it's own language:

    grammar_new 0     Grammar -> Binding
    grammar_cat [0 2] Grammar -> Grammar newline binding
    binding           Binding -> symbol Pick Rule
    rule              Rule    -> symbol arrow Symbols

    list_begin   Symbols ->
    list_append  Symbols -> Symbols symbol

    list_begin   Integers ->
    list_append  Integers -> Integers integer

    # Passes the whole match as list
    pick_all     Pick -> 
    # Passes only single part
    pick_one  0  Pick -> integer
    # Passes parts from the match in given order as a list
    pick_many 1  Pick -> lbracket Integers rbracket
