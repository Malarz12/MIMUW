
# Long number division

Implementation of the mdiv function in nasm assembly, which performs integer division with remainder.
Function divides 64*n bits number by 64 bit number, both in two's complement.

## Original task
[Original task in Polish](./task_description.pdf)

## Task description

Implement a callable C function in assembly with the following declaration:

```c
int64_t mdiv(int64_t *x, size_t n, int64_t y);
```
This function performs integer division with remainder. The function treats the dividend, divisor, quotient, and remainder as numbers represented in two's complement encoding. The first and second parameters of the function specify the dividend: x is a pointer to a non-empty array of n 64-bit numbers. The dividend has 64 * n bits and is stored in memory in little-endian order. The third parameter y is the divisor. The result of the function is the remainder of the division of x by y. The function places the quotient in the array x.

If the quotient cannot be stored in the array x, it indicates an overflow condition. A special case of overflow is division by zero. The function should respond to overflow in the same way as the div and idiv instructions, which is to raise interrupt number 0. Handling this interrupt in Linux involves sending the SIGFPE signal to the process. The description of this signal as "floating-point exception" can be somewhat misleading.

It is permissible to assume that the pointer x is valid and that n is a positive value.

### Example Usage
An example usage is part of the task description. In particular, from the example usage, one should infer the dependencies between the signs of the dividend, divisor, quotient, and remainder. The example usage can be found in the attached file mdiv_example.c. It can be compiled and linked using the following commands:

```bash
gcc -c -Wall -Wextra -std=c17 -O2 -o mdiv_example.o mdiv_example.c
gcc -z noexecstack -o mdiv_example mdiv_example.o mdiv.o
```
### Submission
As a solution, please submit a file named mdiv.asm on Moodle.

### Compilation
The solution will be compiled using the following command:

```bash
nasm -f elf64 -w+all -w+error -o mdiv.o mdiv.asm
```

### Grading
The grading will consist of two parts.

Compliance with the specification will be assessed using automated tests, for which a maximum of 7 points can be awarded. Compliance with ABI rules, memory access correctness, and memory usage will also be checked. The score from the automated tests will be reduced by a value proportional to the amount of additional memory used by the solution (sections .bss, .data, .rodata, stack, heap). Additionally, a threshold size for the .text section will be established. Exceeding this threshold will result in a deduction proportional to the extent of the excess. An additional criterion for automatic evaluation will be the execution speed of the solution. Solutions that are too slow will not receive the maximum grade. A point will be deducted for an incorrect file name.

For formatting and code quality, a maximum of 3 points can be awarded. Traditional assembly program formatting involves starting labels in the first column and instruction mnemonics from a chosen fixed column. No other indentation is used. This format is consistent with the examples shown in class. The code should be well-commented, meaning that each block of code should include information about what it does. The purpose of registers should be described. All key or non-trivial lines of code require comments. In the case of assembly, it is not an exaggeration to comment on nearly every line of code, but comments that describe what is visible should be avoided.

## Testing
To run example tests:
```bash
make
./mdiv_example
make clean
