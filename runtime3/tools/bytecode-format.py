import sys, json, re

spec   = {}
source = sys.stdin.read()

r_optab   = re.compile("optable\[\] = {\s+([^}]*)};", re.DOTALL)
r_opnames = re.compile("&&op_(\w+)")

optab = r_optab.search(source).group(1)
for opcode, opname in enumerate(r_opnames.findall(optab)):
    spec[opname] = op = {"opcode":opcode}
    pat = "/\* {} ([^*]+) \*/".format(opname)
    m = re.compile(pat).search(source)
    if m is not None:
        op["format"] = m.group(1).strip().split(' ')

json.dump(spec, open(sys.argv[1], 'w'))
print("-- {} Generation done".format(sys.argv[1]))
