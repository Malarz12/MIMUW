# Zadanie: Arytmetyka przybliżonych wartości

## Implementacja pakietu operacji arytmetycznych

### Cel

Zaimplementuj pakiet operacji arytmetycznych na przybliżonych wartościach zgodnie z interfejsem określonym w pliku `ary.h`.

### Wymagania

1. **Struktura `struct wartosc`**: Definicja typu danych przechowującego przybliżone wartości.

2. **Konstruktory**:
   - `wartosc_dokladnosc(x, p)`: Tworzy wartość `x ± p%`.
   - `wartosc_od_do(x, y)`: Tworzy `(x+y)/2 ± (y-x)/2` dla `x < y`.
   - `wartosc_dokladna(x)`: Tworzy dokładną wartość `x ± 0`.

3. **Selektory**:
   - `in_wartosc(x, y)`: Sprawdza, czy wartość `x` należy do zbioru możliwych wartości `y`.
   - `min_wartosc(x)`: Zwraca dolny kres możliwych wartości `x`.
   - `max_wartosc(x)`: Zwraca górny kres możliwych wartości `x`.
   - `sr_wartosc(x)`: Zwraca średnią arytmetyczną dolnego i górnego kresu możliwych wartości `x`.

4. **Modyfikatory**:
   - `plus(a, b)`: Zwraca wynik dodawania dwóch przybliżonych wartości `a` i `b`.
   - `minus(a, b)`: Zwraca wynik odejmowania dwóch przybliżonych wartości `a` i `b`.
   - `razy(a, b)`: Zwraca wynik mnożenia dwóch przybliżonych wartości `a` i `b`.
   - `podzielic(a, b)`: Zwraca wynik dzielenia dwóch przybliżonych wartości `a` przez `b`.

### Uwagi

- Operacje na wartościach przybliżonych są monotoniczne.
- Implementacja powinna obsługiwać wartości specjalne: `HUGE_VAL`, `-HUGE_VAL` i `NAN`.
- Kompilacja wymaga opcji `-lm` na końcu, aby załączyć bibliotekę matematyczną.

### Przykładowa kompilacja

```bash
gcc przyklad.c ary.c -o przyklad.e -lm
