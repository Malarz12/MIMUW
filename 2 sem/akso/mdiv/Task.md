### Dzielenie
Zaimplementuj w asemblerze wołaną z języka C funkcję o następującej deklaracji:

int64_t mdiv(int64_t *x, size_t n, int64_t y);
Funkcja wykonuje dzielenie całkowitoliczbowe z resztą. Funkcja traktuje dzielną, dzielnik, iloraz i resztę jako liczby zapisane w kodowaniu uzupełnieniowym do dwójki. Pierwszy i drugi parametr funkcji określają dzielną: x jest wskaźnikiem na niepustą tablicę n liczb 64-bitowych. Dzielna ma 64 * n bitów i jest zapisana w pamięci w porządku cienkokońcówkowym (ang. little-endian). Trzeci parametr y jest dzielnikiem. Wynikiem funkcji jest reszta z dzielenia x przez y. Funkcja umieszcza iloraz w tablicy x.

Jeśli iloraz nie daje się zapisać w tablicy x, to oznacza wystąpienie nadmiaru (ang. overflow). Szczególnym przypadkiem nadmiaru jest dzielenie przez zero. Funkcja powinna reagować na nadmiar tak jak rozkazy div i idiv, czyli zgłaszać przerwanie numer 0. Obsługa tego przerwania w Linuksie polega na wysłaniu do procesu sygnału SIGFPE. Opis tego sygnału „błąd w obliczeniach zmiennoprzecinkowych” jest nieco mylący.

Wolno założyć, że wskaźnik x jest poprawny oraz że n ma wartość dodatnią.

### Przykład użycia
Przykład użycia jest częścią treści zadania. W szczególności z przykładu użycia należy wywnioskować, jakie są zależności między znakami dzielnej, dzielnika, ilorazu i reszty. Przykład użycia znajduje się w niżej załączonym pliku mdiv_example.c. Można go skompilować i skonsolidować z rozwiązaniem poleceniami:
```bash
gcc -c -Wall -Wextra -std=c17 -O2 -o mdiv_example.o mdiv_example.c
gcc -z noexecstack -o mdiv_example mdiv_example.o mdiv.o
```
### Oddawanie rozwiązania
Jako rozwiązanie należy wstawić w Moodle plik o nazwie mdiv.asm.

### Kompilowanie
Rozwiązanie będzie kompilowane poleceniem:

nasm -f elf64 -w+all -w+error -o mdiv.o mdiv.asm
Rozwiązanie musi się kompilować w laboratorium komputerowym.
