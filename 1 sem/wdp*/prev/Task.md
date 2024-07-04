

Twoim zadaniem jest zaimplementowanie struktury danych umożliwiającej efektywne odpowiadanie na określone zapytania dotyczące ciągu liczb X. Co więcej, kolejne elementy ciągu mogą być ujawniane on-line.
___
Dla ustalonego ciągu 𝑋=𝑥0,𝑥1,…,𝑥𝑛−1
 interesuje nas funkcja:

```cpp
prevInRange(i, [lo, hi])
```
która wyznacza największy indeks j taki, że 0≤𝑗≤𝑖
 oraz 𝑥𝑗∈[𝑙𝑜,ℎ𝑖]
. Jeśli taki indeks nie istnieje, wynikiem funkcji powinno być -1.

Czyli:

𝑝𝑟𝑒𝑣𝐼𝑛𝑅𝑎𝑛𝑔𝑒(𝑖,[𝑙𝑜,ℎ𝑖])=max{0≤𝑗≤𝑖:𝑥𝑗∈[𝑙𝑜,ℎ𝑖]}
 lub -1 (jeśli taki indeks nie istnieje)
___
Twój moduł powinien udostępniać następujące funkcje:
```cpp
void init(const vector<int> &x)
```
Inicjalizacja początkowego ciągu X przez wartości wektora x (uwaga: ciąg może zawierać dowolne wartości mieszczące się w typie int)
int prevInRange(int i, int lo, int hi) – oblicz wartość prevInRange(i, [lo, hi]). Możesz założyć, że 0≤𝑖<|𝑋|
 i 𝙸𝙽𝚃_𝙼𝙸𝙽≤𝑙𝑜≤ℎ𝑖≤𝙸𝙽𝚃_𝙼𝙰𝚇.
___
```cpp
void pushBack(int v)
```
Dodanie na końcu obecnego ciągu X elementu o wartości 𝑣
___
```cpp
void done()
```
Zwolnienie całej pamięci używanej do obsługiwania ciągu X.
Deklaracje podanych funkcji znajdują się w pliku prev.h. Twoim zadaniem jest zaimplementowanie podanych funkcji w pliku prev.cpp.
___

Przykład interakcji znajduje się w załączonym pliku main.cpp.

Komenda kompilacji:

g++ @opcjeCpp main.cpp prev.cpp -o main.e

Aby Twoje rozwiązanie uzyskało maksymalną punktację, (zamortyzowany) koszt czasowy funkcji prevInRange oraz pushBack musi wynosić 𝑂(log𝑧)
, a złożoność funkcji init powinna być w najgorszym razie 𝑂(|𝑋|log𝑧)
, przy czym 𝑧
 to zakres typu int. Za dodatkowy logarytm w złożoności można stracić 1 punkt. Rozwiązanie siłowe dostanie 0, sorry.

Twoje rozwiązanie zostanie także uruchomione za pomocą narzędzia valgrind, które pozwala sprawdzać m.in., czy program nie miał wycieków pamięci. W przypadku wykrycia wycieków pamięci za pomocą komendy:

valgrind --tool=memcheck --leak-check=yes ./main.e
