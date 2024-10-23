# Bolek's Guessing Game

## Overview

In this game, Bolek is trying to guess a secret number `x` chosen by Ala from a range of numbers from 1 to `n`. Bolek can ask questions of the form "Is `x < y`?" and receives a response of "Yes" or "No." However, Ala may sometimes lie in her responses, and Bolek must figure out the number `x` using the fewest questions possible.

### Functionality

1. **Initialization**: The game parameters are provided via the `dajParametry(int &n, int &k, int &g)` function, which gives:
   - `n`: The range of numbers (1 to `n`).
   - `k`: The maximum number of times Ala can lie.
   - `g`: The number of games to be played with these parameters.

2. **Questioning**: Bolek can ask questions using the `mniejszaNiz(int y)` function, which returns:
   - `true`: if Ala answers "Yes" (indicating that `x < y`).
   - `false`: if Ala answers "No".

3. **Responding**: After determining the number, Bolek will provide his guess using the `odpowiedz(int x)` function.

## Implementation Details

The implementation involves the following key components:

- **Global Variables**: 
  - `stan_poczatkowy`: The initial state of the game.
  - `maksymalne_ruchy`: The maximum number of moves allowed.
  - `N`, `K`, `glebia`: Parameters defining the game's state and limits.

- **Data Structures**: 
  - `std::unordered_map<int, int> mapa`: To store game states and their corresponding minimum moves to a solution.
  - `std::vector<int> potegi`: To hold powers of five, used in hashing.

- **Functions**:
  - `ustaw()`: Initializes the powers of five.
  - `hashowanie(int a, int b)`: Hashing function for game states.
  - `czy_przerwac(int x)`: Checks if the current game state is invalid.
  - `czy_wynik(int x)`: Determines if a game state is a valid solution.
  - `aktualizuj(int a, int zapytanie, bool odpowiedz)`: Updates the game state based on Ala's response.
  - `gra(int x)`: Recursively builds the game tree to find the minimum number of moves.

## Usage

To compile the program, use the following command:

```bash
g++ -o wys_naive wys_naive.cpp wyslib.cpp
