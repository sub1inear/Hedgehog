# The Hedgehog Programming Language

## Introduction
Hedgehog is a general-purpose systems programming language.
It is designed on three core principles, listed in order of importance (I > II > III).

### I: As Powerful as C++ with the Simplicity of Python and the Safety of Rust
Make programming in Hedgehog and the resulting program as fast as possible, while maintaining safety.

### II: Don't Reinvent the Wheel
No drastic changes from C++ or Python; Hedgehog should look familiar and be easy to learn from either.

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

Similarly to Python, an `f` prefix before a string denotes a formatted string (known as f-strings). Expressions in `{}` are evaluated and printed.

```python
print(f"There are {4 * 5} ducks.")
```

F-strings can be modified, like in Python (look [here](https://docs.python.org/3/library/string.html#format-specification-mini-language) for the complete list).

```python
print(f"Two digits of pi: {3.14159:.2f}.")
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

Variable types are automatically inferred by default. However, if you want to explicitly set a type, it is appended before the variable name.

| Name | Size | Range |
|------|------|-------|
| i8 | signed 8-bit number | -128 to 127 |
| u8 | unsigned 8-bit number | 0 to 255 |
| i16 | signed 16-bit number | -32,768 to 32,767 |
| u16 | unsigned 16-bit number | 0 to 65535 |
| i32 | signed 32-bit number | -2,147,483,648 to 2,147,483,647 |
| u32 | unsigned 32-bit number | 0 to 4,294,967,295 |
| i64 | signed 64-bit number | -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 |
| u64 | unsigned 64-bit number | 0 to 18,446,744,073,709,551,615 |
| int | signed arbitrary-precision number | memory of machine |
| uint | unsigned arbitrary-precision number | memory of machine |
| f32 | 32-bit floating point number | -3.40 × 10<sup>38</sup> to 3.40 × 10<sup>38</sup> |
| f64 | 64-bit floating point number | -1.80 × 10<sup>308</sup> to 1.80 × 10<sup>308</sup>|
| float | arbitrary-precision floating point number | memory of machine |
| bool | 1 byte | 0 to 1 (`true` or `false`) |
| char | unsigned 8-bit number | 0 to 255 |
| usize | unsigned number able to store the maximum memory of machine | platform-specific |
| isize | signed number able to store the maximum memory of machine | platform-specific |
| time_t | number able to store a timestamp | platform-specific |
```
u64 i = 0
float j = 1
uint k = 4674
```

Numbers are inferred to be the smallest type possible, with deduction starting at `i32` for integers and `f32` for floating-point numbers. Arbitrary-precision types must always be explicitly set.

In debug mode, limited-precision integers will error if they exceed their ranges.
In release mode, limited-precision integers will wrap around in their ranges if they exceed them.

Floating-point numbers will always clamp at ∞ and -∞.

`const` can be used to make a variable immutable.
`volatile` can be used to tell the compiler that a variable can change at any time externally.
`constexpr` can be used to tell the compiler that a variable's value can be computed at compile-time.

`'`s declare a character literal, unlike Python.
```python
char x = 'h'
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

## Arithmetic

All standard arithmetic operators are supported with C++ precedence (see [here](https://en.cppreference.com/w/cpp/language/operator_precedence)).

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

`++` and `--` can only be applied once to a variable, and that variable may not appear again in the statement.
In expressions, `++`/`--`s are evaluated from left to right.
In an assignment, the right-hand side is evaluated before the left-hand side.
```c++
x = i++ // ok
y = x++ + x // error!
z = y++ + y++ // error!
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

`switch` is supported like C++, with some differences.
`break` is implicit at the end of the case.
`fallthrough` is supported to fallthrough to the next case.
Empty `case`s fallthrough automatically.
`case`s automatically are scoped.
Every enum case needs to be handled in some way.
```c++
switch x {
case 0:
case 1:
    println("low")
case 2:
    println("medium")
case 3:
    println("high")
default:
    println("unknown")
}
```
The `do-while` statement is not supported.

`and` evaluates to `true` if both sides are true. `or` evaluates to `true` if either side is true. Like Python and C++, they are short-circuiting and will not evaluate beyond what is necessary to determine the result.

```python
if x > 3 and y > 4 {
    println("Settings achieved")
}
```

`not` is used to invert a condition.

```python
if not x {
    println("x was false.")
}
```

## Functions

`def` declares a function. Arguments must be explicitly typed.

```python
def foo(i32 bar) {
    return bar * 32 + 4
}
print(foo(4))
```

Nested functions are allowed, like Python but unlike C++. These are not closures, just scope-limited functions.

`inline` **forces** the compiler to inline a function.

```
inline def swap(i32 &a, i32 &b) {
    tmp = a
    a = b
    b = tmp
}
```

## Arrays and Lists

An array is declared by adding square brackets with the size after the variable name. Square brackets are also used to initialize it, similar to Python but different than C++.

```c++
a[4] = [1, 2, 3, 4]
```

To declare a list (variable-size array), put a `*` inside the brackets.

```c++
constants[*] = [0.0, 3.14, 6.28]
```

`[]` can be used to automatically count the size of an array, similar to C++.
```python
l[] = [5, 6, 7, 8]
```

Literals default to using `[]`.
```python
l = [5, 6, 7, 8]
```

Items can be added to lists with `.append()`, similar to Python.

```python
constants.append(23.14)
```

To index an array/list, use the square brackets with the index, which starts at 0.

```python
print(a[0])
print(a[i])
```

Array/list indexes are checked for overflow automatically in debug mode.

To get the length of an array/list, use the `len` built-in function.

```python
print(len(a))
```

## Strings

Strings are represented as `char` arrays (static strings) or lists (dynamic strings).
Both types are null-terminated; lists also track their length.

## For Loops

The `for` loop is declared similarly to Python.
```python
for i in range(10) {
    print(i)
}
```

Looping over `range(stop)` or `range(start, stop)` iterates from `0` or `start` to `stop - 1`. Note that unlike Python, this is a syntactical construct and not a function.

Using `in`, the `for` loop can also iterate over a collection of items:
```python
for x in constants {
    print(x)
}
```


The `for` loop can also be declared similarly to C++. This is intended for **more complex loops**, like iterating with a step or incrementing multiple variables.
```python
for i = x; i < y; i++ {
    print(i)
}
```

## Runtime

Like Python, code execution starts in the global scope. The function `main` is not called, unlike C++, although you can make one and call it yourself.

## Scope

Variables declared in the global scope live forever. Variables declared inside `{}` are only present inside it.

```python
global_variable = 2
def func(i32 i) {
    local_variable = 4
    if i {
        inner_local_variable = 6
    }
}
```

`{}` can be used on its own to limit the scope of variables.
```python
{
    x = 5
    print(x)
}
// x is not accessible here
```

## References

References can be used to access/change something but not pass it by value. A reference is declared by an `&`.

```python
a[4] = [1, 2, 3, 4]

b = &a
```

For arrays, put the size in the square brackets (e.g. `[4]`). This is known as a sized reference.
To explicitly require lists, use `[*]`. This is known as a list reference.
To take arrays or lists of any size, leave the square brackets empty (`[]`). This is known as an unsized reference.
You must wrap the `&` and the variable in parenthesis to distinguish it from a list of references.

```
def sized(u32 (&i)[10]) {

}
def list(u32 (&i)[*]) {

}
def unsized(f64 (&p)[]) {

}
def arr_of_ref(u32 &i[10]) {

}
```

Function references are declared similarly to C++.
```c++
i32 (&func)(i32 x, i32 y) = ...
```

## Casting

Casting is done by calling the new type as a function and passing it the old type.

```c++
f32(10)
```

In Hedgehog, casting rules are significantly strengthened. In expressions, nothing will ever be automatically promoted.

```c++
i32 x = -1
u32 y = 1
if x > y {
    println("-1 > 1")
} else {
    println("-1 <= 1")
}
```
With C++ casting rules, `x` is promoted to `u32`, becoming `4294967295` because of two's complement, and so `-1 > 1`.
In Hedgehog, the above code will cause an error, forcing you to specify which variable to cast.

To correctly compare, cast `y` to `i32`:
```c++
if x > i32(y) {
    println("-1 > 1")
} else {
    println("-1 <= 1")
}
```
Which will print `-1 <= 1` as expected.

Typed variable declarations/return values also perform casting. This is preferred over manually casting and then using type inference.
```python
x = 10
u8 y = x
def func() {
    return 1
}
```

## Dictionaries/Hash Maps

To create a dictionary, use curly braces and `key:value` syntax. Manually declaring the type is done by adding `[key type : value type]` to the end.

```python
d = { "Hello" : 0, ", " : 1, "World" : 2}
def dict(u32 d[char[] : i32]) {
    ...
}
```

Accesses are the same as arrays/lists.

```python
d["!\n"] = 4
```

Nested dictionaries can occur only with values. Dictionaries are unordered.

## Declaration

Variables and arrays can be declared without being defined, causing their contents to be undefined. However, this can only occur in the case that Hedgehog can prove that the variable is not accessed uninitialized.
For example, this is acceptable:
```c++
u32 &r
x = 0
r = &x
```
but this will cause an error:
```c++
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

Unlike C++ and Python, `break` and `continue` can also be used with a number after it. This specifies how many loops to `break` out of/which loop to apply the `continue` to.

```python
for x in range(100) {
    for y in range(100) {
        if i > x * 100 + y {
            break 2
        }
    }
}
println("Exited both loops.")
```

## Advanced Statements

All statements are actually expressions! The value of the expression is its last statement. This is a very powerful feature of Hedgehog, which in its simplicity allows quite a number of convenient features.

This allows an `if` statement to act like a ternary operator (`?:`) in C++.

```python
dx = if input == "forward" { 1 } else { -1 }
```

A `for` loop can be used inside an array/list to dynamically set its contents, similar to list comprehension in Python.

```python
x[8] = [ for i in range(8) { i } ]
```

Finally, this can also be used like the comma operator in C++.

```python
value = 0
def func() {
    ...
    return { value = y; x }
}

for i = 0; i < 10; { i++; value++ } {
    println(f"i: {i}\nvalue: {value}")
}
```

## Classes

Classes are declared with the keyword `class`. All variables in a class must be typed.

The `__init__` method serves as the constructor. By default, the constructor initializes all the members in the order they are declared, like the default constructor of C++.

Unlike Python and like C++, you don't need to use `self`/`this` to access members of a class.
```python
class Player {
    i32 x
    i32 y
    i32 health
    def forward() {
        x++
        y++
    }
    def backward() {
        x--
        y--
    }
}
player = Player(1, 1, 3)
print(player)
```

Class variables default to public, like Python. To explicitly set a variable's visibility, use `public:` or `private:`, like C++.

```c++
class Encapsulated {
private:
    f32 a
    f32 b
    f32 c
public:
    bool f
}
```

There is no inheritance or polymorphism in Hedgehog. It is recommended to use composition instead.

Hedgehog supports other `__*__` methods (known as dunder methods).

| Method | Description | Default? |
|--------|-------------|----------|
| `Object __add__(Object y)` | `+` operator | No |
| `Object __sub__(Object y)` | `-` operator | No |
| `Object __mul__(Object y)` | `*` operator | No |
| `Object __div__(Object y)` | `/` operator | No |
| `Object __mod__(Object y)` | `%` operator | No |
| `Object __eq__(Object y)` | `==` operator | No |
| `bool __ne__(Object y)` | `!=` operator | No |
| `bool __lt__(Object y)` | `<` operator | No |
| `bool __le__(Object y)` | `<=` operator | No |
| `bool __gt__(Object y)` | `>` operator | No |
| `bool __ge__(Object y)` | `>=` operator | No |
| `usize __len__()` | `len` function | No |
| `u64 __hash__()` | hash function for dictionary keys | No |
| `void __del__()` | destructor | No |
| `i32 __print__(File &stream)` | `print` function/f-string within | Yes |
| `char (&)[*] __str__()` | f-strings | Yes |

Operator overloading should only be used if it logically makes sense to perform arithmetic (for example, `+` for adding two matrices).
These should be quick and efficient, as no one expects `+` to be slow.
For other applications/speeds, please define a member function.

Hedgehog will automatically generate `__print__` and `__str__` to output objects in the format `ClassName(member=value, ...)`.
It is recommended to stick to this format if you choose to implement your own `__print__` and `__str__` for consistency.
`__print__` is called by `print`/f-strings in `print`; `__str__` is called by f-strings in other context.
This allows for greater optimization, as `__print__` can directly write to the console.

## Typedefs

`typedef` creates a distinct alias for a type, similar to `using` in C++.
Resulting types are not interchangeable with the original, although they can be casted back-and-forth.
```
typedef u32 int_t
```

## Tuples

Tuples are declared by a comma separated list of items within a tuple. Unlike Python and like C++, they are mutable.

```python
t = (1, 2.0, '3')

def func() {
    return (7, 8.0, '9')
}
```

You can index a tuple similar to a normal array. Unlike Python and like C++, tuple indices must be computable at compile-time.

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

## Compiler Directives

Compiler directives are categorized into three categories.

| Sigil | Description | Explanation |
|-------|-------------| ----------- |
| `#` | Compile-Time Statement/Constant | A statement/constant that is evaluated at compile-time. | 
| `@` | Attribute | Modifies a function, variable, or type. |
| ` ` | Built-in Function | A function provided by Hedgehog that is built-in to the language. |

### Compile-Time Statements/Constants

`#if`, `#else if`, and `#else` are similar to normal `if`-statements, except they are evaluated at compile-time and only the taken branch is compiled, similar to `if constexpr` in C++.

```c++
constexpr x = 1
#if x == 1 {
    print("x == 1\n")
} #else if x == 2 {
    print("x == 2\n")
} #else {
    print("x != 1 and x != 2\n")
}
```

`#run` runs a statement at compile time. The changes made to variables are permanent.
```c++
x = 1
#run x = 2
constexpr x = #run { func() }
```

`#error(message)` prints `message` when encountered and fails to compile.
`#warning(message)` prints `message` when encountered as a warning.

`#assert(expr, message)` asserts that `expr` is true. `#assert`s will be enabled in debug mode and disabled in release mode. `#assert` can be run at compile-time or runtime (it is both `static_assert()` and `assert()` in C++). `#assert` will also optimize based on its condition being true (if supported by the backend).
```c++
x = 0
#assert(x == 0, "x != 0, help!")

if (x != 0) {
    ... // ideally this will optimize away
}
```

`#asm` allows for a programmer to directly use GCC-style inline assembly. Support is guaranteed for backend compilers supporting it.
```
i32 result
#asm {
    "mov $1, %%eax"
        : "=a" (result) 
}
```

`#version` can be used to determine the Hedgehog version, in SemVer (e.g. `1.0.0`).

`#c` marks a section of code to be exported to the C programming language, similar to `extern "C"` in C++. Advanced features not supported in C, like classes or templates, within a `#c` block will cause an error.

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

### Attributes

`@unroll(times)` tells the compiler to unroll a loop `times` many iterations.
```c++
@unroll(10)
for i in range(10) {
    print(i)
}
```

`@noreturn` marks a function as not returning.

`@used` prevents a variable/function from being optimized away.

`@pack` ensures a `class` is packed.

`@align(alignment)` ensures a variable is aligned at `alignment`.

`@fmt` allows a function to take a format string as C-style with variadic arguments for optimization.

`@scan` allows a function to take a scan string as C-style with variadic arguments (as references) for optimization.

### Built-In Functions

`sizeof(expr)` gets the size in bytes of `expr`.

```c++
print(sizeof(u32))
```

`typeof(expr)` gets the type of `expr`. Types can be compared to others.

```c++
#if typeof(x) == u32 {
    print("x is a u32.")
}
```

As discussed above, `len(obj)` gets the length of an array/list. `len` counts **elements**, not **size**.
```c++
l = [1, 2, 3, 4]
print(len(l)) // 4
print(sizeof(l)) // 16
```

`error(message)` is the runtime version of `#error`. It prints `message` and exits the program.
Hedgehog uses `error`s internally for out-of-memory, index out of bounds, and other unrecoverable errors.
`error` should only be used for unrecoverable errors; for recoverable errors, use `Result` instead, described below.

In the Hedgehog REPL, `help(expr)` prints the documentation for `expr`.

## Import

Importing another Hedgehog file is done with the `import` keyword, similar to Python. The `.hhg` is omitted. To access the contents, use the import name and the `.` operator.

```python
import module

module.func()

module.variable = 3

c = module.Class()
```

## Variadic Functions

Variadic functions are supported with `...` syntax.
They are always typed.

```c++
def sum(u32... args) {
    u32 total = 0
    for i in range(len(args)) {
        total += args.get()
    }
    return total
}

println(sum(1, 2, 3, 4))
```

## Templates

Templates have a similar syntax to C++. `class` is always used instead of `typename`.

```c++
template <class T>
def add(T a, T b) {
    return a + b
}
```

Templates can be specialized, like C++.
```c++
template <>
u32 add<i32>(i32 a, i32 b) {
    return a + b
}
```

Unlike C++, templates cannot recursively instantiate themselves. This is intended to prevent template metaprogramming that is generally less understandable and can be replaced with compile-time execution.
However, a `class` can always instantiate itself, so recursive data structures are possible.

```
template <>
def fibonacci<0>() {
    return 1
}
template <u32 N>
def fibonacci() {
    return N * fibonacci<N - 1>()
}

constexpr result = fibonacci<10>()
```
This example calculates the 10th Fibonacci number at compile-time.
However, because of Hedgehog's rules limiting template wizardry, this will not compile.

The correct way is to use Hedgehog's compile-time execution features:
```
def fibonacci(u32 n) {
    if n == 0 {
        return 1
    } else {
        return n * fibonacci(n - 1)
    }
}
constexpr result = #run { fibonacci(10) }
```
Much better.

## Unsafe

The `unsafe` keyword can apply to a function or scope, allowing you to bypass the safety features of Hedgehog.
```python
unsafe {
    u32 x[4]
    print(x[10])
}
```
This **will compile**, so be careful!

## Borrow Checker

Hedgehog uses a borrow checker to manage memory, similar to Rust.
Hedgehog aims for an 80%/20% split of memory safety; that is, Hedgehog has 80% of Rust's memory safety at 20% of the complexity.

Hedgehog's borrow checker tracks **ownership**. Only one object can own a value at a time. When the owner goes out of scope, the value is automatically deallocated.

By default, primitives are copied on assignment.
```python
x = 5
y = x
z = x // ok
```

More complex types, like arrays, lists, dictionaries, and classes, are moved on assignment.
```python
x = [1, 2, 3]
y = x
z = x // error! x has been moved to y
```

To explicitly copy a complex type, use the `copy` built-in function.
```python
x = [1, 2, 3]
y = x.copy()
z = x // ok
```

The borrow checker also tracks **references**.
References are either immutable (`&const`) or mutable (`&`).
Multiple immutable references can exist at a time, but only one mutable reference can exist at a time.
Mutable and immutable references cannot coexist.

```python
x = [1, 2, 3]
y = &const x
z = &const x
```

```python
x = [1, 2, 3]
y = &x
y = &const x // error!
```

By default, objects are allocated on the **stack**.
This means that they will be automatically deallocated when they go out of scope.
**Heap** allocations, on the other hand, can live beyond the scope of a single function.
Hedgehog manages heap allocations through four smart pointer types: `unique`, `shared`, `arc`, and `weak`.

`unique` is similar to `std::unique_ptr` in C++ and `Box` in Rust.
It uniquely owns a value on the heap, so no reference counting is needed.
```python
def func() {
    unique x = [1, 2, 3]
    return x
}
```

`shared` is similar to `std::shared_ptr` in C++ and `Rc` in Rust.
It uses reference counting to allow for multiple owners of the same value on the heap.
Use `.share()` to created a shared value to give to another variable.
```python
shared x = [1, 2, 3]
y = x.share()
a = &y
b = &y
```

`arc` is the same as `shared` but thread-safe, similar to `std::atomic_shared_ptr` in C++ and `Arc` in Rust.
```python
arc x = [1, 2, 3]
y = x.share()
a = &y
b = &y
```

`weak` is similar to `std::weak_ptr` in C++ and `Weak` in Rust. It is used to break reference cycles caused by `std::shared_ptr`. Use `.upgrade()` to get a `shared` reference to the value if it still exists.
```
shared x = [1, 2, 3]
weak y = &x
if z = y.upgrade() {
    print(z)
}
```

`unique` variables are automatically deallocated when they go out of scope. `shared` and `arc` are automatically deallocated when the last reference to them goes out of scope.


Unlike Rust, Hedgehog does not use lifetimes for simplicity.
Instead, it uses a simplified syntax combined with limited lifetime inference.

```python
u32 &critical(u32 (&x)[]) -> &x {
    process(x)
    handle(x)
    return find(x, 0)
}
```
This function has to be manually annotated with `-> &x` to signify that the returned reference is from `x`, so the borrow checker can verify that the return value never outlives `x`.

To annotate the possibility of returning either reference (or returning them both at the same time), use a comma-separated list of references.
```python
def longest(char (&a)[], char (&b)[]) -> &a, &b {
    if len(a) > len(b) {
        return &a
    } else {
        return &b
    }
}
```

## Optional/Result
`Optional<T>` allows for an optional result; it can be either a value or `None`.

```python
Optional<i32> test(i32 x) {
    return if x >= 0 { x } else { None }
}
x = test(5)
switch x {
case v:
    println(v)
case None:
    println("None")
}
```

`Result<T, E>` allows for a result that can be either a value or an error (an enum).
```python
enum TestError {
    ZeroError,
    NegativeError,
}
Result<i32, TestError> test(i32 x) {
    return if x > 0 { x }
           else if x == 0 { TestError.ZeroError }
           else { TestError.NegativeError }
}
x = 5
result = test(x)
switch result {
case v:
    println(v)
case TestError.ZeroError:
    error(f"{x} was zero!")
case TestError.NegativeError:
    error(f"{x} was negative!")
}
```

Hedgehog also supports the following, for both `Optional` and `Result`:
```
if x.has_value() { ... }
y = x.value() // returns value, otherwise errors
z = x.error() // returns error, otherwise errors
z = x.value_or(10) // 10 if x is None
```



The `?` operator can be used to propagate errors, similar to Rust. It can only be used in a function that returns a `Result`.
```python
Result<i32, TestError> func(i32 x) {
    i32 x = test(x)?
    return x * 2
}
```

## Linking to C/C++

To use Hedgehog with C/C++, simply `import` the `.h`/`.hpp` files and access the variables/functions/classes similarly as if it was a Hedgehog file. C header files must be surrounded with an `extern "C"`.

To use C/C++ with Hedgehog, simply `#include` the `.h`/`.hpp` files generated for each Hedgehog file and access the functions similarly as if it were C/C++. Functions in Hedgehog meant to export to C must be surrounded by an `#c` block, as described above.

For streamlined compatibility, use `#include "hstdlib.h"` in C/C++ to be able to access references, arrays, lists, tuples, and dictionaries.

## Platform Requirements

Hedgehog requires the C standard libraries.

Hedgehog's C++ backend requires a C++11 compiler.
Hedgehog's QBE backend requires an installation of QBE.


## Hedgehog v0.1.0 Features
- [ ] `print`
- [ ] `println`
- [ ] Formatted Input
- [ ] Variables
- [ ] Types (basic)
- [ ] Comments
- [ ] Arithmetic
- [ ] `if`
- [ ] `while`
- [ ] `and`, `or`, `not`
- [ ] Functions (basic)
- [ ] Arrays and Lists
- [ ] `len`
- [ ] `for` (`range`, `in`, C++-style)
- [ ] Scope
- [ ] Casting
- [ ] Semicolons
- [ ] `break`/`continue` (not numbered)
- [ ] Classes (basic)


## Appendix A: Hedgehog Standard Library Reference

The Hedgehog standard library is split into several modules.

| Module | Description | Automatically Imported? | Required |
|--------|-------------|-------------------------|----------|
| `core` | Core functionality needed for Hedgehog to run | Yes | Yes |
| `std` | Built-in high-level Hedgehog features (heap allocation, lists, etc.) and functions (`print`, etc.) | Yes | No |
| `fs` | Filesystem functions | No | No |
| `math` | Common math functions | No | No |
| `random` | Random number generation | No | No |
| `time` | Time, date, and sleeping | No | No |
| `collections` | Common data structures | No | No |
| `sys` | System calls | No | No |

## Appendix B: Python Quick Start Guide

## Appendix C: C++ Quick Start Guide

## Appendix D: CLI Quick Start Guide

## Appendix E: Inspiration

The following languages influenced the design of Hedgehog:
 * Python
 * C++
 * Rust
 * Go
 * Odin
 * C#
 * And many more!

To all of the inventors and visionaries, thank you for your incredible languages!
