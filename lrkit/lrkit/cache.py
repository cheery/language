from .rule import Rule, Accept

# the last four bytes here is a version string.
# It's not a good or proven format, so it is initially largest number.
# This means that this file format is not ready for other programs to be used.
cache_header = "\x89GRA\x0D\x0A\0x1A\0x0A\xff\xff\xff\xff"

def load_cache(path):
    with open(path, 'r') as fd:
        return read_cache(fd)

def read_cache(fd):
    if fd.read(len(cache_header)) != cache_header:
        raise Exception("Not a cache file")
    a = read_uint16(fd)
    b = read_uint16(fd)
    c = read_uint16(fd)
    strtab = [None] + fd.read(a).decode('utf-8').split('\x00')
    init   = Accept([strtab[read_uint16(fd)]])
    rules  = [init]
    grammar = {}
    for i in range(b):
        rule, bind = read_rule(fd, strtab)
        grammar[rule] = bind
        rules.append(rule)
    table = [read_row(fd, strtab, rules) for n in range(c)]
    return table, init, grammar

def read_rule(fd, strtab):
    name   = strtab[read_uint16(fd)]
    length = read_uint16(fd)
    lhs    = strtab[read_uint16(fd)]
    prod   = [strtab[read_uint16(fd)] for n in range(length)]
    picktype = read_uint16(fd)
    if picktype == 0:
        pick = None
    elif picktype == 1:
        pick = read_uint16(fd)
    else:
        pick = [read_uint16(fd) for n in range(picktype - 2)]
    return Rule(lhs, prod), (name, pick)

def read_row(fd, strtab, rules):
    length = read_uint16(fd)
    row = {}
    for n in range(length):
        key = strtab[read_uint16(fd)]
        value = read_uint16(fd)
        if value >= 0x8000:
            row[key] = value - 0x8000
        else:
            row[key] = rules[value]
    return row

def write_cache(path, results, grammar):
    assert len(results.conflicts) == 0
    with open(path, 'w') as fd:
        fd.write(encode_table(results, grammar))

def encode_table(results, grammar):
    strings  = []
    inverted = {}
    inverted[None] = 0
    def string_encode(string):
        if string not in inverted:
            inverted[string] = len(inverted)
            strings.append(string)
        return inverted[string]
    map(string_encode, results.nonterminals)
    map(string_encode, results.terminals)

    ruletab = []
    tabtab  = []

    ruletab.append(string_encode(results.init[0]))
    rules = list(grammar)
    for rule in rules:
        name, pick = grammar[rule]
        data = [string_encode(name), len(rule), string_encode(rule.lhs)] + map(string_encode, rule)
        if pick is None:
            data.append(0)
        elif isinstance(pick, int):
            data.append(1)
            data.append(pick)
        else:
            data.append(2 + len(pick))
            data.extend(pick)
        ruletab.extend(data)
    inverted[results.init] = 0
    for i, rule in enumerate(rules, 1):
        inverted[rule] = i

    for row in results.table:
        tabtab.append(len(row))
        for key, value in row.items():
            tabtab.append(inverted[key])
            if isinstance(value, int):
                tabtab.append(value + 0x8000)
            else:
                tabtab.append(inverted[value])

    strtab = '\x00'.join(strings).encode('utf-8')
    a = encode_uint16(len(strtab))
    b = encode_uint16(len(rules)) 
    c = encode_uint16(len(results.table))  
    return cache_header + a + b + c + strtab + ''.join(map(encode_uint16, ruletab + tabtab))

def encode_uint16(num):
    return chr(num >> 0 & 255) + chr(num >> 8 & 255)

def read_uint16(fd):
    return decode_uint16(fd.read(2))

def decode_uint16(data):
    return ord(data[0]) | ord(data[1]) << 8

