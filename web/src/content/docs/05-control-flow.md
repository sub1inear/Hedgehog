---
title: Control Flow
description: if, while, for, match, break with labels.
order: 5
section: Language Basics
slug: control-flow
---

# Control Flow

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

## `match`

`match` statements are exhaustive:

```hhg
match x {
    0 => print("zero"),
    1 => print("one"),
    _ => print("other"),
}
```

## Labeled break

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
