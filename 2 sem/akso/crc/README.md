
# CRC Calculator

This NASM program calculates the [CRC](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) remainder using a given polynomial. 

## Full task description

### Files with Gaps
Files in Linux can be "gappy." For the purposes of this task, we assume that a gappy file consists of continuous fragments. At the beginning of a fragment, there is a two-byte length indicating the size of the data in the fragment. Following this are the data. The fragment ends with a four-byte offset, which indicates how many bytes to move from the end of this fragment to the beginning of the next fragment. The length of the data in the block is a 16-bit number in natural binary code. The offset is a 32-bit number in two's complement encoding. Numbers in the file are stored in little-endian order. The first fragment starts at the beginning of the file. The last fragment is identified by the fact that its offset points to itself. Fragments in the file may touch and overlap.

### File Checksum
The file checksum is calculated using the cyclic redundancy check (CRC), taking into account the data in successive fragments of the file. The data of the file is processed byte by byte. We assume that the most significant bit of the data byte and the CRC polynomial (divisor) is written on the left side.

### Command
Implement a program in assembly language called crc, which calculates the checksum of the data contained in the specified gappy file:

```bash
./crc file crc_poly
```
The file parameter is the name of the file. The crc_poly parameter is a sequence of zeros and ones describing the CRC polynomial. We do not record the coefficient of the highest power. The maximum degree of the CRC polynomial is 64 (the maximum length of the CRC divisor is 65). For example, 11010101 represents the polynomial 
𝑥
8
+
𝑥
7
+
𝑥
6
+
𝑥
4
+
𝑥
2
+
1
x 
8
 +x 
7
 +x 
6
 +x 
4
 +x 
2
 +1. A constant polynomial is considered invalid.

The program outputs the calculated checksum to standard output as a string of zeros and ones, ending with a newline character \n. The program indicates successful completion with exit code 0.

The program should use Linux system calls: sys_open, sys_read, sys_write, sys_lseek, sys_close, and sys_exit.

The program should check the validity of parameters and the successful execution of system calls (except for sys_exit). If any parameter is invalid or a system call fails, the program should terminate with exit code 1. In every situation, the program should explicitly call sys_close for the file it opened before exiting.

To achieve satisfactory performance, reads should be buffered. An optimal buffer size should be chosen, and this information should be included in a comment.

### Compilation
The solution will be compiled using the following commands:

```bash
nasm -f elf64 -w+all -w+error -o crc.o crc.asm
ld --fatal-warnings -o crc crc.o
```

### Usage Examples
Examples from the original task can be found in `test.sh`
Any number of other examples can be generated using a CRC calculator.

## Contests
- `crc.asm` contains implementation of the crc program
- `test.sh` is a script for running example tests
- `plik1` is used for the example tests

## Testing
To run example test:
```bash
chmod +x test.sh
./test.sh
```
