# The Hedgehog Programming Language
## Overview
Hedgehog is a toy statically-typed compiled systems language, designed around:
1. Speed, Simplicity, and Safety
2. Familiar syntax to Python and C++
3. One, and only one, obvious way to do things

To accomplish its goals, Hedgehog has a small, learnable specification, and includes a simplified borrow checker with no explicit lifetimes.

It is implemented entirely in C99 for portability and emits C++ with effortless C/C++ interop.

The Hedgehog compiler is in an alpha stage and is currently being implemented.

## Example
```hhg
fn main() {
    println("Hello, World!")
}
```

Compile and run with:
```
hhg run main.hhg
```

## Docs
See the [docs](./docs/main.md) for more info.

## Installation
Download the latest release from the [releases page](https://github.com/sub1inear/Hedgehog/releases).

## Building from Source
```bash
git clone git@github.com:sub1inear/Hedgehog.git
cd Hedgehog
mkdir build
cd build
cmake ..
```
