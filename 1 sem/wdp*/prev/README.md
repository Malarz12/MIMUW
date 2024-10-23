# Project Title: Efficient Interval Query Data Structure

## Overview

This project implements a data structure that allows for efficient online queries on a sequence of integers. Specifically, it provides functionality to find the largest index in a sequence such that the value at that index falls within a specified range. The key function implemented is `prevInRange(i, [lo, hi])`, which identifies the largest index \( j \) for given conditions.

## Problem Statement

Given a sequence the goal is to support the following operations:

- **Initialize the sequence**: `init(const vector<int> &x)` – Initializes the sequence \( X \) with values from the provided vector.
- **Add an element**: `pushBack(int v)` – Appends a new integer \( v \) to the end of the sequence.
- **Query the maximum index**: `prevInRange(int i, int lo, int hi)` – Returns the largest index \( j \) such that \( 0 \leq j \leq i \) and \( x_j \in [lo, hi] \). If no such index exists, it returns -1.
- **Memory cleanup**: `done()` – Frees all memory allocated for the data structure.

## Implementation Details

The implementation uses an interval tree structure, which allows for efficient searching and updating. The tree is divided into positive and negative segments to handle both types of values separately. The main components include:

- **Tree Node Structure**: Each node stores an interval, a value, and pointers to its left and right children.
- **Tree Creation and Updating**: Functions to create new trees and update existing ones based on incoming values.
- **Search Functionality**: A recursive search method to find the maximum value within a given interval.

### Time Complexity

- **Initialization (`init`)**: \( O(|X| \log z) \), where \( z \) is the range of integers.
- **Adding Elements (`pushBack`)**: Amortized \( O(log z) \).
- **Range Queries (`prevInRange`)**: Amortized \( O(log z) \).

### Memory Management

The project emphasizes proper memory management, ensuring that all dynamically allocated trees are deleted correctly to prevent memory leaks. The implementation has been tested with tools like Valgrind to verify the absence of memory leaks.
