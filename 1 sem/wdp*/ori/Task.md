## Origami - program do badania warstw kartki papieru

Napisz program dla fanów origami do badania, ile warstw ma w danym punkcie sprytnie poskładana kartka papieru. Program powinien wczytać opis tworzenia kolejnych kartek origami i pytań o ilość warstw w zadanych punktach zadanych kartek z stdin.

### Opis wejścia

W pierwszym wierszu wejścia są dwie liczby `n` i `q` oznaczające odpowiednio liczbę kartek i liczbę pytań. Następnie na wejściu jest `n` wierszy opisujących kolejne kartki i `q` pytań o liczbę warstw zadanych kartek w zadanych punktach.

### Opis kartki

Opis i-tej kartki (1 ≤ i ≤ n) znajduje się w wierszu i + 1. Kartka może być opisana jedną z trzech form:

1. **P x1 y1 x2 y2**: Reprezentuje domknięty prostokąt o bokach równoległych do osi układu współrzędnych, gdzie lewy dolny róg ma współrzędne `(x1, y1)` a prawy górny `(x2, y2)`. Szpilka wbijana wewnątrz lub na krawędziach prostokąta przekłuwa kartkę 1 raz, w innym przypadku 0 razy.

2. **K x y r**: Reprezentuje koło o środku w punkcie `(x, y)` i promieniu `r`. Szpilka wbijana wewnątrz lub na krawędzi koła przekłuwa kartkę 1 raz, w innym przypadku 0 razy.

3. **Z k x1 y1 x2 y2**: Reprezentuje kartkę powstałą przez zgięcie k-tej kartki (1 ≤ k < i) wzdłuż prostej przechodzącej przez punkty `(x1, y1)` i `(x2, y2)`. Przebicie po prawej stronie prostej daje 0 warstw; przebicie na prostej tyle samo co przed złożeniem; po stronie lewej tyle co przed złożeniem plus przebicie kartki w punkcie, który nałożył się na punkt przebicia.

### Opis zapytania

j-te zapytanie (1 ≤ j ≤ q) znajduje się w wierszu n + j + 1 i ma postać k x y, gdzie k jest numerem kartki (1 ≤ k ≤ n), a (x, y) punktem, w którym wbijamy szpilkę, aby określić liczbę warstw k-tej kartki w tym punkcie.

### Wyjście

Program powinien wypisać na stdout `q` wierszy - odpowiedzi na zapytania o liczbę warstw.

Uwagi: Liczby oznaczające współrzędne punktów bądź promień koła są liczbami zmiennopozycyjnymi. Nie ma ograniczeń na złożoność czasową rozwiązania. W szczególności będą dopuszczane rozwiązania wykładnicze. Nasze testy nie są złośliwe ze względu na precyzję obliczeń.

### Przykładowe wejście

4 5  
P 0 0 2.5 1  
Z 1 0.5 0 0.5 1  
K 0 1 5  
Z 3 0 1 -1 0  
1 1.5 0.5  
2 1.5 0.5  
2 0 0.5  
3 4 4  
4 4 4  

### Przykładowe wyjście

1  
0  
2  
1  
2  
