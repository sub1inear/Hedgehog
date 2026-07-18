---
title: Types
description: Built-in primitive types and literal suffixes.
order: 3
section: Language Basics
slug: types
---

# Types

Hedgehog has many built-in types.

| Name | Size | Range |
|------|------|-------|
| `i8` | signed 8-bit | -128 to 127 |
| `u8` | unsigned 8-bit | 0 to 255 |
| `i16` | signed 16-bit | -32,768 to 32,767 |
| `u16` | unsigned 16-bit | 0 to 65,535 |
| `i32` | signed 32-bit | -2,147,483,648 to 2,147,483,647 |
| `u32` | unsigned 32-bit | 0 to 4,294,967,295 |
| `i64` | signed 64-bit | -9.22×10¹⁸ to 9.22×10¹⁸ |
| `u64` | unsigned 64-bit | 0 to 1.84×10¹⁹ |
| `f32` | 32-bit float | ±3.40×10³⁸ |
| `f64` | 64-bit float | ±1.80×10³⁰⁸ |
| `bool` | 1 byte | `true` or `false` |
| `char` | 4 bytes | Unicode scalar value |
| `usize` | platform-sized unsigned | maximum memory of machine |
| `isize` | platform-sized signed | maximum memory of machine |
| `void` | — | no type |

Literals can be suffixed with type annotations, like Rust.
