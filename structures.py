class Constant(object):
    def __init__(self, location, group, value):
        self.location = location
        self.group = group
        self.value = value

    def __repr__(self):
        return "<Constant %s %r>" % (self.group, self.value)

#    def find_all(self, groups, result=None):
#        result = [] if result is None else result
#        if self.group in groups:
#            result.append(self)
#        return result
#
class Struct(object):
    def __init__(self, location, group, *data):
        self.location = location
        self.group = group
        self.data = list(data)

    def append(self, value):
        self.data.append(value)

#    def find_all(self, groups, result=None):
#        result = [] if result is None else result
#        if self.group in groups:
#            result.append(self)
#        for struct in self:
#            struct.find_all(groups, result)
#        return result
#
    def __getitem__(self, index):
        return self.data[index]
    
    def __setitem__(self, index, value):
        self.data[index] = value

    def __len__(self):
        return len(self.data)

    def __repr__(self):
        return "<Struct %s %r>" % (self.group, self.data)
