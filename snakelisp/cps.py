class Call(object):
    type = 'call'
    def __init__(self, arguments):
        self.arguments = arguments

    def __getitem__(self, index):
        return self.arguments[index]

    def __len__(self):
        return len(self.arguments)

    def coalesce(self):
        self.arguments = [arg.coalesce() for arg in self.arguments]
        return self

class Lambda(object):
    type = 'lambda'
    def __init__(self, arguments, body):
        self.arguments = arguments
        self.body      = body
        self.motion    = []
        self.notdefn   = set()

    def __getitem__(self, index):
        return self.arguments[index]

    def __len__(self):
        return len(self.arguments)

    def coalesce(self):
        while self.body.type == 'assign':
            a = self.body.coalesce()
            mot = (a.variable, a.value)
            self.motion.append(mot)
            if not self.body.defn:
                self.notdefn.add(mot)
            self.body = a.body
        self.body = self.body.coalesce()
        if self.body.type == 'call' and self.body.arguments[1:] == self.arguments:
            return self.body.arguments[0]
        return self

class Assign(object):
    type = 'assign'
    def __init__(self, variable, value, body, defn=False):
        self.variable = variable
        self.value    = value
        self.body     = body
        self.defn     = defn

    def coalesce(self):
        self.variable = self.variable.coalesce()
        self.body  = self.body.coalesce()
        self.value = self.value.coalesce()
        return self

class Variable(object):
    type = 'variable'
    def __init__(self, name=None, link=None):
        self.name = name
        self.link = link
        self.glob = False

    def __repr__(self):
        return "Variable({})".format(self.name)

    def coalesce(self):
        if self.link is not None:
            return self.link
        return self

class Constant(object):
    type = 'constant'
    def __init__(self, value):
        self.value = value

    def coalesce(self):
        return self

null  = Constant(None)
true  = Constant(True)
false = Constant(False)

class Environ(object):
    def __init__(self):
        self.arguments = []
        self.motion   = []
        self.inside   = {}
        self.outside  = {}
        self.unbound  = set()
        self.implicit = {}
        self.children = []
        self.closed   = False

    def new_argument(self, name, bind=True):
        if bind:
            var = self.get_local(name, False)
        else:
            var = Variable(name)
        self.arguments.append(var)
        return var

    def new_implicit(self, name):
        if name in self.implicit:
            return self.implicit[name]
        else:
            var = self.implicit[name] = Variable(name)
            return var

    def get_local(self, name, initnull=True):
        if name in self.inside:
            return self.inside[name]
        var = self.inside[name] = Variable(name)
        if initnull:
            self.motion.append((var, null))
        return var

    def lookup(self, name):
        if name in self.inside:
            return self.inside[name]
        elif name in self.outside:
            return self.outside[name]
        var = self.outside[name] = Variable(name)
        self.unbound.add(var)
        return var

    def new_environ(self):
        env = Environ()
        env.implicit = self.implicit
        self.children.append(env)
        return env

    def close(self, body):
        for child in self.children:
            assert child.closed
            for variable in child.unbound:
                if variable.name in self.inside:
                    variable.link = self.inside[variable.name]
                else:
                    self.unbound.add(variable)
        self.closed = True
        lamb = Lambda(self.arguments, body)
        lamb.motion.extend(self.motion)
        return lamb

    def seal(self):
        for var in self.unbound:
            var.glob = True
            yield var
        for var in self.implicit.values():
            var.glob = True
            yield var
