from .snerror import SnError

def tokenize(fd, specials, index=0):
    ah = CharacterLookahead(fd, index)
    indent = 0
    layers = []
    newlines = False
    while ah.ch != '':
        while ah.ch == ' ':
            ah.advance()
        if ah.ch == '#':
            while ah.ch != '\n':
                ah.advance()
        if ah.ch == '\n':
            ah.advance()
            spaces = 0
            while ah.ch == ' ':
                spaces += 1
                ah.advance()
            if ah.ch != '\n' and ah.ch != '#' and ah.ch:
                if spaces > indent:
                    indent = spaces
                    if newlines:
                        layers.append(indent)
                        yield Token(ah.index, ah.index, "indent", "")
                else:
                    while spaces < indent:
                        yield Token(ah.index, ah.index, "dedent", "")
                        indent = layers.pop(-1)
                    if spaces > indent:
                        raise SnError("inconsistent indentation", ah.index, ah.index)
                    if newlines:
                        yield Token(ah.index, ah.index, "newline", "")
        elif issym(ah.ch):
            string = ah.advance()
            start  = ah.index
            while issym(ah.ch) or isnum(ah.ch):
                string += ah.advance()
            if string in specials:
                yield Token(start, ah.index, specials[string], string)
            else:
                yield Token(start, ah.index, "symbol", string)
        elif isnum(ah.ch):
            string = ah.advance()
            start  = ah.index
            base   = 10
            if ah.ch == 'x':
                base   = int(string) or 16
                string = ''
                ah.advance()
            while isnum(ah.ch) or issym(ah.ch):
                string += ah.advance()
            if base == 10 and ah.ch == '.':
                string += ah.advance()
                while isnum(ah.ch):
                    string += ah.advance()
                yield Token(start, ah.index, "real", float(string))
            else:
                yield Token(start, ah.index, "integer", int(string, base))
        elif ah.ch == '"' or ah.ch == "'":
            start  = ah.index
            string = ah.advance()
            while ah.ch != string[0]:
                if ah.ch == '\\':
                    string += ah.advance()
                string += ah.advance()
                if ah.ch == '':
                    raise SnError("unterminated string", start, ah.index)
            string += ah.advance()
            yield Token(start, ah.index, "string", string)
        else:
            start = ah.index
            string = ah.advance()
            if string == '.' and issym(ah.ch):
                while issym(ah.ch) or isnum(ah.ch):
                    string += ah.advance()
                yield Token(start, ah.index, "attribute", string)
            elif string in specials:
                while string + ah.ch in specials:
                    string += ah.advance()
                yield Token(start, ah.index, specials[string], string)
            else:
                raise SnError("unexpected character {!r}".format(string), start, ah.index)
        newlines = True
    for layer in layers:
        yield Token(ah.index, ah.index, "dedent", "")

def issym(ch):
    return ch.isalpha() or ch == '_'

def isnum(ch):
    return ch.isdigit()

class CharacterLookahead:
    def __init__(self, fd, index):
        self.fd    = fd
        self.index = index
        self.ch    = ''
        self.advance()

    def advance(self):
        rh, self.ch = self.ch, self.fd.read(1)
        self.index += len(self.ch)
        return rh

class Token(object):
    def __init__(self, start, stop, group, value):
        self.start = start
        self.stop = stop
        self.group = group
        self.value = value

    def __repr__(self):
        return '<{0.group} {0.value!r} {0.start}:{0.stop}>'.format(self)
