# -*- coding: utf-8 -*-
"""
    LR(1) parser generator

    Here you have a parser generator that's fairly easy to use and small.
    It produces canonical parser tables so the resulting parsers are fairly large.

    How to use:
        from lr import rule, build_canonical, Parser

    * create bunch of rules like this (there must be at least 1 item in a rule):
        rule(lhs, *items)
    * build a canonical table, check that there are no errors:
        table, analysis, errors = build_canonical(rules, accept_symbol)
        assert len(errors) == 0
    * create a function 'visit'
        def visit(rule, data, start, stop, *args):
            return data
    * create the parser
        parser = Parser(table, visit, *args)
    * feed tokens to the parser:
        parser.step(name, value, start, stop)
    * feed eof to the parser:
        result = parser.done(stop)
    * catch SynError, reset or discard the parser if you don't switch the input:
        parser.reset()
    * the grammar errors are like this, a, b are either shift numbers or rules.
        (row, symbol, a, b)
        you won't probably get shift/shift errors.
"""
def main():
    rules = [
        rule("Products", "Products", "*", "Terms"),
        rule("Products", "Products", "/", "Terms"),
        rule("Sums",     "Sums",     "+", "Products"),
        rule("Sums",     "Products"),
        rule("Products", "Terms"),
        rule("Terms", "Terms", "(", "ProductList", ")"),
        rule("Terms", "Terms", "(", ")"),
        rule("ProductList", "Products"),
        rule("ProductList", "ProductList", ",", "Products"),
        rule("Terms", "0"),
    ]
    def visit(rule, expr, start, stop):
        print("reduced into", expr, "with", rule, "at", start, stop)
        #i = rules.index(rule)
        #if i == 0:
        #    return expr
        #if i == 1:
        #    return expr
        #if i == 3 or i == 2:
        #    return expr[0]
        #if i == 4:
        #    return int(expr[0])
        #if i == 5:
        #    return int(expr[0])

        return expr
    table, analysis, errors = build_canonical(rules, accept="Sums")
    print(errors)
    assert len(errors) == 0
    #pretty_print_itemsets(analysis)
    #print("")
    #print("")
    pretty_print_table(analysis, table)
    parser = Parser(table, visit)
    import sys
    while True:
        sys.stdout.write('> ')
        sys.stdout.flush()
        text = sys.stdin.readline().strip()
        try:
            index = 0
            for token in text.split(' '):
                if token != '':
                    parser.step(token, token, index, index + 0.5)
                index += 1
            result = parser.done(index, index)
        except SynError as syn:
            print(syn)
            parser.reset()
        else:
            print(result)

class Parser:
    def __init__(self, table, visitor, *args):
        self.state = 0
        self.table = table
        self.data  = []
        self.stack = []
        self.starts = []
        self.stops  = []
        self.result = None
        self.visitor = visitor
        self.args    = args

    @property
    def idle(self):
        return self.state == 0 and len(self.stack) == 0

    def step(self, name, value, start, stop):
        action = self.table[self.state].get(name, 0)
        while isinstance(action, Rule):
            expr = []
            r_start = self.starts[-1]
            r_stop  = self.stops[-1]
            for n in range(len(action)):
                expr.append(self.data.pop(-1))
                self.state = self.stack.pop(-1)
                r_start = self.starts.pop(-1)
                self.stops.pop(-1)
            expr.reverse()
            if action.lhs == acc:
                assert len(self.data) == 0
                assert len(self.stack) == 0
                assert len(self.starts) == 0
                assert len(self.stops) == 0
                return expr[0]
            goto = self.table[self.state].get(action.lhs, 0)
            self.data.append(self.visitor(action, expr, r_start, r_stop, *self.args))
            self.stack.append(self.state)
            self.starts.append(r_start)
            self.stops.append(r_stop)
            self.state = goto
            action = self.table[self.state].get(name, 0)
        if action == 0:
            expects = ', '.join(map(str, self.table[self.state]))
            raise SynError("got {} but expected {}".format(name, expects))
        self.data.append(value)
        self.stack.append(self.state)
        self.starts.append(start)
        self.stops.append(stop)
        self.state = action

    def reset(self):
        self.state = 0
        self.data  = []
        self.stack = []
        self.starts = []
        self.stops  = []

    def done(self, start, stop=None):
        if stop is None:
            stop = start
        return self.step(eof, eof, start, stop)

class SynError(Exception):
    pass

def build_canonical(rules, accept):
    analysis = analyse(rules)

    init    = rule(acc, accept)
    first   = frozenset({Item(init, 0, eof)})
    kernels = {first: 0}
    kernelsets = [first]
    errors = []

    table = [{}]

    for i, kernel in enumerate(kernelsets):
        closure = analysis.closure_of(kernel)
        sets = {}
        for item in closure:
            if item.reduced:
                if item.ahead in table[i]:
                    errors.append((i, item.ahead, table[i][item.ahead], item.rule))
                table[i][item.ahead] = item.rule
                continue
            if item.sym not in sets:
                sets[item.sym] = set()
            sets[item.sym].add(item.shifted)
        for sym, itemset in sets.items():
            itemset = frozenset(itemset)
            if itemset in kernels:
                index = kernels[itemset]
            else:
                index = kernels[itemset] = len(kernels)
                kernelsets.append(itemset)
                table.append({})
            if sym in table[i]:
                errors.append((i, sym, table[i][sym], index))
            table[i][sym] = index

    analysis.kernelsets = kernelsets
    analysis.rules     = rules
    analysis.init      = init
    return table, analysis, errors

def analyse(rules):
    symbols = set()
    rulesets = {}
    terminals = set()
    for rule in rules:
        sym = rule.lhs
        first = rule[0]
        if sym not in symbols:
            symbols.add(sym)
            rulesets[sym] = set()
        rulesets[sym].add(rule)
    for rule in rules:
        for cell in rule:
            if cell not in rulesets:
                terminals.add(cell)
    return Analysis(symbols, rulesets, terminals)

class Analysis:
    def __init__(self, symbols, rulesets, terminals):
        self.symbols = symbols
        self.rulesets = rulesets
        self.terminals = terminals

    def build_closure(self, closure, item):
        if item not in closure:
            closure.add(item)
            for rule in self.rulesets.get(item.sym, ()):
                next_item = Item(rule, 0, item.ahead)
                self.build_closure(closure, next_item)

    def closure_of(self, itemset):
        closure = set()
        for item in itemset:
            if item.reduced:
                closure.add(item)
            else:
                self.build_closure(closure, item)
        return closure

def pretty_print_table(analysis, table):
    columns = list(analysis.terminals) + list(analysis.symbols) + [eof]
    h = " nro. | "
    tabsize = 5
    for col in columns:
        h += str(col).ljust(tabsize)
    print(len(h) * '-')
    print(h)
    print(len(h) * '-')
    for index, row in enumerate(table):
        s = "{:4}. | ".format(index)
        for i in columns:
            if i in row:
                item = row[i]
                if item == analysis.init:
                    item = "acc"
                if item in analysis.rules:
                    item = '-' + str(analysis.rules.index(item) + 1)
                s += str(item).ljust(tabsize)
            else:
                s += " " * tabsize
        print(s)

    print(len(h) * '-')
    for i, rule in enumerate(analysis.rules):
        print("{}. {}".format(-i-1, rule))

def pretty_print_itemsets(analysis):
    for i, itemset in enumerate(analysis.kernelsets):
        if i > 0:
            print("")
        print("itemset", i)
        for item in itemset:
            print(item)
        for item in analysis.closure_of(itemset):
            print("   ", item)

def rule(lhs, *prod):
    return Rule(lhs, prod)

class Rule:
    def __init__(self, lhs, prod):
        self.lhs = lhs
        self.prod = prod

    def __repr__(self):
        return "{} -> {}".format(self.lhs, ' '.join(map(str, self.prod)))

    def __getitem__(self, index):
        return self.prod[index]

    def __len__(self):
        return len(self.prod)

class Item:
    def __init__(self, rule, index, wait):
        self.rule  = rule
        self.index = index
        self.wait  = wait

    def __hash__(self):
        return hash((self.rule, self.index, self.wait))

    def __eq__(self, other):
        return all((
            self.rule == other.rule,
            self.index == other.index,
            self.wait == other.wait,
        ))

    @property
    def sym(self):
        if self.index < len(self.rule):
            return self.rule[self.index]

    @property
    def ahead(self):
        if self.index + 1 < len(self.rule):
            return self.rule[self.index + 1]
        return self.wait

    @property
    def reduced(self):
        return self.index == len(self.rule)


    @property
    def shifted(self):
        if self.index < len(self.rule):
            return Item(self.rule, self.index+1, self.wait)

    def __repr__(self):
        head = self.rule[:self.index]
        tail = self.rule[self.index:]
        return "{} → {}°{}  {}".format(self.rule.lhs, ' '.join(map(str, head)), ' '.join(map(str, tail)), self.wait)

class Enum:
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

eof = Enum('eof')
acc = Enum('acc')

if __name__=='__main__':
    main()
