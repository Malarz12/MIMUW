# Arithmetic Operations on Approximate Values

## Project Overview

This project implements a set of arithmetic operations for handling approximate values in C. The core idea is to represent physical measurements, which are inherently prone to errors, as intervals of possible values. Each approximate value is treated as a set of potential values, allowing for operations such as addition, subtraction, multiplication, and division on these intervals.

### Key Features
- **Interval Representation**: Approximate values are represented as intervals or exact values with a certain percentage of accuracy.
- **Arithmetic Operations**: The package supports addition, subtraction, multiplication, and division of approximate values.
- **Error Handling**: The implementation ensures proper handling of edge cases such as dividing by zero or working with infinite values, following the IEEE floating-point standard.

## Core Structures and Functions

### Data Structure

- **`struct wartosc`**: A structure representing the approximate value, characterized by its minimum and maximum bounds and a precision value.

### Constructors

- **`wartosc_dokladnosc(x, p)`**: Returns an approximate value `x ± p%` (for `p > 0`).
- **`wartosc_od_do(x, y)`**: Returns the average of `x` and `y` with an error range from `(y-x)/2`.
- **`wartosc_dokladna(x)`**: Returns an exact value `x ± 0`.

### Selectors

- **`in_wartosc(x, y)`**: Checks if the value `y` is within the range of possible values for `x`.
- **`min_wartosc(x)`**: Returns the lower bound of the possible values for `x` (or `-∞` if unbounded).
- **`max_wartosc(x)`**: Returns the upper bound of the possible values for `x` (or `∞` if unbounded).
- **`sr_wartosc(x)`**: Returns the average of `min_wartosc(x)` and `max_wartosc(x)`.

### Arithmetic Modifiers

- **`plus(a, b)`**: Performs addition on two approximate values.
- **`minus(a, b)`**: Performs subtraction on two approximate values.
- **`razy(a, b)`**: Performs multiplication on two approximate values.
- **`podzielic(a, b)`**: Performs division on two approximate values.

## Compilation

To compile the program, ensure that the `ary.c` and `ary.h` files are in the same directory. Use the following command to compile:

```bash
gcc @options example.c ary.c -o example.e -lm
