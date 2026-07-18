---
title: Arrays, Lists, Strings, Slices, Dicts
description: The core sequence and mapping types.
order: 6
section: Language Basics
slug: collections
---

# Arrays

Arrays are fixed-size and declared with square brackets.
Type annotation: `T[N]`. Type inference defaults to arrays.

```hhg
let arr: i32[5] = [1, 2, 3, 4, 5]
```

# Lists

Lists are dynamic and can grow or shrink in size.
Type annotation: `list<T>`.

```hhg
let mut l: list<i32> = [1, 2, 3]
l.append(4)
print(l.len()) // 4
```

# Strings

Static strings/string literals are declared with double quotes.
Type annotation: `&static str`.

```hhg
let s: &static str = "Hello, World!"
```

Dynamic strings are UTF-8 and can grow or shrink in size.
Type annotation: `str`.

```hhg
let mut s: str = "Hello, World!"
s.replace("World", "Hedgehog")
```

# Slices

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

# Dicts

Dicts are key-value pairs and are declared with curly braces.
Type annotation: `dict<K, V>`.

```hhg
let d: dict<str, i32> = {"one": 1, "two": 2}
```
