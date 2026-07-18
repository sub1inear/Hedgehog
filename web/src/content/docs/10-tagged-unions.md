---
title: Tagged Unions
description: A | B unions, ?/! propagation, error types.
order: 10
section: Type System
slug: tagged-unions
---

# Tagged Unions

Hedgehog has tagged unions (`A | B | ...`), often convenient for optionals and results.
Tagged unions are **nominal** (e.g. `(A | B) | C` is the same as `A | B | C`).

- `!type` declares an error type.
- `null` is a constant meaning nothing.
- `expr?` will propagate `null`/`!type` if `expr` is not valid.
- `expr!` will `panic` if `expr` is not valid.

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

## Binding out of a union

```hhg
if let result: u32 = test() {
    print(result)
} else {
    print("no value")
}

match divide(10.0, 2.0) {
    result: f64 => print(result),
    zero_error => print("division by zero"),
}
```
