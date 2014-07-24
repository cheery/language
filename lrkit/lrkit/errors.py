class SnError(Exception):
    def __init__(self, message, start, stop):
        self.message = message
        self.start = start
        self.stop  = stop

    def __str__(self):
        return self.message

class TokenError(SnError):
    pass

class ParseError(SnError):
    pass
