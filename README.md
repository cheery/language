# Language Parser

This language might change in middle of development. It is an alpha release.

## How to use

Package it yourself, or put it such that you can access the parser.py There are two ways to use it after you've imported it in python:

    parser.parse_file(filename)
    parser.parse(source, filename=None, debug=False)

These commands return an iterator, the iterator gives you `parser.Struct` and `parser.Constant` -objects. All objects have `location` and `group` -attributes. `Struct` can be indexed or iterated. `Constant` has `value` -attribute.

## Development history

### Current status

The error messaging hasn't been proofed. No tests done in practise whether the language is convenient to use. Code tested, though no automatic tests present.

### Testing

I wrote samples, carefully choosing tests that cover the features in the code. I checked that their output is correct and that they fail on some parts I intended them to fail.

I did not bother testing the tokenizer, because I'd have recognised this far if the tokenizer is broken. 

### Troubleshooting

I have debug -parameter in place. If the code breaks, you can enable it to see every token that is consumed. I used this method early to discover errors. It will likely still allow you to detect errors.
