# ECC Memory Manager

[Русский](/README.md)

## Overview

This project implements a memory manager in C that includes error correction codes (ECC) to ensure data integrity. The memory manager utilizes Hamming codes for error detection and correction, providing a robust way to manage memory allocation and deallocation, as well as data operations.

## Objectives

- Develop a memory management system capable of detecting and correcting errors in data related to single-bit errors.
- Implement memory allocation and deallocation using a bitmap.
- Provide functions for writing data with ECC generation and reading data with ECC verification and error correction.
- Simulate errors to test the effectiveness of the ECC implementation.

## Features

- **Memory Allocation and Deallocation**: Uses a bitmap to track allocated memory blocks.
- **ECC Generation and Verification**: Generates ECC for data blocks and verifies them during read operations.
- **Error Correction**: Corrects single-bit errors in data blocks using Hamming codes.
- **Error Simulation**: Introduces random bit errors to test ECC functionality.

## Prerequisites

- C Compiler (e.g., gcc 13.2.0)

## Installation

1. Compile the code:
   ```bash
   gcc -o ecc_memory_manager main.c
   ```
2. Run the program:
   ```bash
   ./ecc_memory_manager
   ```

## Usage

The program demonstrates memory allocation, writing, error simulation, reading, and deallocation with ECC protection. By default, it performs the following steps:

- Allocates a memory block.
- Writes data to the block with ECC generation.
- Simulates an error by flipping a bit in the data.
- Reads the data with ECC verification and error correction.
- Checks if the error was successfully corrected.
- Frees the memory block.

## Architecture

### Data Structures

- **MemoryManager**: Structure containing the data array, ECC array, and allocation bitmap.
  ```c
  typedef struct {
      uint8_t data[DATA_SIZE];
      uint8_t ecc[ECC_SIZE];
      uint8_t bitmap[DATA_SIZE / 4];
  } MemoryManager;
  ```

### Functions

- **Parity Calculation**: Computes the parity bit for a given value.
  ```c
  static inline uint8_t parity(uint8_t x);
  ```

- **ECC Generation**: Computes the ECC for a 4-bit data segment.
  ```c
  void ecc_calculate(uint8_t *data, uint8_t *ecc);
  ```

- **ECC Verification and Correction**: Verifies the ECC and corrects single-bit errors.
  ```c
  void ecc_check_and_correct(uint8_t *data, uint8_t *ecc);
  ```

- **Memory Allocation**: Finds and marks a free memory block in the bitmap.
  ```c
  int ecc_allocate(MemoryManager *mm);
  ```

- **Memory Deallocation**: Marks a memory block as free in the bitmap.
  ```c
  void ecc_deallocate(MemoryManager *mm, int index);
  ```

- **Write Operation**: Writes data to memory and generates ECC.
  ```c
  void ecc_write(MemoryManager *mm, int index, uint8_t *data, int size);
  ```

- **Read Operation**: Reads data from memory, verifies ECC, and corrects errors.
  ```c
  void ecc_read(MemoryManager *mm, int index, uint8_t *data, int size);
  ```

## Testing

The program includes a simple test case that:

- Allocates a memory block.
- Writes sample data to the block.
- Introduces an error by flipping a bit in the data.
- Reads the data and corrects the error using ECC.
- Checks if the data matches the original sample data.

## Results

The program outputs whether the error was successfully corrected:
```
Error fixed!
```
or
```
Fatal error.
```

## References

- Moon, T. K. (2005). *Error Correction Coding: Mathematical Methods and Algorithms*. John Wiley & Sons.
- Silberschatz, A., Gagne, G., & Galvin, P. B. (2013). *Operating System Concepts*. Wiley.
