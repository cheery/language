from .rule import Rule, Accept
from .errors import ParseError

class Parser:
    def __init__(self, results, visitor, *args):
        self.table = results.table
        self.state = 0
        self.stack = []
        self.data  = []
        self.visitor = visitor
        self.args    = args

    @property
    def idle(self):
        return self.state == 0 and len(self.stack) == 0

    def reset(self):
        self.state = 0
        self.stack = []
        self.data  = []

    def step(self, start, stop, group, value):
        action = self.table[self.state].get(group)
        while isinstance(action, Rule):
            c_value = []
            c_range = None
            for n in range(len(action)):
                self.state = self.stack.pop(-1)
                i_range, i_value = self.data.pop(-1)
                c_value.append(i_value)
                c_range = merge(i_range, c_range)
            c_value.reverse()
            c_value = self.visitor(action, c_range, c_value, *self.args)
            self.data.append((c_range, c_value))
            self.stack.append(self.state)
            self.state = self.table[self.state].get(action.lhs)
            action = self.table[self.state].get(group)
        if isinstance(action, Accept):
            self.state = self.stack.pop(-1)
            i_range, i_value = self.data.pop(-1)
            assert len(self.data) == 0
            assert len(self.stack) == 0
            return i_value
        if action is None:
            expects = ', '.join(map(str, self.table[self.state]))
            raise ParseError("got {} but expected {}".format(group, expects), start, stop)
        self.data.append(((start, stop), value))
        self.stack.append(self.state)
        self.state = action

def merge(left, right):
    if left is None:
        return right
    elif right is None:
        return left
    else:
        return (left[0], right[1])
