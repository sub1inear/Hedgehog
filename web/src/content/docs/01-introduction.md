---
title: Introduction
description: What Hedgehog is, and the three principles that guide its design.
order: 1
section: Getting Started
slug: introduction
---

# The Hedgehog Programming Language

Hedgehog is a toy systems programming language.
It has three core principles, listed in order of importance (**I > II > III**).

## I: As Powerful as C++ with the Simplicity of Python and the Safety of Rust

Make programming in Hedgehog and the resulting program as fast as possible, while maintaining safety.
When these conflict: **speed > simplicity > safety**.

## II: Don't Reinvent the Wheel (Aspirational)

No drastic changes from C++ or Python; Hedgehog should look familiar and be easy to learn from either.

## III: There Should Be One, and Only One, Obvious Way to Do It

Duplicates or slight reinventions that cause perplexity when choosing between them should be avoided.

## Hello, World

Every Hedgehog program starts in `fn main()`:

```hhg
fn main() {
    println("Hello, World!")
}
```

`print` writes without a trailing newline; `println` writes with one.
Both can convert any type to a string and format it nicely.

`f`-strings use `{}` to embed expressions:

```hhg
print(f"There are {4 * 5} ducks.")
```

## Installation

The CLI is small on purpose:

| Command | Description |
|---------|-------------|
| `hhg init` | Initializes a new Hedgehog project |
| `hhg build` | Builds the project |
| `hhg run` | Builds and runs the project |
