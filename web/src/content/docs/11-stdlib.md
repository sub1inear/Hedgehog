---
title: Stdlib, Interop, CLI
description: What's in core/std, how to call C/C++, and the hhg CLI.
order: 11
section: Ecosystem
slug: stdlib
---

# Stdlib

| Module | Overview | Contents |
|--------|----------|----------|
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

# C/C++ Interop

Hedgehog can interoperate with C and C++ code by `import`ing C/C++ headers directly.
C/C++ can call Hedgehog functions by including the generated C/C++ header file.

# CLI

| Command | Description |
|---------|-------------|
| `hhg init` | Initializes a new Hedgehog project |
| `hhg build` | Builds the project |
| `hhg run` | Builds and runs the project |

# Roadmap: Hedgehog v0.1.0

- Variables (`let`, `let mut`, `const`)
- Arithmetic Operators (`+`, `-`, `*`, `/`, `%`, etc.)
- Boolean Logic (`and`, `or`, `not`)
- Functions and Return Values
- Types
- Integer, Boolean, and Array Literals
- Control Flow (`if`, `else`, `while`, `for`)
- Arrays
- References
- Comments
