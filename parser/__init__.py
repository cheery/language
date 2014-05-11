from lookahead import CharacterLookAhead, LookAhead
from structures import Constant, Struct

specials = {
    ',': 'comma',
    '(': 'leftparen', ')': 'rightparen', '[': 'leftbracket', ']': 'rightbracket',
}

operators = set([
    'or', 'and', 'not',
    '!', ':', '=', '-', '+', '*', '/', '<>', '==', '!=', '->',
    '<', '<=', '>', '>=', '|', '^', '&', '<<', '>>', '//', '%', '~',
    '.', '.;', '.:', ':.', ';', '@', '::', '..'
])

infix_operators = {
    'or': 20,
    'and': 30,
    '<': 40, '<=': 40, '>': 40, '>=': 40,
    '<>': 40, '!=': 40, '==': 40,
    '|': 50,
    '^': 60,
    '&': 70,
    '<<': 80, '>>': 80,
    '+': 90, '-': 90,
    '*': 100, '/': 100, '//': 100, '%': 100,
}
prefix_operators = {
    'not': 30,
    '+': 110, '-': 110, '~': 110,
}
right_binding = set(['or', 'and', '<', '<=', '>', '>=', '<>', '!=', '=='])

def main():
    source = 'tokens'
    fd = open(source, 'r')
    for structure in parse(fd.read().decode('utf-8'), source, debug=True):
        print structure

def parse_file(source, debug=False):
    with open(source, 'r') as fd:
        return parse(fd.read().decode('utf-8'), source, debug)

def parse(source, filename=None, debug=False):
    cla = CharacterLookAhead(source)
    if debug:
        tla = LookAhead(tokenlogger(tokenize(cla), filename))
    else:
        tla = LookAhead(tokenize(cla))
    if ahead(tla, 'newline'):
        expect(tla, 'newline')
    if not tla.empty:
        return parse_block(tla)
    else:
        return ()

def parse_block(tla):
    yield parse_sentence(tla)
    while not tla.empty and not ahead(tla, 'dedent'):
        expect(tla, 'newline')
        yield parse_sentence(tla)

def parse_sentence(tla, required=True):
    location = tla.location
    head = parse_word(tla, required, 10)
    if head is None:
        return
    if ahead_string(tla, '=', ':'):
        operator = Constant(tla.location, 'operator', expect(tla, 'operator').string)
        blocks = find_placeholders(head)
        if len(blocks) > 0:
            raise Exception("%s: not allowed on toplevel lhs side of '=' or ':'." % linecol(blocks[0].location))
        return Struct(location, 'infix', operator, head, parse_sentence(tla))
    sentence = Struct(location, 'sentence', head)
    for word in repeated(parse_word, tla, False, 0):
        sentence.append(word)
    blocks = find_placeholders(sentence)
    if ahead(tla, 'indent'):
        expect(tla, 'indent')
        if len(blocks) > 1:
            raise Exception("%s: cannot fill this placeholder" % linecol(blocks[0].location))
        elif len(blocks) > 0:
            block = blocks[0]
            for item in parse_block(tla):
                block.append(item)
        else:
            sentence.append(Struct(location, 'block', *parse_block(tla)))
        expect(tla, 'dedent')
    elif len(blocks) > 0:
        raise Exception("%s: cannot fill this placeholder" % linecol(blocks[0].location))
    return sentence

def find_placeholders(node, out=None):
    out = [] if out is None else out
    if node.group == 'block':
        out.append(node)
    elif isinstance(node, Struct):
        for item in node:
            if item.group == 'sentence' or item.group == 'function':
                continue
            find_placeholders(item, out)
    return out

def parse_word(tla, required, precedence):
    location = tla.location
    expr = parse_slot(tla, required, precedence)
    if expr is None:
        return
    if ahead(tla, 'comma'):
        expr = Struct(location, 'tuple+', expr)
        while ahead(tla, 'comma'):
            expect(tla, 'comma')
            expr.append(parse_slot(tla, True, precedence))
    return expr

def parse_arglist(tla, location, *head):
    arglist = Struct(location, 'tuple', *head)
    slot = parse_slot(tla, False, 0)
    if slot is None:
        return arglist
    arglist.append(slot)
    while ahead(tla, 'comma'):
        expect(tla, 'comma')
        arglist.append(parse_slot(tla, True, 0))
    return arglist

def parse_slot(tla, required, precedence):
    if precedence >= 10:
        return parse_slice(tla, required, precedence)
    location = tla.location
    slic = parse_slice(tla, required, precedence)
    if ahead_string(tla, '=', ':'):
        operator = Constant(tla.location, 'operator', expect(tla, 'operator').string)
        return Struct(location, 'infix', operator, slic, parse_slot(tla, required, precedence))
    return slic

def parse_slice(tla, required, precedence):
    location = tla.location
    expr = parse_expr(tla, False, precedence)
    if expr is None:
        condition = lambda: tla.value.near == tla.value.balanced
    else:
        condition = lambda: tla.value.balanced
    if ahead_string(tla, '.:', ':.') and condition():
        mode  = ('incr' if tla.step().string == '.:' else 'decr')
        start = expr
        stop  = parse_expr(tla, False, precedence)
        if start is None:
            start = Constant(tla.location, 'symbol', 'null')
        if stop is None:
            stop  = Constant(tla.location, 'symbol', 'null')
        stride = Constant(tla.location, 'symbol', 'null')
        step   = Constant(tla.location, 'symbol', 'null')
        if ahead_string(tla, '::') and tla.value.balanced:
            expect(tla, 'operator')
            stride = parse_expr(tla, False, precedence)
        if ahead_string(tla, '..') and tla.value.balanced:
            expect(tla, 'operator')
            step = parse_expr(tla, False, precedence)
        return Struct(location, mode, start, stop, stride, step)
    if expr is None:
        return parse_expr(tla, required, precedence)
    return expr

def parse_expr(tla, required, precedence):
    location = tla.location
    if ahead(tla, 'operator') and tla.value.string in prefix_operators:
        if tla.value.near <> tla.value.balanced and tla.value.string <> 'not':
            raise Exception("%s: This is not C" % linecol(tla.location))
        operator = Constant(tla.location, 'operator', expect(tla, 'operator').string)
        expr = Struct(location, 'prefix', operator, parse_expr(tla, True, prefix_operators[operator.value]))
    else:
        expr = parse_fullterm(tla, required)
    while ahead(tla, 'operator') and tla.value.string in infix_operators:
        prec = infix_operators[tla.value.string]
        if prec <= precedence or not tla.value.balanced:
            break
        prex = prec - (tla.value.string in right_binding)
        operator = Constant(tla.location, 'operator', expect(tla, 'operator').string)
        expr = Struct(location, 'infix', operator, expr, parse_expr(tla, True, prex))
    return expr

def parse_fullterm(tla, required):
    term = parse_term(tla, required)
    while not tla.empty and tla.value.near:
        location = tla.location
        if ahead(tla, 'attribute'):
            string = expect(tla, 'attribute').string
            term = Struct(location, 'attribute', term, Constant(location, 'attribute', string[1:]))
        elif ahead(tla, 'leftparen'):
            expect(tla, 'leftparen')
            term = parse_arglist(tla, location, term)
            term.group = 'call'
            expect(tla, 'rightparen')
        elif ahead(tla, 'leftbracket'):
            expect(tla, 'leftbracket')
            term = parse_arglist(tla, location, term)
            term.group = 'index'
            expect(tla, 'rightbracket')
        elif ahead_string(tla, ';'):
            expect(tla, 'operator')
            term = Struct(location, 'call', term, Struct(location, "block"))
        elif ahead_string(tla, '.;'):
            expect(tla, 'operator')
            term = Struct(location, 'attribute', term, Struct(location, "block"))
        else:
            break
    return term

def parse_term(tla, required):
    location = tla.location
    if ahead(tla, 'symbol'):
        return Constant(location, 'symbol', expect(tla, 'symbol').string)
    elif ahead_string(tla, ';'):
        expect(tla, 'operator')
        return Struct(location, 'block')
    elif ahead(tla, 'string'):
        string = expect(tla, 'string').string
        return Constant(location, 'string', string[1:-1])
    elif ahead(tla, 'number'):
        string = expect(tla, 'number').string
        if ahead(tla, 'flot'):
            if not tla.value.near:
                raise Exception("%s: decimal expression supposed to be typed with no spacing" % (linecol(tla.location)))
            string += expect(tla, 'flot').string
            return Constant(location, 'float', string)
        return Constant(location, 'number', string)
    elif ahead(tla, 'leftparen'):
        expect(tla, 'leftparen')
        if ahead(tla, 'operator'):
            operator = Constant(tla.location, 'operator', expect(tla, 'operator').string)
            expect(tla, 'rightparen')
            return operator
        else:
            term = parse_arglist(tla, location)
            expect(tla, 'rightparen')
        if ahead_string(tla, '->'):
            expect(tla, 'operator')
            blocks = find_placeholders(term)
            if len(blocks) > 0:
                raise Exception("%s: not allowed inside function argument list" % linecol(blocks[0].location))
            return parse_function(tla, location, term)
        elif len(term) == 1 and term[0].group != 'block':
            return term[0]
        else:
            term.group = 'tuple'
            return term
    elif ahead(tla, 'leftbracket'):
        expect(tla, 'leftbracket')
        arglist = parse_arglist(tla, location)
        arglist.group = 'list'
        expect(tla, 'rightbracket')
        return arglist
    elif ahead_string(tla, '->'):
        expect(tla, 'operator')
        return parse_function(tla, location, Struct(location, 'arglist'))
    elif ahead_string(tla, '@'):
        expect(tla, 'operator')
        term = Constant(location, 'self', None)
        if ahead(tla, '.'):
            raise Exception("%s: you're serious?" % (linecol(tla.location)))
        if ahead(tla, 'symbol') and tla.value.near:
            term = Struct(location, 'attribute', term, Constant(tla.location, 'attribute', expect(tla, 'symbol').string))
        return term
    elif required:
        raise Exception("%s: a term is missing after '%s'" % (linecol(tla.previous_location), tla.previous_value.string))

def parse_function(tla, location, func):
    func.group = 'function'
    sentence = parse_sentence(tla, False)
    if sentence is not None:
        func.append(sentence)
    elif ahead(tla, 'indent'):
        expect(tla, 'indent')
        func.append(Struct(location, 'block', *parse_block(tla)))
        expect(tla, 'dedent')
    return func

def repeated(fn, *args):
    node = fn(*args)
    while node is not None:
        yield node
        node = fn(*args)

def ahead_string(tla, *strings):
    return not tla.empty and tla.value.string in strings

def ahead(tla, *groups):
    return not tla.empty and tla.value.group in groups

def expect(tla, group, string=None):
    if tla.empty:
        raise Exception(u"%s: expected %s, but stream is empty" % (linecol(tla.location), repr_expect(group, string)))
    value = tla.value
    valid = (value.group == group) and string is None or value.string == string
    if not valid:
        raise Exception(u"%s: expected %s, got %r(%s)" % (linecol(tla.location), repr_expect(group, string), value.string, value.group))
    return tla.step()

def repr_expect(group, string):
    if string is None:
        return "(%s)" % group
    else:
        return "%r(%s)" % (string, group)

def tokenlogger(tokens, source):
    for token in tokens:
        print "%s:%s: %r (%s) near=%r balanced=%r" % (
            source, linecol(token.location),
            token.string, token.group, token.near, token.balanced
        )
        yield token

class tokenize(object):
    def __init__(self, cla):
        self.cla = cla
        self.indent = 0
        self.layers = [-1]

    def __iter__(self):
        return self

    def next(self):
        cla = self.cla
        if cla.empty and len(self.layers) > 1:
            self.indent = self.layers.pop(-1)
            return Token(cla.location, '', 'dedent')
        if cla.empty:
            raise StopIteration
        if self.indent <  self.layers[-1]:
            indent = self.layers.pop(-1)
            if self.indent != self.layers[-1]:
                return Token(cla.location, '', 'badline')
            return Token(cla.location, '', 'dedent')
        if self.indent == self.layers[-1]:
            indent = self.layers.pop(-1)
            return Token(cla.location, '', 'newline')
        while cla.value == ' ':
            cla.step()
        if cla.value == '#':
            while cla.value != '\n':
                cla.step()
        if cla.value == '\n':
            cla.step()
            indent = 0
            while cla.value == ' ':
                indent += 1
                cla.step()
            if cla.value == '\n' or cla.value == '#':
                return self.next()
            if cla.empty:
                return self.next()
            if indent > self.indent:
                self.layers.append(self.indent)
                self.indent = indent
                return Token(cla.location, '', 'indent')
            elif indent == self.indent:
                return Token(cla.location, '', 'newline')
            else:
                self.indent = indent
                return Token(cla.location, '', 'dedent')
        location = cla.location
        near = (cla.previous_value != ' ')
        string = ""
        if issym(cla.value):
            while issym(cla.value):
                string += cla.step()
            balanced = near <> (cla.value == ' ')
            if string in operators:
                return Token(location, string, 'operator', near, balanced)
            if string[:1].isdigit():
                return Token(location, string, 'number', near, balanced)
            return Token(location, string, 'symbol', near, balanced)
        if cla.value in "\"'":
            terminator = string = cla.step()
            while cla.value != terminator:
                string += cla.step()
                if cla.value == '\\':
                    string += cla.step()
            string += cla.step()
            balanced = near <> (cla.value == ' ')
            return Token(location, string, 'string', near, balanced)
        string = cla.step()
        if string == '.':
            while issym(cla.value):
                string += cla.step()
            if string != '.':
                balanced = near <> (cla.value == ' ')
                if isnum(string[1]):
                    return Token(location, string, 'flot', near, balanced)
                return Token(location, string, 'attribute', near, balanced)
        if string in operators:
            while not cla.empty and string + cla.value in operators:
                string += cla.step()
            balanced = near <> (cla.value == ' ')
            return Token(location, string, 'operator', near, balanced)
        balanced = near <> (cla.value == ' ')
        if string in specials:
            return Token(location, string, specials[string], near, balanced)
        return Token(location, string, 'unknown', near, balanced)

def issym(ch):
    return ch.isalnum() or ch == '_'

def isnum(ch):
    return ch.isdigit()

def linecol(location):
    return "line %i, col %i" % (location >> 8, location & 255)

class Token(object):
    def __init__(self, location, string, group, near=False, balanced=False):
        self.location = location
        self.string = string
        self.group = group
        self.near = near
        self.balanced = balanced

    def __repr__(self):
        return '<Token %r>' % self.string

if __name__=='__main__':
    main()
