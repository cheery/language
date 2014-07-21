from lrkit import Rule, rule, canonical

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
    rule("Term", "0"),
    rule("Term", "(", "ExprList", ")"),

    rule("ExprList"),
    rule("ExprList", "Expr"),
    rule("ExprList", "ExprList", ",", "Expr"),
]

results = canonical.simulate(rules, "Program")

#print results.terminals
#for symbol, result in results.nonterminals.items():
#    print "first:", result.first
#    print "allow empty:", result.empty
#    print "rules:"
#    for rule in result.rules:
#        print "   ", rule
#    print ""
#
print "conflicts:", len(results.conflicts)
for row, symbol, conflict in results.conflicts:
    print "row", row
    print "symbol", symbol
    for thing in conflict:
        if isinstance(thing, Rule):
            print "  ", thing
        else:
            print "  ", thing
#print "kernelsets:", results.kernelsets
