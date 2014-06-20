# Programming Language

Project Goal: Unobstructed programming experience

Everything of the remaining text is planned.

## Licensing

The implementation is open source, with GPLv3 license. We will also provide commercial licensing with fair pricing.

There will be a catalog in web which provides third party libraries with similar licensing scheme. The users will be able to automaticly calculate how much they need to pay for licensing fees.

Foundation or a company will be set up, to share income with contributors. The most recent contributions will receive larger share of income than the old contributions.

## Documentation

The documentation is preferred to be separate from the source code. There will be automated tools to check how well the reference documentation covers a module it is documenting.

You are expected to find users and reference manuals about everything that is supposed to by used by somebody else.

If the documentation is not sufficiently good and purposeful, the language is not allowed to become stable.

## Deployment

The system modules will be divided to safe and unsafe. The apps that only use the safe modules are allowed a direct execution from a HTTP URL. The remaining systems require user install.

Deployment should be straightforward, and considered by the language designers in any case.

## Support

Automatic, or semi-automatic updates for every application written in the language. Modules to support attaching issue tracker into distributed software. These things should work without programmer having to pay deep attention to them.

Remote debugging tools? Not in first stable.

## Optimization

The whole language is designed to support late, informed optimizations. The first stable version will have a JIT compiler. The design will be adjusted to allow efficient JIT, to make sure the optimizations can be postponed late as possible. There will be language supported, programmable, profiling, tracing and debugging tools. Writing a C extension module will be the last breath optimization to what you should have to resort only in extreme uses of the language.

## Troubleshooting

Noticing how often just printf() works for finding bugs from the code, tracing tools will be in the focus. Considering a programmable debugger concept. Needs further studying.

## Standard libraries

Will plagiarize webgl API at least, because that's the best thing since blue cheese.

Greenlets will allow writing code that appears synchronous, but runs asynchronously. An internal event loop using greenlets would be what I want.

Module level live reloading support in consideration. Reloadable modules should be usable for at least limited usecases.

Language supported plugins for end-users? Requires further studies.

## Syntax

Whitespace sensitive, indented syntax. It should be clean and unobstructed, meant for humans to read and modify. Disallows parenthesis that span multiple lines, insisting on representing the structure always by indentation rules. Experienced people who insist on using curly braces, are concerned about \t vs. whitespace issues and aren't using VIM with expandtabs yet, should take a creative pause at this point.

The module format will be specified, and one will expect certain stability from it. Will also allow plugging transpilers in. Especially interested about visual programming tools people might create this way.

## VM

Instruction format will be imitated from luajit, that is op:a:d and op:a:b:c, always encoded in little endian. The opcodes of the mnemonics will be in alphabetic order, so decoding will only require knowing the used virtual instruction set.

The language will be copying garbage collected. Still studying how to implement eq hash tables. Perhaps with a rehash on hash table lookup failure.

## Object system

There is a special variable `@`, which refers to "self". Every object has an interface, except null object, which is queried for a method when an attribute is called. The interface contains system methods in `+method` -syntax, for example `+add`, so they're never directly accessible. If the getattr/setattr/callattr fails to find anything from the interface table, it will call `+getattr/+setattr/+callattr`.

Major plans for the use of this object system: JIT-supported vectors, matrices, foreign function interface. FFI might have to be one of the unsafe modules.

## Module space

Every program has a so-called module space, supposed to allow attachment of remote and custom modules, without filesystem changes. Partially maps into the filesystem and the web.

The module loader will allow global names to be overrided, such that the programmer can control the module system by redefining `require`, `module` and `exports`.

## Summary

Overall, I try to tackle every problem I've experienced with using dynamic languages. The major problem is that they're supposed to make things lot easier, often succeeding. But then they fail at some point, dropping you back to the old slow ways to develop code.

Overall programming is combination of three things:

 * Learn
 * Do things
 * Share/Sell things

My language design is supposed to cover all three, not just the beef in the middle.

## Participate

To participate, you need to communicate with me, the author of this project. You can [file an issue][issues], or send me a private message in IRC (irc.freenode.net nick: Cheery), You may also throw an [email to Henri Tuhola <henri.tuhola@gmail.com>](mailto:henri.tuhola@gmail.com).

Start by telling about what you've done before, what would you like to do, what you would have to know to participate. Whether you've managed to read and study at the C source code in the runtime directory.

I will map out the status of the project in the [github issues list][issues], so you will know where we are going.

 [issues]: https://github.com/cheery/language/issues
