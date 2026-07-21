---
title: Operators
description: Standard operators and precedence.
order: 4
section: Language Basics
slug: operators
---

# Operators

Standard operators and precedence.

```hhg
let x = 5 + 3 * 2
let y = (x - 4) / 2
let z = false
let bits = 0b1010
bits |= 0b0101
let a = x > 0 and y < 10 or not z
```

## Precedence

| # | Category | Operators | Assoc. |
|---|----------|-----------|--------|
| 1 | Postfix / access | `.`, `()`, `[]`, `?`, `!` | Left |
| 2 | Unary prefix | `-` (neg), `~`, `*` (deref), `&`, `&mut` | Right |
| 3 | Multiplicative | `*`, `/`, `%` | Left |
| 4 | Additive | `+`, `-` | Left |
| 5 | Shift | `<<`, `>>` | Left |
| 6 | Bitwise AND | `&` | Left |
| 7 | Bitwise XOR | `^` | Left |
| 8 | Bitwise OR | `\|` | Left |
| 9 | Range | `..`, `..=` | None |
| 10 | Comparison | `==`, `!=`, `<`, `>`, `<=`, `>=` | None |
| 11 | Logical NOT | `not` | Right |
| 12 | Logical AND | `and` | Left |
| 13 | Logical OR | `or` | Left |
| 14 | Assignment | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `\|=`, `^=`, `<<=`, `>>=` | None |
