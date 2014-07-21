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
