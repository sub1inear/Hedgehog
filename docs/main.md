# The Hedgehog Programming Language

## Introduction
Hedgehog is a toy systems programming language.
It has three core principles, listed in order of importance (I > II > III).

### I: As Powerful as C++ with the Simplicity of Python and the Safety of Rust
Make programming in Hedgehog and the resulting program as fast as possible, while maintaining safety.
When these conflict, speed > simplicity > safety.

### II: Don't Reinvent the Wheel (Aspirational)
No drastic changes from C++ or Python; Hedgehog should look familiar and be easy to learn from either.

### III: There Should Be One, and Only One, Obvious Way to Do It
Duplicates or slight reinventions that cause perplexity when choosing between them should be avoided.

## Printing
`print` writes without a trailing newline; `println` writes with one.
`print` and `println` can convert any type to a string (and will format it nicely).
```hhg
print("Hello, World!")
println("Hello, World!")
```

`f`-strings use `{}` to embed expressions.
```hhg
print(f"There are {4 * 5} ducks.")
```

## Variables
Variables are declared with `let` or `let mut` and can optionaly have a type annotation.
```hhg
let x = 5
let y: str = "Hello, World!"
let mut z = 42
print(x)
print(y)
```

`const` variables are immutable and must be initialized at compile time.
```hhg
const pi = 3.14159
```

## Types
Hedgehog has many built-in types, including:

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
| f32 | 32-bit floating point number | -3.40 × 10^38^ to 3.40 × 10^38^ |
| f64 | 64-bit floating point number | -1.80 × 10^308^ to 1.80 × 10^308^|
| bool | 1 byte | 0 to 1 (`true` or `false`) |
| char | 4 bytes | Unicode scalar value |
| usize | unsigned number able to store the maximum memory of machine | platform-specific |
| isize | signed number able to store the maximum memory of machine | platform-specific |
| void | no type | N/A |

Literals can be suffixed with type annotations, like Rust.

## Arithmetic Operators
Standard arithmetic operators and precedence.
```hhg
let x = 5 + 3 * 2
let y = (x - 4) / 2
let bits = 0b1010
bits |= 0b0101
```

## Boolean Logic
`and`, `or` and `not`.
```hhg
let is_valid = (x > 0) and (y < 10) or (z >= 5)
let is_invalid = not is_valid
```

## Control Flow
Blocks use braces and conditions do not require parentheses.

```hhg
if x > 0 {
    print("x is positive")
} else if x < 0 {
    print("x is negative")
} else {
    print("x is zero")
}
while x > 0 {
    print(x)
    x -= 1
}
for i in 0..10 { // 0..=10 for inclusive
    print(i)
}
```
The value of a block is the value of its last expression.

`match` statements are exhaustive:
```hhg
match x {
    0 => print("zero"),
    1 => print("one"),
    _ => print("other"),
}
```

`break` and `continue` are supported, optionally with labels.
```hhg
outer:
for i in 0..10 {
    for j in 0..10 {
        if i * j > 50 {
            break outer
        }
        print(f"{i}, {j}")
    }
}
```

## Arrays
Arrays are fixed-size and declared with square brackets.
Type annotation: `T[N]`.
Type inference defaults to arrays.
```hhg
let arr: i32[5] = [1, 2, 3, 4, 5]
```

## Lists
Lists are dynamic and can grow or shrink in size.
Type annotation: `list<T>`.
```hhg
let mut l: list<i32> = [1, 2, 3]
l.append(4)
print(l.len()) // 4
```

## Strings
Static strings/string literals are declared with double quotes.
Type annotation: `&static str`.
```
let s: &static str = "Hello, World!"
```

Dynamic strings are UTF-8 and can grow or shrink in size.
Type annotation: `str`.
```hhg
let mut s: str = "Hello, World!"
s.replace("World", "Hedgehog")
```

## Slices
Slices are views into arrays or lists and are declared with `T[]`.
Slices are the universal sequence type; they can be used in place of arrays or lists in function parameters.
```hhg
let arr: i32[5] = [1, 2, 3, 4, 5]
let slice = arr[1..4] // [2, 3, 4]
fn arr_or_list(s: i32[]) {
    print(s)
}
```

The universal slice type for strings is `&str`, which is UTF-8.

## Dicts
Dicts are key-value pairs and are declared with curly braces.
Type annotation: `dict<K, V>`
```hhg
let d: dict<str, i32> = {"one": 1, "two": 2}
```

## Functions
Functions are declared with the `fn` keyword.
```hhg
fn add(a: i32, b: i32) -> i32 {
    return a + b
}
```

## Import
Modules are imported with the `import` keyword using dot-separated paths.
```hhg
import std.math
```
`from` and `as` are supported.
```hhg
from std.math import cos as cosine
````

## Comments
Single-line comments start with `//` and multi-line comments are enclosed in `/* */`.
```hhg
// This is a single-line comment
/*
This is a very long comment
that spans multiple lines.
*/
```

## References
References are declared with `&` and can be mutable with `&mut`.

```hhg
let x = 14
let y = 5

let rx = &x
let ry = &mut y
```

`&static` references are references to static data that will live for the entire program.

## Casting
Casting is done with `type(expr)`.
```hhg
let x = 5
let y = f64(x) // 5.0
```

Hedgehog bans implicit casting except for widening conversions (e.g. `i32` to `i64`).

## Borrow Checker
Hedgehog has a simplified borrow checker.
There are no explicit lifetimes.
Hedgehog will infer function lifetimes, but `class` lifetimes require `unsafe`.

Types:
- `unique<T>` (`Box<T>` in Rust)
- `shared<T>` (`Rc<T>` in Rust)
- `weak<T>` (`Weak<T>` in Rust), `.upgrade()` to `shared<T>` if possible

Basic types copy on assignment; complex types (`str`, `list`, `dict`, arrays, classes, etc.) move by default (`.copy()` to copy/share).
```hhg
let x = 5
let y = x
let z = y
print(x) // 5

let s1: str = "hello"
let s2 = s1.copy()
let s3 = s1 // move
print(s1) // error: s1 has been moved
print(s2) // "hello"
print(s3) // "hello"
```

The XOR mutability rule applies: you can have either one mutable reference or any number of immutable references to a value at a time.

## Classes
Classes are declared with the `class` keyword.
Make instances with `ClassName(...)`.
```hhg
import std.math
class Point {
    x: f64
    y: f64
    fn dist(&self, p: &Point) -> f64 {
        let dx = self.x - p.x
        let dy = self.y - p.y
        return std.math.sqrt(dx * dx + dy * dy)
    }
}
let p1 = Point(3.0, 4.0)
let p2 = Point(0.0, 0.0)
print(p1.dist(&p2)) // 5.0
```
Field access is with dot notation (`p.x`).
No `::`.

`static` methods do not take `self` and are called with `ClassName.method(...)`.

Several methods are standardized:

| Method | Meaning |
|--------|---------|
| `fn ClassName(...)` | Constructor, automatically generated but can be overridden |
| `fn ~ClassName(&mut self)` | Destructor, automatically generated but can be overridden |
| `fn copy(&self) -> ClassName` | Copy method |
| `fn hash(&self) -> usize` | Hash method |
| `fn print(&self, f: &File)` | Print method |
| `fn str(&self) -> str` | String method |
| `fn len(&self) -> usize` | Length method |
| `fn cap(&self) -> usize` | Capacity method |

## Templates
Generic functions/classes use angle-bracket syntax.
```hhg
fn identity<T>(x: T) -> T {
    x
}
```

## Interfaces
Interfaces (structurally typed) declare method signatures only.
```hhg
interface Drawable {
    fn draw(&self)
}
```

## Enums
Enums are declared with the `enum` keyword.
```hhg
enum Color {
    Red,
    Green,
    Blue
}
```

Access with dot notation (`Color.Red`).

## Type Aliases
Type aliases are declared with the `type` keyword.
```hhg
type int_t = i32
```

## Tagged Unions
Hedgehog has tagged unions (`A | B | ...`), often convenient for optionals and results.
Tagged unions are nominal (e.g. `(A | B) | C` is the same as `A | B | C`).
`!type` declares an error type.
`null` is a constant meaning nothing.

`expr?` will propagate `null`/`!type` if `expr` is not valid.
`expr!` will `panic` if `expr` is not valid.

```hhg
fn test() -> u32 | null {
    let u: u32 = 4
    if u % 2 == 0 {
        return u
    }
    return null
}

type zero_error = !void // error type, void -> no data
fn divide(a: f64, b: f64) -> f64 | zero_error {
    if b == 0.0 {
        return zero_error
    }
    return a / b
}
```

To bind from tagged unions:
```
if result: u32 = test() {
    print(result)
} else {
    print("no value")
}

match divide(10.0, 2.0) {
    result: f64 => print(result),
    zero_error => print("division by zero"),
}
```



## Main
Execution starts in `fn main()`.
No global execution (in examples, `main` is sometimes omitted for brevity).
```hhg
fn main() {
    println("Hello, World!")
}
```

## Visibility
By default, every declaration is private.
Mark something with `pub` to make it public.

### Pointers

Pointers are declared with `*` and have the same syntax as references, but they can bypass the borrow checker.
Pointers cannot be `null` by default; you must use `*T | null` for nullable pointers.

## Unsafe
Unsafe code is declared with the `unsafe` keyword. `unsafe` code can:

- Assign/use `class` references
- Dereference raw pointers
- Call unsafe functions
- Declare variables without initializing them

`unsafe` can modify a function, class, or variable.
Global variable access does not require `unsafe`, as Hedgehog does not have threads (yet).

```hhg
unsafe {
    let ptr: *const i32 = &x
    println(*ptr)

    let nullable_ptr: *i32 | null = null
    *nullable_ptr = 5 // this will segfault! be careful!

    let undefined: *i32
    println(*undefined) // UB

    cpp_func() // C++ functions are unsafe
}
```

## Stdlib

| Module | Overview | Contents |
|--------|-------------|---------|
| `core` | Built-in primitive types, functions, etc. | Types, `panic`, `assert`, etc. |
| `core.info` | Compiler and system info | `version`, `os`, `arch`, etc. |
| `core.interfaces` | Interfaces | `Add`, `Sub`, etc. |
| `core.mem` | Memory operations | `copy`, `move`, `bitcast`, `sizeof`, `alignof`, etc. |
| `core.cell` | Cell types | `Cell`, `RefCell`, `Ref`, `ConstRef` |
| `core.range` | Ranges | `Range` |
| `std` | Standard library | `print`, `input`, `list`, `str`, `unique`, `shared`, `weak`, etc. |
| `std.fs` | Filesystem | `File`, `Path`, etc. |
| `std.math` | Math functions | `sqrt`, `sin`, `cos`, etc. |
| `std.alloc` | Memory allocations | `malloc`, `free`, `realloc`, etc. |
| `std.random` | Random class and methods | `Random`, etc. |
| `std.time` | Time functions | `Time`, `sleep`, etc. |
| `std.collections` | Collections | `LinkedList`, `BinaryHeap`, etc. |
| `std.os` | OS functions | `env`, `args`, `exit`, `abort`, `system`, `stdout`, `stderr`, `stdin`, etc. |

`core` and `std` are automatically imported.
`std` may be disabled for bare-metal programming.
`core.*` are renamed `std.*` for convenience when `std` is enabled (`core.*` then may not be used).

## CLI

| Command | Description |
|---------|-------------|
| `hhg init` | Initializes a new Hedgehog project |
| `hhg build` | Builds the project |
| `hhg run` | Builds and runs the project |

## C/C++ Interop

Hedgehog can interoperate with C and C++ code by `import`ing C/C++ headers directly.
C/C++ can call Hedgehog functions by including the generated C/C++ header file.

## Hedgehog v0.1.0
- [ ] Variables (`let`, `let mut`, `const`)
- [ ] Arithmetic Operators (`+`, `-`, `*`, `/`, `%`, etc.)
- [ ] Boolean Logic (`and`, `or`, `not`)
- [ ] Functions and Return Values
- [ ] Types
- [ ] Integer and Array Literals
- [ ] Control Flow (`if`, `else`, `while`, `for`)
- [ ] Arrays
- [ ] References
- [ ] Comments