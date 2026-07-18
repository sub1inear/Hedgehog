---
title: Variables & Constants
description: let, let mut, and const.
order: 2
section: Language Basics
slug: variables
---

# Variables

Variables are declared with `let` or `let mut` and can optionally have a type annotation.

```hhg
let x = 5
let y: str = "Hello, World!"
let mut z = 42
print(x)
print(y)
```

## `const`

`const` variables are immutable and must be initialized at compile time.

```hhg
const pi = 3.14159
```

## Casting

Casting is done with `type(expr)`:

```hhg
let x = 5
let y = f64(x) // 5.0
```

Hedgehog bans implicit casting except for widening conversions (e.g. `i32` to `i64`).

## Move / copy semantics

Basic types copy on assignment; complex types (`str`, `list`, `dict`, arrays, classes, etc.) move by default.
Call `.copy()` to duplicate.

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
