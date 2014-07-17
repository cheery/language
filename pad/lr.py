def main():
    terminals = {"*", "+", "0", "1"}
    rules = [
        rule("E", "E", "*", "B"),
        rule("E", "E", "+", "B"),
        rule("E", "B"),
        rule("B", "0"),
        rule("B", "1"),
    ]
    table, analysis = build_canonical(terminals, rules, accept="E")

    pretty_print_itemsets(analysis)
    print("")
    print("")
    pretty_print_table(analysis, table)

def build_canonical(terminals, rules, accept):
    analysis = analyse(terminals, rules)

    init    = rule("acc", accept)
    first   = frozenset({Item(init, 0, eof)})
    kernels  = {first: 0}
    kernelsets = [first]

    table = [{}]

    for i, kernel in enumerate(kernelsets):
        closure = analysis.closure_of(kernel)
        sets = {}
        for item in closure:
            if item.reduced:
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
            table[i][sym] = index

    analysis.kernelsets = kernelsets
    analysis.terminals = terminals
    analysis.rules     = rules
    analysis.init      = init
    return table, analysis

def analyse(terminals, rules):
    symbols = set()
    rulesets = {}
    for rule in rules:
        sym = rule.lhs
        first = rule[0]
        if sym not in symbols:
            symbols.add(sym)
            rulesets[sym] = set()
        rulesets[sym].add(rule)
    return Analysis(symbols, rulesets)

class Analysis:
    def __init__(self, symbols, rulesets):
        self.symbols = symbols
        self.rulesets = rulesets

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

class Eof:
    def __str__(self):
        return "eof"

eof = Eof()

if __name__=='__main__':
    main()
