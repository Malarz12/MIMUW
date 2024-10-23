# Motel Distance Calculation

## Description

This program calculates the distances between three motels located along a hypothetical straight highway, ensuring that each motel belongs to a different chain. The user is required to input the number of motels along with their chain identifiers and distances from the start of the highway. The program outputs two values: the minimum of the maximum distances and the maximum of the minimum distances between the selected motels.

### Problem Statement

Given `n` motels numbered from `1` to `n`, each associated with a chain represented by integers, the goal is to find three motels `A`, `B`, and `C` such that:

1. The motels are positioned in this order along the highway.
2. All three motels belong to different chains.

The output will consist of:
- The **nearest trio** (minimizing the maximum distance between any two motels).
- The **furthest trio** (maximizing the minimum distance between any two motels).

If no valid trio exists, the program will output `0 0`.


## Implementation Details

The program is implemented in C and operates with a time complexity of **O(n)**, ensuring efficiency even for the upper limit of input size (up to 1,000,000 motels).

## How to Compile and Run

To compile the program, use the following command:

```bash
gcc -o motel_distance motel_distance.c

