class CharacterLookAhead(object):
    def __init__(self, generator):
        self.generator = iter(generator)
        try:
            self.value = self.generator.next()
            self.empty = False
            self.location = self.advance_location(self.value, 0x100)
        except StopIteration:
            self.value = None
            self.empty = True
            self.location = 0x100
        self.previous_value    = None
        self.previous_location = 0x100

    def step(self):
        self.previous_value    = self.value
        self.previous_location = self.location
        if self.empty:
            raise Exception("CharacterLookAhead overstep")
        try:
            self.value = self.generator.next()
            self.empty = False
        except StopIteration:
            self.value = None
            self.empty = True
        self.location = self.advance_location(self.value, self.location)
        return self.previous_value

    def advance_location(self, value, loc):
        if value == '\n':
            return (loc >> 8) + 1 << 8
        else:
            return loc & ~0xff | (loc & 0xff) + 1

class LookAhead(object):
    def __init__(self, generator):
        self.generator = iter(generator)
        try:
            self.value = self.generator.next()
            self.empty = False
            self.location = self.value.location
        except StopIteration:
            self.value = None
            self.empty = True
            self.location = 0
        self.previous_value    = None

    def step(self):
        self.previous_value    = self.value
        if self.empty:
            raise Exception("LookAhead overstep")
        try:
            self.value = self.generator.next()
            self.empty = False
            self.location = self.value.location
        except StopIteration:
            self.value = None
            self.empty = True
            self.location = 0
        return self.previous_value
