---
title: References & Borrow Checker
description: References, mutability rules, unique/shared/weak, and pointers.
order: 9
section: Memory & Safety
slug: memory
---

# References

References are declared with `&` and can be mutable with `&mut`.

```hhg
let x = 14
let y = 5

let rx = &x
let ry = &mut y
```

`&static` references are references to static data that will live for the entire program.

# Borrow Checker

Hedgehog has a simplified borrow checker. There are no generic lifetime parameters.
Hedgehog will infer function lifetimes, but non-`static` `class` lifetimes require `unsafe`.

Types:

- `unique<T>` (`Box<T>` in Rust)
- `shared<T>` (`Rc<T>` in Rust)
- `weak<T>` (`Weak<T>` in Rust), `.upgrade()` to `shared<T>` if possible

The XOR mutability rule applies: you can have either **one mutable reference or any number of immutable references** to a value at a time.

# Pointers

Pointers are declared with `*` and have the same syntax as references, but they can bypass the borrow checker.
Pointers cannot be `null` by default; you must use `*T | null` for nullable pointers.

# Unsafe

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
