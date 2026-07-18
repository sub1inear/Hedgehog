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
| 1 | Postfix / access | `.`, `()`, `[]`, `?`, `!` | left |
| 2 | Unary prefix | `-` (neg), `~`, `*` (deref), `&`, `&mut` | right |
| 3 | Multiplicative | `*`, `/`, `%` | left |
| 4 | Additive | `+`, `-` | left |
| 5 | Shift | `<<`, `>>` | left |
| 6 | Bitwise AND | `&` | left |
| 7 | Bitwise XOR | `^` | left |
| 8 | Bitwise OR | `\|` | left |
| 9 | Range | `..`, `..=` | non-assoc |
| 10 | Comparison | `==`, `!=`, `<`, `>`, `<=`, `>=` | **non-assoc** |
| 11 | Logical NOT | `not` | right |
| 12 | Logical AND | `and` | left |
| 13 | Logical OR | `or` | left |
| 14 | Assignment | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `\|=`, `^=`, `<<=`, `>>=` | right |
