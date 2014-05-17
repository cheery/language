Yesterday (2014-05-17) I studied javascript, Self objects and lua metatables to figure out design for an object system.
You might like to see what I came up with, so I described it here.

## Here's the requirements I had

 * it must be simple, approachable
 * it must not hinder optimization attempts
 * some objects must be able to gather new attributes, but this is not useful or necessary for all of them
 * should catch some good behaviors from all: python, javascript, self, lua

## Analysis

Lua metatables are the simplest system, but to call the method you need to use the weird syntax. eg. :append(). It is a bit inconvenient if not understandable. Self -language has very different object system with very similar implementation technique, calling them 'maps' there. I think this can be combined, to produce python-style object system that is easy to control.

Javascript binds the callattr object to `this` -variable. When writing programs with javascript, this is confusing at first, but turns out to be nice behavior most of the time.

In the language every object will have a behavior table associated with it. The table doesn't dictate what the object data layout is, but it tells how the object reacts to manipulation. Call to behavior table can be used to instantiate the object.

The behavior table contains string entries that are used to resolve getattr, and other entries that are there to describe behavior of the object.

    +construct    method to build an entirely new object and return it.
    +init         sets the initial state of the object
    +call         What happens when the object is called.

When the programmer does a callattr/getattr/setattr, the behavior table is used to resolve what happens:

    lookup for the member name from the behavior table.
        If found, use the +call, +get, +set -handler in the entry.
    If not found, call +callattr, +getattr, +setattr

Behavior of different objects in the function table.

    generic
        callattr  call +call -handler.
        getattr   call +get  -handler.
        setattr   call +set  -handler.
    function
        callattr(object, name, ...) call the function {object bound to @ -variable}
        getattr(object, name)       get the function, perhaps auto bind the function to the object
        setattr(object, name, ...)  system behaviour
    slot
        callattr  call the contents {object as self -member}
        getattr   get the contents
        setattr   set the contents
    property
        callattr  call what the getattr retrieves.
        getattr   call user-defined get function
        setattr   call user-defined set function

Here is the partial behavior of the list object:

    append()
    index()
    insert()
    pop()
    remove()
    length ->
    +iter
    +concat
    +getitem
    +setitem

Another behavior mapping for fixnums:

    toString()
    +add
    +sub
    +mul
    +div
    +mod
    +cmp

