from .canonical import simulate
from .parser import Parser
from .rule import Rule
from .tokenizer import tokenize
from .errors import SnError

specials = {
    "->": "arrow",
    "[": "lb",
    "]": "rb",
}

bindings = {}
def bind(lhs, *prod):
    def _bind(fn):
        bindings[Rule(lhs, prod)] = fn
        return fn
    return _bind

@bind("Grammar", "Binding")
def grammar_begin(nodes):
    command, pick, rule = nodes[0]
    return {rule: (command, pick)}

@bind("Grammar", "Grammar", "newline", "Binding")
def grammar_append(nodes):
    grammar = nodes[0]
    command, pick, rule = nodes[2]
    grammar[rule] = (command, pick)
    return grammar

@bind("Binding", "symbol", "Pick", "Rule")
def grammar_binding(nodes):
    return nodes

@bind("Rule", "symbol", "arrow", "Symbols")
def grammar_rule(nodes):
    return Rule(nodes[0], nodes[2])

@bind("Symbols")
@bind("Integers")
def list_begin(nodes):
    return []

@bind("Symbols", "Symbols", "symbol")
@bind("Integers", "Integers", "integer")
def list_append(nodes):
    lst, item = nodes
    lst.append(item)
    return lst

@bind("Pick")
def pick_all(nodes):
    return None

@bind("Pick", "integer")
def pick_one(nodes):
    return nodes[0]

@bind("Pick", "lb", "Integers", "rb")
def pick_many(nodes):
    return nodes[1]

def chew(rule, pos, data):
    return bindings[rule](data)

results = simulate(bindings, "Grammar")
assert len(results.conflicts) == 0
parser = Parser(results, chew)

def parse(fd, index=0):
    index = 0
    parser.reset()
    for token in tokenize(fd, specials):
        parser.step(token.start, token.stop, token.group, token.value)
        index = token.stop
    return parser.step(index, index, None, None)

def load(path, accept):
    with open(path) as fd:
        grammar = parse(fd)
    results = simulate(grammar, accept)
    if len(results.conflicts) == 0:
        return results, instantiator(grammar, results)
    else:
        return results, None

def instantiator(grammar, results):
    def _instantiate(obj, *args):
        def reducer(rule, pos, expr):
            name, pick = grammar[rule]
            return getattr(obj, name)(pos, pick_out(pick, expr), *args)
        return Parser(results, reducer)
    return _instantiate

def pick_out(pick, expr):
    if pick is None:
        return expr
    if isinstance(pick, int):
        return expr[pick]
    return [expr[i] for i in pick]
