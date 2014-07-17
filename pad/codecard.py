class Card:
    def __init__(self, lines=None):
        self.lines   = lines or [Line()]

    def yank(self, start, stop):
        if start.line == stop.line:
            text = self.lines[start.line].text[start.index:stop.index]
            return TextBuffer(self, text)
        _, tail1 = self.lines[start.line].cut(start.index)
        middle   = self.lines[start.line+1:stop.line]
        head2, _ = self.lines[last.line].cut(stop.index)
        return TextLineBuffer(self, tail1, middle, head2)

    def yank_lines(self, start, stop):
        lines = self.lines[start.line:stop.line+1]
        return TextLineBuffer(self, Line(), lines, Line())

    def kill(self, start, stop):
        if start.line == stop.line:
            text = self.lines[start.line].text[start.index:stop.index]
            self.lines[start.line].text[start.index:stop.index] = []
            return TextBuffer(None, text)
        head1, tail1 = self.lines[start.line].cut(start.index)
        middle       = self.lines[start.line+1:stop.line]
        head2, tail2 = self.lines[last.line].cut(stop.index)
        self.lines[start.line:stop.line+1] = head1 + tail2
        return TextLineBuffer(None, tail1, middle, head2)

    def kill_lines(self, start, stop):
        lines = self.lines[start.line:stop.line+1]
        self.lines[start.line:stop.line+1] = []
        return TextLineBuffer(None, (Line(), lines, Line()))

    def put(self, position, buffer):
        return buffer.put(self, position)

    @property
    def top(self):
        return Position(0, 0)

    @property
    def bot(self):
        return Position(len(self.lines)-1, len(self.lines[-1].text))


class Line:
    def __init__(self, text=None, indent=0):
        self.text   = text or []
        self.indent = indent
    
    def __len__(self):
        return len(self.text)

    def cut(self, index):
        head = Line(self.text[:index], self.indent)
        tail = Line(self.text[index:], self.indent)
        return head, tail

    def __add__(self, other):
        return Line(self.text + other.text, self.indent)

class Position:
    def __init__(self, line, index):
        self.line  = line
        self.index = index

class TextBuffer:
    def __init__(self, link, text):
        self.link = link
        self.text = text

    def put(self, card, pos):
        assert self.link is None
        card.lines[pos.line].text[pos.index:pos.index] = self.text
        return pos, Position(pos.line, pos.index + len(self.text))

class TextLineBuffer:
    def __init__(self, link, head, lines, tail):
        self.link  = link
        self.head  = head
        self.lines = lines
        self.tail  = tail

    def put(self, card, pos):
        assert self.link is None
        head, tail = card.lines[pos.line].cut(pos.index)
        lines = [head + self.head] + self.lines + [self.tail + tail]
        start = Position(pos.line, 0)
        stop  = Position(pos.line + len(lines) - 1, len(lines[-1].text))
        card.lines[pos.line:pos.line+1] = lines
        return start, stop
