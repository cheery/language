from .rule import Rule, Accept
from .errors import SnError, TokenError, ParseError
from .tokenizer import tokenize
from .parser import Parser

def rule(lhs, *prod):
    return Rule(lhs, prod)

__version__ = '0.0.0'
