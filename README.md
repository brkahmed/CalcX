# 🧮 CLI Calculator: Expression Evaluator

This is a command-line calculator that evaluates complex math expressions. It supports arithmetic, functions, constants, factorial, exponentiation, and more.

---

<img src="demo.gif" alt="demo">

---

## 📦 How to Use

### Run from terminal:

- **Single expression mode** (evaluate and exit):

```bash
$ ./calc "2 + 3 * (4 + sin(pi/2))"
17.000000
```

- **REPL mode** (no argument = interactive prompt):

```bash
./calc
>>> 1 + 2
3.000000
>>> ans * 10
30.000000
```

---

## 🧾 Supported Syntax

### 🔹 Numbers

- **Decimal numbers**: `1`, `3.14`, `.5`
- **Negative numbers**: `-5`, `-sqrt(4)`

---

### 🔹 Operators

| Operator | Meaning              | Example                         |
| -------- | -------------------- | ------------------------------- |
| `+`      | Addition             | `2 + 3`, `+1`                   |
| `-`      | Subtraction / Negate | `5 - 2`, `-3`                   |
| `*`      | Multiplication       | `2 * 4`                         |
| `/`      | Division             | `8 / 2`                         |
| `^`      | Exponentiation       | `2 ^ 3 = 8`                     |
| `%`      | Modulo (remainder)   | `10 % 3 = 1`                    |
| `!`      | Factorial            | `5! = 120`                      |
| `!!`     | Chained factorial    | `5!! = factorial(factorial(5))` |
| `\|x\|`  | Absolute value       | `\|-3\| = 3`                    |

---

### 🔹 Implicit Multiplication

You can omit `*` in common cases:

| Expression   | Interpreted as  |
| ------------ | --------------- |
| `2pi`        | `2 * pi`        |
| `3(1 + 2)`   | `3 * (1 + 2)`   |
| `2sin(pi/2)` | `2 * sin(pi/2)` |

---

## 🔣 Built-in Constants

| Name  | Value                | Description                                                              |
| ----- | -------------------- | ------------------------------------------------------------------------ |
| `pi`  | 3.141592653589793    | Ratio of a circle’s circumference to its diameter                        |
| `e`   | 2.718281828459045    | Euler’s number (base of natural logarithm)                               |
| `tau` | 6.283185307179586    | Equal to `2 * pi`; represents a full turn in radians                     |
| `phi` | 1.618033988749895    | Golden ratio: `(1 + sqrt(5)) / 2`                                        |
| `deg` | 0.017453292519943295 | Radians per degree: `pi / 180` (use to convert degrees → radians)        |
| `rad` | 57.29577951308232    | Degrees per radian: `180 / pi` (use to convert radians → degrees)        |
| `ans` | (last result)        | Value of the previous expression result. Useful for chained calculations |

---

## 📚 Built-in Functions

### 🔸 Trigonometric Functions

| Function(s)                    | Description                                   | Example                |
| ------------------------------ | --------------------------------------------- | ---------------------- |
| `sin(x)`                       | Sine of angle `x` (in radians)                | `sin(pi / 2) = 1`      |
| `cos(x)`                       | Cosine of angle `x` (in radians)              | `cos(0) = 1`           |
| `tan(x)`                       | Tangent of angle `x` (in radians)             | `tan(pi / 4) = 1`      |
| `asin(x)`, `arcsin(x)`         | Inverse sine — returns angle (in radians)     | `asin(1) = pi / 2`     |
| `acos(x)`, `arccos(x)`         | Inverse cosine                                | `acos(1) = 0`          |
| `atan(x)`, `arctan(x)`         | Inverse tangent                               | `atan(1) = pi / 4`     |
| `atan2(y, x)`, `arctan2(y, x)` | Inverse tangent of `y/x` considering quadrant | `atan2(1, 1) = pi / 4` |

---

### 🔸 Hyperbolic Functions

| Function(s)              | Description                                          | Example        |
| ------------------------ | ---------------------------------------------------- | -------------- |
| `sinh(x)`                | Hyperbolic sine                                      | `sinh(0) = 0`  |
| `cosh(x)`                | Hyperbolic cosine                                    | `cosh(0) = 1`  |
| `tanh(x)`                | Hyperbolic tangent                                   | `tanh(0) = 0`  |
| `asinh(x)`, `arcsinh(x)` | Inverse hyperbolic sine                              | `asinh(0) = 0` |
| `acosh(x)`, `arccosh(x)` | Inverse hyperbolic cosine (defined for `x` ≥ 1)      | `acosh(1) = 0` |
| `atanh(x)`, `arctanh(x)` | Inverse hyperbolic tangent (defined for `\|x\|` < 1) | `atanh(0) = 0` |

---

### 🔸 Exponential and Logarithmic Functions

| Function(s)       | Description                          | Example              |
| ----------------- | ------------------------------------ | -------------------- |
| `exp(x)`          | Euler's number raised to `x` (`e^x`) | `exp(1) = e ≈ 2.718` |
| `exp2(x)`         | 2 raised to the power `x` (`2^x`)    | `exp2(3) = 8`        |
| `log(x)`, `ln(x)` | Natural logarithm (base `e`)         | `ln(e) = 1`          |
| `log10(x)`        | Logarithm base 10                    | `log10(1000) = 3`    |
| `log2(x)`         | Logarithm base 2                     | `log2(8) = 3`        |

---

### 🔸 Rounding Functions

| Function(s) | Description                                         | Example            |
| ----------- | --------------------------------------------------- | ------------------ |
| `round(x)`  | Rounds `x` to the nearest integer                   | `round(2.5) = 2`   |
| `floor(x)`  | Rounds `x` down to the nearest integer (toward −∞)  | `floor(2.9) = 2`   |
| `ceil(x)`   | Rounds `x` up to the nearest integer (toward +∞)    | `ceil(2.1) = 3`    |
| `trunc(x)`  | Truncates the decimal part of `x` (rounds toward 0) | `trunc(-2.9) = -2` |

---

### 🔸 Power and Root Functions

| Function(s)   | Description                                            | Example           |
| ------------- | ------------------------------------------------------ | ----------------- |
| `sqrt(x)`     | Square root                                            | `sqrt(9) = 3`     |
| `cbrt(x)`     | Cube root                                              | `cbrt(8) = 2`     |
| `pow(x, y)`   | `x` raised to the power of `y`                         | `pow(2, 3) = 8`   |
| `hypot(x, y)` | Euclidean distance `√(x² + y²)` (length of hypotenuse) | `hypot(3, 4) = 5` |

---

### 🔸 General Math Functions

| Function(s)         | Description                                                  | Example              |
| ------------------- | ------------------------------------------------------------ | -------------------- |
| `abs(x)`            | Absolute value of `x`                                        | `abs(-3) = 3`        |
| `mod(x, y)`         | Floating-point remainder of `x / y`                          | `mod(10.5, 3) = 1.5` |
| `gamma(x)`          | Euler’s gamma function: `Γ(x)` (generalization of factorial) | `gamma(6) = 120`     |
| `factorial(x)`      | Computes `x!`                                                | `factorial(5) = 120` |
| `min(a, b, ..., n)` | Minimum value among all arguments                            | `min(4, 1, 6) = 1`   |
| `max(a, b, ..., n)` | Maximum value among all arguments                            | `max(4, 1, 6) = 6`   |
| `erf(x)`            | Error function (used in probability and statistics)          | `erf(0) = 0`         |
| `erfc(x)`           | Complementary error function: `1 - erf(x)`                   | `erfc(0) = 1`        |

---

## 🛠 Error Messages

Your input may trigger descriptive errors like:

- `Syntax error: unexpected ')'`
- `Division by zero`
- `Unsupported function 'cot'`
- `Expected number, got ','`
- `Too many arguments Function 'pow' expected 2 arguments, got 1`

---

## 🧰 Build Instructions

```bash
mkdir build && cd build
# build in debug mode
cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
# build in release mode
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
# run tests
ctest --output-on-failure
```
