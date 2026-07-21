---
title: Classes, Templates, Interfaces
description: OO in Hedgehog — with structural interfaces and simple generics.
order: 8
section: Type System
slug: classes
---

# Classes

Classes are declared with the `class` keyword.
Make instances with `ClassName(...)`.

```hhg
import std.math
class Point {
    x: f64
    y: f64
    fn dist(&self, p: &Point) -> f64 {
        let dx = self.x - p.x
        let dy = self.y - p.y
        return std.math.sqrt(dx * dx + dy * dy)
    }
}
let p1 = Point(3.0, 4.0)
let p2 = Point(0.0, 0.0)
print(p1.dist(&p2)) // 5.0
```

Field access is with dot notation (`p.x`). No `::`.

`static` methods do not take `self` and are called with `ClassName.method(...)`.

## Standardized methods

| Method | Meaning |
|--------|---------|
| `fn ClassName(...)` | Constructor, automatically generated but can be overridden |
| `fn ~ClassName(&mut self)` | Destructor, automatically generated but can be overridden |
| `fn copy(&self) -> ClassName` | Copy method |
| `fn hash(&self) -> usize` | Hash method |
| `fn print(&self, f: &File)` | Print method |
| `fn str(&self) -> str` | String method |
| `fn len(&self) -> usize` | Length method |
| `fn cap(&self) -> usize` | Capacity method |

# Templates

Generic functions/classes use angle-bracket syntax.

```hhg
fn identity<T>(x: T) -> T {
    return x
}
```

# Interfaces

Interfaces (structurally typed) declare method signatures only.

```hhg
interface Drawable {
    fn draw(&self)
}
```

# Enums

Enums are declared with the `enum` keyword.

```hhg
enum Color {
    Red,
    Green,
    Blue
}
```

Access with dot notation (`Color.Red`).

# Type Aliases

Type aliases are declared with the `type` keyword.

```hhg
type int_t = i32
```
