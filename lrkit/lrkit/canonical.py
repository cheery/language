from .rule import Rule, Accept

def simulate(rules, accept):
    terminals, nonterminals = analyse(rules)
    init = Accept([accept])
    initset = frozenset({Item(init, 0, None)})
    kernelindices = {initset:0}
    kernelsets = [initset]
    conflicts  = []
    table      = [dict()]
    for i, kernel in enumerate(kernelsets):
        closure_sets, reduceset = closure_of(kernel, nonterminals)
        for item in reduceset:
            if item.ahead in table[i]:
                add_conflict(conflicts, table, i, item.ahead, item.rule)
            else:
                table[i][item.ahead] = item.rule
        for symbol, itemset in closure_sets.items():
            kernelset = frozenset(itemset)
            if kernelset in kernelindices:
                index = kernelindices[kernelset]
            else:
                index = kernelindices[kernelset] = len(kernelsets)
                kernelsets.append(kernelset)
                table.append(dict())
            if symbol in table[i]:
                add_conflict(conflicts, table, i, symbol, index)
            else:
                table[i][symbol] = index
    return Results(rules, init, terminals, nonterminals, table, kernelsets, conflicts)

def closure_of(kernel, nonterminals):
    closure_sets = {}
    reduceset = set()
    found     = set(kernel)
    unvisited = list(kernel)
    def queue(item):
        if item not in found:
            found.add(item)
            unvisited.append(item)
    while len(unvisited) > 0:
        item = unvisited.pop(-1)
        if item.reduced:
            reduceset.add(item)
            continue
        expect = item.expect
        follow = follow_of(item, nonterminals, 1)
        if expect in closure_sets:
            closure = closure_sets[expect]
        else:
            closure = closure_sets[expect] = set()
        shift  = item.shift()
        closure.add(shift)
        if expect in nonterminals:
            info   = nonterminals[expect]
            for rule in info.rules:
                for ahead in follow:
                    queue(Item(rule, 0, ahead))
    return closure_sets, reduceset

def follow_of(item, nonterminals, offset):
    anticipate = item.anticipate(offset)
    if anticipate in nonterminals:
        info = nonterminals[anticipate]
        if info.empty:
            return info.first | follow_of(item, nonterminals, offset+1)
        else:
            return info.first
    else:
        return {anticipate}

def add_conflict(conflicts, table, i, symbol, obj):
    conflict = table[i][symbol]
    if isinstance(conflict, set):
        conflict.add(obj)
    else:
        conflict = table[i][symbol] = {conflict, obj}
        conflicts.append((i, symbol, conflict))

def analyse(rules):
    terminals = set()
    results   = {}
    for rule in rules:
        symbol = rule.lhs
        if symbol in results:
            result = results[symbol]
        else:
            result = results[symbol] = Analysis()
        result.rules.append(rule)
        result.empty |= (len(rule) == 0)
    for symbol, result in results.items():
        for rule in result.rules:
            for cell in rule:
                if cell not in results:
                    terminals.add(cell)
    changed = True
    while changed:
        changed = False
        for symbol, result in results.items():
            n = len(result.first)
            for rule in result.rules:
                e = 0
                for cell in rule:
                    if cell not in results:
                        result.first.add(cell)
                        break
                    cellinfo = results[cell]
                    result.first.update(cellinfo.first)
                    if not cellinfo.empty:
                        break
                    e += 1
                if e == len(rule) and not result.empty:
                    result.empty = True
                    changed = True
            changed |= n < len(result.first)
    return terminals, results

class Analysis:
    def __init__(self):
        self.first = set()
        self.empty = False
        self.rules = []

class Item:
    def __init__(self, rule, index, ahead):
        self.rule = rule
        self.index = index
        self.ahead = ahead

    def __hash__(self):
        return hash((type(self), self.rule, self.index, self.ahead))

    def __eq__(self, other):
        return type(self) == type(other) and self.rule == other.rule and self.index == other.index and self.ahead == other.ahead

    @property
    def reduced(self):
        return self.index == len(self.rule)

    def shift(self):
        return Item(self.rule, self.index+1, self.ahead)

    @property
    def expect(self):
        if self.index < len(self.rule):
            return self.rule[self.index]
        else:
            return self.ahead

    def anticipate(self, offset):
        if self.index + offset < len(self.rule):
            return self.rule[self.index + offset]
        else:
            return self.ahead

    def __repr__(self):
        head = list(self.rule[:self.index])
        tail = list(self.rule[self.index:])
        both = head + ['.'] + tail
        return "{} -> {} | {}".format(self.rule.lhs, ' '.join(both), self.ahead)

class Results:
    def __init__(self, rules, init, terminals, nonterminals, table, kernelsets, conflicts):
        self.rules = rules
        self.init  = init
        self.terminals = terminals
        self.nonterminals = nonterminals
        self.table = table
        self.kernelsets = kernelsets
        self.conflicts = conflicts
