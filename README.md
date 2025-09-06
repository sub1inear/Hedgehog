# The Hedgehog Programming Language

## Introduction
The Hedgehog programming language is based on three core principles, listed in order of importance below.

### I: As Powerful as C++ with the Simplicity of Python
Make programming in Hedgehog and the resulting program as fast as possible.

### II: Don't Reinvent the Wheel
No drastic syntax changes from C++ or Python; Hedgehog should look familiar and be easy to learn from either.

### III: There Should Be One, and Only One, Way to Do It
Duplicates or slight reinventions should be avoided.

## Quick Starts

If you already know Python and want a quick introduction, read the [Python Quick Start Guide](#appendix-b-python-quick-start-guide). If you already know C++ and want a quick introduction, read the [C++ Quick Start Guide](#appendix-c-c-quick-start-guide). These only cover the differences between the languages.

## Hello, World

The `print` function takes a string and sends it to the console.
`"`s denote a string (null terminated).
```python
print("Hello, World")
```

## Formatted Input

A single expression can be passed directly to `print`. More complex types are formatted automatically.

```python
print(3 + 4)
```

Similarly to Python, an `f` prefix before a string denotes a formatted string. Expressions in `{}` are evaluated and printed.

```python
print(f"There are {4 * 5} ducks.")
```

All standard control characters are supported.

```python
print("\n\t\a")
```

## Println

For convenience, `println` appends a newline at the end of the string.

```c++
println("There should be a newline after this ->")
```

## Variables

Variables are declared as `name = value`.

```python
x = 4
y = 3.14159
z = "This is a string"
```

Hedgehog is strongly typed, and so an attempt to assign a value to a variable of a different type results in an error. The below code will cause an error:

```python
x = 3
x = "Hello, World"
```

## Types

Variables types are automatically inferred by default. However, if you want to explicitly set a type, it is appended before the variable name.

| Name | Size | Range |
|------|------|-------|
| i8 | signed 8-bit number | -128 to 127 |
| u8 | unsigned 8-bit number | 0 to 255 |
| i16 | signed 16-bit number | 32,768 to 32,767 |
| u16 | unsigned 16-bit number | 0 to 65535 |
| i32 | signed 32-bit number | -2,147,483,648 to 2,147,483,647 |
| u32 | unsigned 32-bit number | 0 to 4,294,967,295 |
| i64 | signed 64-bit number | -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 |
| u64 | unsigned 64-bit number | 0 to 18,446,744,073,709,551,615 |
| int | signed arbitrary-precision number | memory of machine |
| f32 | 32-bit floating point number | -3.40 × 10<sup>38</sup> to 3.40 × 10<sup>38</sup> |
| f64 | 64-bit floating point number | -1.80×10<sup>308</sup> to 1.80×10<sup>308</sup>|
| float | arbitrary-precision floating point number | memory of machine |
| bool | unsigned 8-bit number | 0 to 1 (`true` or `false`) |
| char | unsigned 8-bit number | 0 to 255 (using ASCII) |
| usize | unsigned number able to store the maximum memory of machine | platform-specific |
| isize | signed number able to store the maximum memory of machine | platform-specific |
| time_t | number able to store a timestamp | platform-specific |
```
u64 i = 0
float j = 1
uint k = 4674
```

Numbers are inferred to be the smallest type possible, starting at `i32` for integers and `f32` for floating-point numbers. Arbitrary-precision types must always be explicitly set.

Limited-precision integers will wrap around in their ranges if they exceed them. Floating-point numbers will clamp at ∞ and -∞.

`constexpr` can be used to tell the compiler a variable's value can be computed at compile-time. 

## Arithmatic

All standard arithmatic operators are supported with C++ precedence.

```python
x = y + z - a
y = x + 5
```

`variable = variable operator expr` can be abbreviated with `variable operator= expr`

```python
x /= 5
y *= 3
```
Increment (`++`) and decrement (`--`) are supported. Being before the variable means the value is the one after the operator is applied and being after means the value is the original one before the operator.

```c++
x++
y = ++z
```

## Comments

`//` denotes a one line comment. `/*` denotes a multi-line comment, with `*/` ending it.
```c++
// This comment is one line.

/*
The comment is very long,
and so it needs more than one line.
*/
```

## Control Flow Statements

The `if` statement and the `while` statement are very similar to C++. All standard equality operators are supported.

```python
if x == 3 {
    x = 4
    println("Let it be known that x was 3 but now is 4!")
} else {
    println("x was not 3. Foolishness!")
}
while z {
    z = x + y
    y = j + 3
    print(f"z: {z}")
}
```

`do-while` and `switch` statements are not supported.

If the body is only one line, the braces can be omitted.

```python
if x > 3
    x = 4
```

`and` is used to be `true` if both sides are true. `or` is used to be `true` if either side is true. Like Python and C++, they are short-circuiting and will not evaluate beyond what is necessary to determine the result.

```python
if x > 3 and y > 4
    print("Settings achieved")
```

`not` is used to invert a condition.

```python
if not x
    print("x was false.")
```

## Functions

Functions are declared with the `def` keyword. While the return type is always inferred, the arguments must have explicit types. Unlike C++, one line functions are allowed to not use curly braces.

```python
def foo(i32 bar)
    return bar * 32 + 4
print(foo(4))
```

Nested functions are allowed, like Python but unlike C++.

## Arrays and Lists

An array is declared by adding square brackets with the size after the variable name. Square brackets are also used to initialize it, similar to Python but different than C++.

```c++
a[4] = [1, 2, 3, 4]
```

To declare a variable-length array (list), leave the array size empty.

```c++
constants[] = [0.0, 3.14, 6.28]
```

To automatically count the size of an array, omit the square brackets.

```python
l = [5, 6, 7, 8]
```

Items can be added to to lists with `.append()`, similar to Python.

```python
constants.append(23.14)
```

To index an array, use the square brackets with the index, which starts at 0.

```python
print(a[0])
print(a[i])
```

Array indexes are checked for overflow automatically.

Unlike C++ and Python, arrays are first-class objects. Assigning an array to another will copy it. You can also pass in arrays by value and return them.
```python
b[4] = [ 5, 6, 7, 8 ]
a = b
def array(u32 b[4]) {
    return [ 1, 2, 4, 8, 16 ]
}
```

## For Loops

The `for` loop is declared similarly to C++.
```python
for i = 0; i < 10; i++
    print(i)
```

Using `in`, the `for` loop can also iterate over a collection of items, similarly to Python:
```python
for x in constants
    print(x)
```

The `for` `in` loop can also be used with `range(stop)` or `range(start, stop)` to iterate from `0` or `start` to `stop - 1`, similarly to Python:
```python
for i in range(10)
    print(i)
```

`else` can be used after a `for` loop to only run a statement if `break` is not called.

```c++
for x in items
    if x == y {
        print("Found.")
        break
    }
else
    print("Not found.")
```

## Runtime

Like Python, code execution starts in the global scope. The function `main` is not called, unlike C++, although you can make one and call it yourself.

## Scope

Variables declared in the global scope live forever. Variables declared inside a function/statement are only present inside it.

```python
global_variable = 2
def func(i32 i) {
    local_variable = 4
    if i {
        inner_local_variable = 6
    }
}
```

## References

References can be used access/change something but not pass it by value. A reference is declared by an `&`.

For an array, a reference must know the size of the array it refers to (a sized reference). For arrays or lists you don't know the size of (an unsized reference), leave the square brackets empty. This costs performance so prefer to use the size.

References to arrays must surround the name of the array and `&` in parenthesis to distinguish it from a list of references.
```python
a[4] = [ 1, 2, 3, 4 ]

b = &a

def sized(u32 (&i)[10]) {

}
def unsized(f64 (&p)[]) {

}
def arr_of_ref(u32 &i[10]) {

}
```

## Casting

Casting is done by calling the new type as a function and passing it the old type.

```c++
x = 10
y = f32(x)
b = u8(x)
```


## Dictionaries/Hash Maps

To create a dictionary, use curly braces and `key:value` syntax. Manually declaring the type is done by adding `[key type:value type]` to the end.

```python
d = { "Hello" : 0, ", " : 1, "World" : 2}
```

Accesses are the same as arrays/lists.

```python
d["!\n"] = 4
```

Nested dictionaries can occur only with value. Dictionaries are unordered.

## Declaration

Variables and arrays can be declared without being defined, causing their contents to be undefined. However, this can only occur in the case that Hedgehog can prove that the variable is not accessed uninitialized.
For example, this is acceptable:
```
u32 &r
x = 0
r = &x
```
but this will cause an error:
```
def danger() {
    u32 x
    return x
}
```

Lists and dictionaries are not allowed to be declared without being defined.

## Semicolons

For compatibility with C++, Hedgehog supports putting a semicolon at the end of a line. Semicolons can also be used to put more than one statement on a line.

## Break and Continue

The `break` statement is used to exit early from a loop. The `continue` statement jumps to the bottom.

```c++
while running {
    ...
    if x {
        break
    }
    if y {
        continue
    }
}
```

Unlike C++ and Python, `break` and `continue` can also be used with a number after it. This specifies how many loops to `break` out of/which loop to apply the `continue`.

```python
for x in range(100)
    for y in range(100)
        if i > x * 100 + y
            break 2
```

## Advanced Statements

All statements are actually expressions! The value of the expression is its last statement. This is a very powerful feature of Hedgehog, which in its simplicity allows quite a number of convenient features.

This allows an `if` statement to act like a ternary operator (`?:`) in C++.

```python
dx =
    if input == "forward"
        1
    else
        -1
```

For a `while` statement, this allows you to easily get a statement's result.

```python
x =
    while z {
        z--
        y--
    }
```

A `for`-loop can be used in a similar way as to the `while` loop. It can also be used inside an array/list to dynamically set its contents, similar to list comprehension in Python.

```python
x[8] = [ for i = 0; i < 8; i++
            i
       ]
```

Finally, this can also be used like the comma operator in C++.

```python
value = 0
def func() {
    ...
    return { value = y; x }
}

for i = 0; i < 10; { i++; value++ }
    print(f"i: {i}\nvalue: {value}")
```

## Classes

Classes are declared with the keyword `class`. All members of a class are public.  All variables in a class must be typed.

The `__init__` method serves as the constructor, while the `__del__` method serves as the deconstructor. By default, the constructor initalizes all the members in the order they are declared, like the default constructor of C++.

`self` is automatically passed to the function, similar to `this` in C++. Like Python and unlike C++, you must explicitly use it.
```python
class Player {
    i32 x
    i32 y
    i32 health
    def forward() {
        self.x++
        self.y++
    }
    def backward() {
        self.x++
        self.y++
    }
}
player = Player(1, 1, 3)
print(player)
```

## Tuples

Tuples are declared by comma separated list of items. Unlike Python and like C++, they are mutable.

```python
t = 1, 2.0, '3'

def func() {
    return 7, 8.0, '9'
}
```

You can index a tuple similar to a normal array. Unlike Python and like C++, tuple indicies must be computable at compile-time.

```python
print(t[1])
t[1]++
```

To unpack a tuple, use a comma separated list on the left-hand side.

```python
x, y, z = func()
```

## Enums

`enum`s are declared similarly to C++. To type an enum, use `:`. Enums use the same rules as integer literal type deduction for default types.

```c++
enum Color : i32 {
    Red,
    White,
    Blue,
}
```



## Compile-Time Statements

`#if`, `#else if`, and `#else` are similar to normal `if`-statements, except they are evaluated at compile-time and only the taken branch is compiled, similar to `if constexpr` in C++.

```c++
const x = 1
#if x == 1
    print("x == 1")
#else if x == 2
    print("x == 2")
#else
    print("x != 1 and x != 2")
```

`#run` runs a statement at compile time. The changes made to variables are permanent.

```c++
x = 1
#run x = 2
```

`#inline` forces a function to be inlined without question:
```
#inline
def swap(i32 &a, i32 &b) {
    tmp = a
    a = b
    b = tmp
}
```

`#assert(expr, message)` asserts that `expr` is true. `expr` may or may not be evaluated and can be evaluated at run-time or compile-time. It also optimizes based on that assumption, even if asserts are disabled.

```c++
x = 0
#assert(x == 0, "x != 0, help!")
```

`#sizeof(expr)` gets the size in bytes of `expr`.

```c++
print(#sizeof(u32))
```

`#typeof(expr)` gets the type of `expr`. Types can be compared to others.

```c++
#if #typeof(x) == u32
    print("x is a u32.") 
```

`#unroll(times)` provides a hint to the compiler to unroll a loop `times` many iterations.
```c++
#unroll(10)
for i in range(10)
    print(i)
```

`#parallel` makes a loop parallel, when possible.
```c++
#parallel
for i in range(10)
    print(i)
```

`#asm` allows for a programmer to directly use GCC-style inline assembly. Support is guarenteed for backend compilers supporting it.

`#noreturn` marks a function as not returning.

`#used` prevents a function from being optimized away.

`#pack` ensures a `class` is packed.

`#align(alignment)` ensures a variable is aligned at `alignment`.

`#error(message)` prints `message` when encountered and fails to compile.

`#warning(message)` prints `message` when encountered as a warning.

`#help(expr)` prints the documentation on the class, function, or type given as `expr`. This is intented to be used in a REPL.

`#compiler` can be used to determine the compiler.

* `"Standard"`
* ...

`#os` can be used to determine the operating system that the program is compiled for.

* `"Windows"`
* `"Linux"`
* `"Unix"`
* `"MacOS"`
* `"None"`
* ...

`#platform` can be used to determine the machine instruction set that the program is compiled for.

* `"x86"`
* `"x64"`
* `"ARM"`
* `"AVR"`
* `"RISC-V"`
* ...

`#version` can be used to determine the Hedgehog version, in SemVer.

`#c` marks a section of code to be exported to the C programming language, similar to `extern "C"` in C++. Advanced features not supported in C, like classes or templates, within a `#c` block will cause an error. 

## Import

Importing a another Hedgehog file is done with the `import` keyword, similar to Python. The `.hhg` is omitted. To access the contents, use the import name and the `.` operator.

```python
import module

module.func()

module.variable = 3

c = module.Class()
```

## Templates

Templates have a similar syntax as C++. `class` is always used instead `typename`.

```c++
template <class T>
def add(T a, T b)
    return a + b
```

Unlike C++, templates cannot recursively instantiate themselves. This is intended to prevent template metaprogramming that is generally less understandable and can be replaced with `#run`.

To establish constraints on templates, use `requires`.

```c++
template <class T>
requires <T == u32 || T == u64>
def add(T a, T b)
    return a + b;
```

## Allocations

Hedgehog does not have a traditional concept of memory allocation. Without using a `new`/`delete`, smart pointers, or a garbage collection system, all references are guarenteed to point at valid memory and memory is guarenteed not be leaked, double freed.

> ***Note***: If you are not familiar with a manual memory management based language or do not know what the preceding term means, skip to the next section. This is the most complex part of Hedgehog and is only relevent for optimization.

To enforce this, the compiler uses escape detection to determine when a reference to a variable can escape the lifetime of a variable.

If the variable escapes, the compiler uses three rules used to dermine where and how it is stored:

### I. Stack Extention:

If a reference to a variable escapes into an outer scope of a function, it is lifetime extended, meaning the memory for the variable is allocated on the stack at the same scope as the reference.

### II. Unique/Shared Heap Allocation:

If a reference to a variable escapes outside a function, it becomes a heap reference, with the variable being allocated on the heap. If only one owner can hold it at a given time, it is identical to a normal reference to a variable or an array. This is called a unique reference.

However, if more than one owner can access it at a given time, it becomes a shared reference. Every shared reference has a reference count; this tracks the number of owners. When the number of owners goes to zero, it is deleted.

### III. Heap Promotion:

Refering to data on the heap, or by extension shared data, on the heap, is viral.

If a reference points to something on the stack/a global variable, but in one instance is data from the heap, everything assigned to that reference becomes allocated on the heap.The same goes for shared references; if a reference is assigned to unique references, but in one instance is assigned to a shared reference, everything is converted into a shared reference. This applies anywhere, from variables to function arguments to classes.

Perhaps the simplest example of Hedgehog's allocation system lies in a Hedgehog implementation of the C `malloc` function:

```python
template <class T>
def malloc(usize size) {
    T data[size]
    return &data
}
```

Here, a reference to `data` escapes, forcing the Hedgehog compiler to allocate it on the heap.

Hedgehog is designed to be at minimum faster than compiled garbage-collected languages like Go and to be able to compete with C++ as optimizations for allocations improve. Fast non-allocating code in Hedgehog should equal the performance of fast non-allocating code in C++.

## Linking to C/C++

To use Hedgehog with C/C++, simply `import` the `.h` files and access the variables/functions/classes similarly as if it was a Hedgehog file. C header files must be surrounded with an `extern "C"`.

To use C/C++ with Hedgehog, simply `#include` the `.h` files generated for each Hedgehog file and access the functions similarly as if it were C/C++. Functions in Hedgehog meant to export to C must be surrounded by an `#C` block, as described above.

For streamlined compatibility, use `#include "hstdlib.h"` in C/C++ to be able to access (sized and unsized) references, arrays, lists, tuples, and dictionaries.

## Platforms and Requirements

Hedgehog requires a C++11 compiler and the C standard libraries to run on a machine.

## Appendix A: Hedgehog Standard Library Reference

### Constants
`EOF` = `-1`

Represents the end of a file and is also returned by many IO functions as an error.

`stdin`

File for reading input.

`stdout`

File for output.

`stderr`

File for errors.

### Functions
`i32 print(const char (&str)[])`

`i32 print(#attr(fmt) const char (&fmt)[])`

`template <class T> i32 print(T obj)`

Prints a string, format string, or object to the console. Returns number of characters printed on success or `EOF` on error.

`i32 println(const char (&str)[])`

`i32 println(#attr(fmt) const char (&fmt)[])`

`template <class T> i32 println(T obj)`

Prints a string, format string, or object to the console along with a newline. Returns number of characters printed on success or `EOF` on error.

`char, bool read()`

`char[size], bool read(i32 size)`

Reads a character or an array of characters from `stdin`. If an error occurs, `bool` is `true` and `char` is undefined.

`i32 scan(#attr(scan) const char (&fmt)[])`

Scans `stdin` with a scan string. Returns number of variables written to or `EOF` on error.

### Classes

`class Random`

&emsp; Class for generating random numbers.

&emsp; `def __init__()`

&emsp; Inits the random number generator with random seed.

&emsp; `def __init__(u32 seed)`

&emsp; Inits the random number generator with a specified seed.

&emsp; `u32 random()`

&emsp; Returns a random number.

&emsp; `u32 random(u32 max)`

&emsp; Returns a random number between `0` and `max - 1`.

&emsp; `i32 random(i32 min, i32 max)`

&emsp; Returns a random number between `min` and `max - 1`.

`class Time`

&emsp; Class for getting and formatting time.

&emsp; `def __init__()`

&emsp; Inits the time class with the current time.

&emsp; `def __init__(time_t time)`

&emsp; Inits the time class with a time stamp.

&emsp; `i32 second()`

&emsp; Returns the seconds.

&emsp; `i32 minute()`

&emsp; Returns the minutes.

&emsp; `i32 hour()`

&emsp; Returns the hours.

&emsp; `i32 month_day()`

&emsp; Returns the day of the month.

&emsp; `i32 month()`

&emsp; Returns the month.

&emsp; `i32 year()`

&emsp; Returns the month.

&emsp; `i32 week_day()`

&emsp; Returns the day of the week.

&emsp; `i32 year_day()`

&emsp; Returns the day of the year.

&emsp; `bool is_daylight_savings()`

&emsp; Returns if it is daylight savings.

&emsp; `static time_t time()`

&emsp; Returns the current time stamp.

`class File`

&emsp; `enum SeekType { Set, Current, End }`

&emsp; `def __init__(const char (&filename)[], const char (&mode)[])`

&emsp; Opens a file with the specified `filename` in the specified `mode`.

&emsp; `def __del__()`

&emsp; Closes the file.

&emsp; `bool seek(i32 offset, SeekType seek_type)`

&emsp; Seeks to `offset` in the file. If `seek_type == SeekType.Set`, seeks from the beginning of the file. If `seek_type == SeekType.Current`, seeks from the current position of the file. If `seek_type == SeekType.End`, seeks from the end of the file.

&emsp; `i32 tell()`

&emsp; Returns the current file position or `EOF` on error.

&emsp; `char, bool read()`

&emsp; `char[count], usize read(i32 count)`

&emsp; Reads a character or an array of characters from a file. If an error occurs, `bool` is `true` and `char` or `char[count]` is undefined.

&emsp; `template <typename T> T[count], bool read(i32 count)`

&emsp; Reads an array of objects from a file. If an error occurs, `bool` is `true` and `T[count]` is undefined.

&emsp; `i32 scan(#attr(scan) const char (&fmt)[])`

&emsp; Scans file with a scan string. Returns number of variables written to or `EOF` on error.

&emsp; `usize write(const T (&in)[]))`

&emsp; Writes to the file with `in`.

&emsp; `i32 print(const char (&str)[])`

&emsp; `i32 print(#attr(fmt) const char (&fmt)[])`

&emsp; `template <typename T> i32 print(T obj)`

&emsp; Prints a string, format string, or object to the file. Returns number of characters printed on success or `EOF` on error.

&emsp; `i32 println(const char (&str)[])`

&emsp; `i32 println(#attr(fmt) const char (&fmt)[])`

&emsp; `template <typename T> i32 println(T obj)`

&emsp; Prints a string, format string, or object to the file along with a newline. Returns number of characters printed on success or `EOF` on error.

&emsp; `i32 unget(char c)`

&emsp; Pushes a character back into the input stream. The maximum size of the buffer is guaranteed to be at least 1. Returns `1` on success or `EOF` on error.

&emsp; `bool flush()`

&emsp; Flushes the file's buffer. Returns `false` on success and `true` on error.

&emsp; `bool error()`

&emsp; Returns whether the error flag has been set on the file.

&emsp; ```bool eof()```

&emsp; Returns whether the `EOF` flag has been set on the file.

&emsp; `def clear_flags()`

&emsp; Clears the error and `EOF` flags on the file.

&emsp; `static bool rename(const char (&old_filename)[], const char (&new_filename)[])`

&emsp; Renames the file `old_filename` to `new_filename`. Returns `false` on success and `true` on error.

&emsp; `static bool remove(const char (&filename)[])`

&emsp; Removes the file `filename`.

### Builtin Types

## Appendix B: Python Quick Start Guide

### Printing

`print` prints a string with no newline.
`println` prints a string with a newline (similar to `print` in Python).

### Types
In Hedgehog, the types of variables cannot change. Integers are sized, and can wrap around if they exceed their ranges. To use multiple precision types (the default Python behavior), use `int` or `float` before the variable name.

Normally, use the type inference and don't worry about types. However, if it becomes necessary, consider these rules:

1. Use signed over unsigned, except when the range is needed or you are doing bit manipulation.
2. Use i32/u32 and i64/u64 (or your platform's register size), except when storing large amounts of data.
3. For indexes, use `usize` or `isize`.

### Lists and Arrays

List are declared by adding `[]` after the variable name.

Arrays are lists with a specific size. They are faster than lists because of this. To use an array, add square brackets with a max size or omit the square brackets altogether to automatically count array elements to determine its size.

Lists and arrays can only have one type. For different types in a collection, use a class or a tuple.

### Curly Braces

Hedgehog uses curly braces instead of indentation.

```python
if x:
    ...
```
is the same as
```cpp
if x {
    ...
}
```

If the body is only one line, the curly braces can be omitted.

### Functions

Functions are identical with one difference: arguments must be preceded by their types.

Python:
```python
def foo(bar):
    ...
```

Hedgehog:
```python
def foo(u32 bar) {
    ...
}
```

### Copying and References

Unlike Python, in Hedgehog everything is deep-copied (similar to `copy.deepcopy()` in Python).

References (automatically created in Python when passing or copying more complex objects than lists), must be explicitly declared. To create a reference, use `&`.

Python:
```python
a = [1, 2, 3]
b = a
```

Hedgehog:
```cpp
a = [1, 2, 3]
b = &a
```

For an array, a reference must know the size of the array it refers to (a sized reference). For arrays or lists you don't know the size of (an unsized reference), leave the square brackets empty. This costs performance so prefer to use the size.

References to arrays must surround the name of the array and `&` in parenthesis to distinguish it from a list of references.
```python
def sized(u32 (&i)[10]) {

}
def unsized(f64 (&p)[]) {

}
def arr_of_ref(u32 &i[10]) {

}
```

### Classes

Members of a class must be defined with their types inside a class. They are automatically inititalized in order, so you don't need to write an `__init__` method unless more functionality is required. `self` is used to access the members, although it does not need to be declared as an argument.

Python:
```python
class Player:
    def __init__(self, x, y, health):
        self.x = x
        self.y = y
        self.health = health
```

Hedgehog:
```cpp
class Player {
    i32 x
    i32 y
    i32 health
    def forward() {
        self.x++
        self.y++
    }
    def backward() {
        self.x++
        self.y++
    }
}
```

## Appendix C: C++ Quick Start Guide


## Appendix D: Inspiration

The following languages influenced the design of Hedgehog:
 * Python
 * C++
 * Rust
 * Go
 * Odin
 * C#

To all of the inventors and visionaries, thank you for your incredible languages!