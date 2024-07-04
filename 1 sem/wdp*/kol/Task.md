# Zadanie: Symulacja działania urzędu

Twoim zadaniem jest efektywnie zasymulować zdarzenia występujące w dniu pracy urzędu. W urzędzie jest **m** okienek, ponumerowanych od 0 do **m-1**. W kolejce do każdego okienka ustawiają się interesanci. Gdy urzędnik jest wolny, obsługuje pierwszego interesanta z kolejki, po czym ten opuszcza urząd i już tego dnia nie wraca. Poza tym w urzędzie zdarzają się różne inne sytuacje, opisane przez poniższe funkcje. Twoim zadaniem jest zaimplementować te funkcje.

## Funkcje do zaimplementowania

`void otwarcie_urzedu(int m)`

Ta funkcja zostanie wywołana tylko raz, jako pierwsza. Wywołanie tej funkcji informuje Twoją bibliotekę, że w urzędzie jest **m** okienek (oraz jedno okienko specjalne – o nim później). W urzędzie nie ma jeszcze żadnego interesanta.

`interesant *nowy_interesant(int k)`

Do urzędu wchodzi nowy interesant i od razu ustawia się na końcu kolejki przy okienku o numerze **k**. Funkcja powinna stworzyć nowy element typu interesant i dać w wyniku wskaźnik na ten element.

`int numerek(interesant* i)`

Zwraca numerek interesanta **i**. Każdy interesant po przyjściu do urzędu od razu dostaje numerek. Numerki zaczynają się od 0 i są kolejnymi liczbami całkowitymi.

`interesant *obsluz(int k)`

Urzędnik obsługujący okienko o numerze **k** próbuje obsłużyć kolejnego interesanta. Jeśli kolejka do okienka jest niepusta, pierwszy interesant w kolejce zostaje obsłużony i wychodzi z urzędu; wówczas wynikiem funkcji powinien być wskaźnik na tego interesanta. W przeciwnym przypadku nikt nie zostaje obsłużony, a wynikiem funkcji powinno być NULL.

`void zmiana_okienka(interesant *i, int k)`

Interesant **i** orientuje się, że stoi w kolejce do niewłaściwego okienka. Opuszcza swoją kolejkę i ustawia się na końcu kolejki do okienka **k**, którą obecnie uważa za właściwą.

`void zamkniecie_okienka(int k1, int k2)`

Urzędnik obsługujący okienko numer **k1** udaje się na przerwę. Wszyscy interesanci stojący w kolejce do tego okienka zostają skierowani do okienka numer **k2** i ustawiają się na końcu kolejki przy okienku **k2** w tej samej kolejności, w jakiej stali w kolejce do okienka **k1**.

`std::vector<interesant*> fast_track(interesant *i1, interesant *i2)`

Urzędnik otwiera na chwilę okienko specjalne, w którym można szybko obsłużyć wszystkie sprawy. Pewna grupa interesantów stojących dotychczas kolejno w jednej kolejce orientuje się, co się dzieje, i natychmiast przechodzi do okienka specjalnego, gdzie zostaje od razu obsłużona w tej samej kolejności, w jakiej stali w swojej dotychczasowej kolejce, po czym opuszcza urząd, a okienko specjalne się zamyka.

`void naczelnik(int k)`

Raz po raz ze swojego gabinetu wygląda naczelnik urzędu i z nudów przestawia osoby ustawione w kolejce do okienka numer **k**. Za każdym razem wybiera on jedno okienko numer **k** i nakazuje wszystkim interesantom ustawionym w kolejce do tego okienka odwrócić kolejność ich ustawienia w kolejce.

`std::vector<interesant *> zamkniecie_urzedu()`

Ta funkcja zostanie wywołana raz, na koniec interakcji z biblioteką. Oznacza koniec dnia pracy urzędu. Wszyscy pozostali interesanci zostają szybciutko obsłużeni i urząd na ten dzień się zamyka. Wynikiem funkcji powinien być vector wszystkich interesantów, którzy do tego momentu zostali w urzędzie, w kolejności: najpierw wszyscy stojący w kolejce do okienka 0 (w kolejności obsłużenia), potem wszyscy stojący w kolejce do okienka 1 (w kolejności obsłużenia) itd.

## Uwagi dodatkowe

Twoja biblioteka nie zwalnia pamięci po żadnym interesancie. Jest to w odpowiedzialności użytkownika biblioteki. Użytkownik zwalnia pamięć dopiero wtedy, gdy interesant opuszcza urząd. Uwaga: Użytkownik będzie zwalniał pamięć po interesantach za pomocą funkcji `free`, więc Twoja biblioteka musi alokować pamięć za pomocą funkcji `malloc`.

### Deklaracje funkcji

Deklaracje podanych funkcji znajdują się w pliku `kol.h`. Twoim zadaniem jest uzupełnić w pliku `kol.h` definicję typu `struct interesant` (nie zmieniając nic więcej w tym pliku) oraz zaimplementować podane funkcje w pliku `kol.cpp`.

## Komenda kompilacji

```sh
g++ @opcjeCpp main.cpp kol.cpp -o main.e
