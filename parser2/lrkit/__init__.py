from .rule import Rule
from .snerror import SnError
from .tokenizer import tokenize

def rule(lhs, *prod):
    return Rule(lhs, prod)
