---
title: Functions & Imports
description: Declaring functions, importing modules.
order: 7
section: Language Basics
slug: functions
---

# Functions

Functions are declared with the `fn` keyword.

```hhg
fn add(a: i32, b: i32) -> i32 {
    return a + b
}
```

# Imports

Modules are imported with the `import` keyword using dot-separated paths.

```hhg
import std.math
```

`from` and `as` are supported.

```hhg
from std.math import cos as cosine
```

# Comments

Single-line comments start with `//` and multi-line comments are enclosed in `/* */`.

```hhg
// This is a single-line comment
/*
This is a very long comment
that spans multiple lines.
*/
```

# `main`

Execution starts in `fn main()`. There is no global execution
(in examples, `main` is sometimes omitted for brevity).

```hhg
fn main() {
    println("Hello, World!")
}
```

# Visibility

By default, every declaration is private.
Mark something with `pub` to make it public.
