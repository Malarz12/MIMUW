# Opis rozwiązania zadania Domino

## Wprowadzenie

Program oblicza maksymalną sumę elementów w dwuwymiarowej tablicy liczb, stosując kombinacje pionowe i poziome 2x1, w kształcie 'domino'. W tym celu użyto programowania dynamicznego oraz masek bitowych do śledzenia, które elementy zostały już wybrane.

## Struktura klasy `MaskGenerator`

Klasa `MaskGenerator` pełni rolę pomocniczą w generowaniu wszsytkich 'odpowiednich' masek bitowych dla poszczególnych kolumn tablicy. Ma ona następujące pola:

- `prev_sum`: przechowuje maksmylaną sumę osiągniętą dla obecnie modyfikowanej maski bitowej.
- `to_add`: suma elementów, którą należy dodać w bieżącej iteracji.
- `current_row`: indeks bieżącego wiersza, w którym znajduje się obliczany element.
- `new_mask`: bitowa reprezentacja wyboru elementów w bieżącej kombinacji.

## Funkcje operacyjne

### 1. `vertical_combination`

Funkcja ta realizuje pionową kombinację elementów, czyli sumuje wartości dwóch sąsiadujących elementów w tej samej kolumnie, jeśli spełnione są odpowiednie warunki:
- W bieżącym wierszu i kolejnym wierszu elementy mogą być sumowane tylko wtedy, gdy odpowiadająca im pozycja w masce `mask` jest pusta (`0`).
- Jeśli suma jest dodatnia, dodajemy ją do nowego obiektu `MaskGenerator` i umieszczamy w wektorze `masks`.

### 2. `horizontal_combination`

Funkcja `horizontal_combination` tworzy kombinacje poziome:
- Sumuje wartości w bieżącym wierszu i kolejnej kolumnie.
- Jeśli suma jest dodatnia, dodajemy ją do tablicy 'masks' z nowo utworzoną maską, która oznacza, że bieżący wiersz w kolejnej kolumnie został już wybrany.

### 3. `move_to_next_row`

Ta funkcja pozwala przejść do następnego wiersza bez łączenia elementów. Dzięki temu rozwiązanie uwzglęnia puste maski w na danych pozycjach.

## Funkcja `process_column`

Funkcja `process_column` przetwarza każdą kolumnę w tablicy:
1. `next` zostaje zainicjalizowany jako wektor z wartością `LLONG_MIN`, aby reprezentować nieaktywne kombinacje.
2. Przechodzimy przez każdą możliwą maskę `mask`, sprawdzając, czy suma w `cur[mask]` jest aktywna.
3. Tworzymy wektor `masks`, który reprezentuje wszystkie możliwe kombinacje w tej kolumnie.
4. Dla każdej maski tworzymy obiekt `MaskGenerator`, a następnie przechodzimy przez różne kombinacje pionowe, poziome i przejścia do następnego wiersza.
5. Wartości `to_add` i `prev_sum` są wykorzystywane do aktualizacji największej osiągniętej sumy `res` oraz wartości w `next`.

