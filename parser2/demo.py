from lrkit import Parser, Rule, rule, canonical, tokenize, SnError

specials = {
        "*": "*",
        "/": "/",
        "%": "%",
        "+": "+",
        "-": "-",
        "0": "0",
        "(": "(",
        ")": ")",
        ",": ",",
}

rules = [
    rule("Program", "Expr"),
    rule("Program"),
    rule("Expr",    "Sum"),
    rule("Sum",     "Sum", "SumOp", "Product"),
    rule("Sum",     "Product"),
    rule("Product", "Product", "ProductOp", "Term"),
    rule("Product", "Term"),

    rule("ProductOp", "*"),
    rule("ProductOp", "/"),
    rule("ProductOp", "%"),
    rule("SumOp", "+"),
    rule("SumOp", "-"),
    rule("Term", "symbol"),
    rule("Term", "(", "ExprList", ")"),

    rule("ExprList"),
    rule("ExprList", "Expr"),
    rule("ExprList", "ExprList", ",", "Expr"),
]

def visit(rule, pos, data):
    print "reduction", rule, pos, data
    return None

results = canonical.simulate(rules, "Program")
print "conflicts:", len(results.conflicts)
for row, symbol, conflict in results.conflicts:
    print "row", row
    print "symbol", symbol
    for thing in conflict:
        if isinstance(thing, Rule):
            print "  ", thing
        else:
            print "  ", thing

parser = Parser(results, visit)
from sys import stdin

while True:
    try:
        index = 0
        for token in tokenize(stdin.readline(), specials):
            parser.step(token.start, token.stop, token.group, token.value)
            index = token.stop
        result = parser.step(index, index, None, None)
    except SnError, e:
        print e 
        parser.reset()
