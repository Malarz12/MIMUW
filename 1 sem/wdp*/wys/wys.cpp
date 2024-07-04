/* g++ @opcje wys_naive.cpp wyslib.cpp -o wys_naive */

#include  "wys.h"
#include  "wyslib.cpp"
#include  <vector>
#include  <map>
#include  <iostream>
#include  <unordered_map>
#include  <climits>


//Zmienne Globalne i pomocnicze strutktury danych 
int stan_poczatkowy = 0;
int maksymalne_ruchy = 28;
int N = 0; // Zakres liczb
int K = 0; // Maksymalna liczba kłamstw
int glebia = 0; // Aktualna głębia rekursji

std::unordered_map<int,int> mapa; // Mapa do przechowywania stanów gry i minimalnej liczby ruchów do rozwiązania
std::vector<int> potegi; // Tablica przechowująca potęgi piątki

// Funkcja inicjalizująca tablicę potęg piątki
void ustaw() 
{
    int p1 = 1;
    for(int i = 0; i <13; i++) 
    {
        potegi.push_back(p1);
        p1 = p1 * 5;
    }
    potegi.push_back(INT_MAX);
}

// Funkcja hashująca
int hashowanie(int a, int b) 
{
    int wynik = a % potegi[b + 1];
    wynik = wynik / potegi[b];
    return wynik;
}

// Funkcja decydująca, czy dany stan gry jest "sprzeczny" i czy należy zakończyć rekursję
bool czy_przerwac(int x) 
{
    bool wynik = true;
    for(int i = 1; i <= N; i++) 
    {
        if(K >= hashowanie(x, i)) wynik = false;
    }
    return wynik;
}

// Funkcja sprawdzająca, czy x jest rozwiązaniem gry
bool czy_wynik(int x) 
{
    int sol = -1;
    bool wynik = true;
    for(int k = 1; k <= N; ++k) 
    {
        if (K >= hashowanie(x, k)) {
            if (sol == -1) sol = x;
            else wynik = false;
        }
    }
    return wynik;
}

// Funkcja aktualizująca stan gry na podstawie odpowiedzi Ali
int aktualizuj(int a, int zapytanie, bool odpowiedz) 
{
    int b = a;
    for(int i = 1; i <= N; i++) 
    {
        if((i < zapytanie && !odpowiedz) || (i >= zapytanie && odpowiedz)) {
            b = b + (std::min(hashowanie(a, i) + 1, K + 1) - hashowanie(b, i)) * potegi[i];
        }
    }
    return b;
}

// Funkcja rekurencyjnie budująca drzewo gry i zwracająca minimalną liczbę ruchów do rozwiązania dla danego stanu
int gra(int x) 
{
    int wynik;
    // Sprawdzenie, czy stan x nie został wcześniej oceniony lub osiągnięto maksymalną liczbę ruchów
    if(mapa.find(x) == mapa.end() || mapa[x] == maksymalne_ruchy) {
        mapa[x] = maksymalne_ruchy;
        if(czy_wynik(x)) {
            wynik = 0;
            mapa[x] = 0;
        }
        // Jeśli osiągnięto maksymalną głębokość rekursji lub należy przerwać (czy_przerwac), ustaw wynik na maksymalną liczbę ruchów
        else if(glebia == 13 || czy_przerwac(x)) {
            wynik = maksymalne_ruchy;
        } 
        else {
            // Początkowa wartość minimalnej cyfry dla pierwszego zapytania
            int mini = hashowanie(x, 1);
            for(int i = 2; i <= N; i++) 
            {
                // Jeśli można jeszcze zadawać pytania (K >= mini)
                if(K >= mini) {
                    int opcja1 = aktualizuj(x, i, false);
                    int a = gra(opcja1);

                    int opcja2 = aktualizuj(x, i, true);
                    int b = gra(opcja2);
                    // Zaktualizuj mapę dla danego stanu x
                    mapa[x] = std::min(mapa[x], std::max(a, b) + 1);
                } 
                else {
                    // Jeśli K nie pozwala już na zadawanie pytań, zaktualizuj mapę dla danego stanu x
                    int pomoc1 = aktualizuj(x, i, false);
                    if(x != pomoc1) mapa[x] = std::min(gra(pomoc1), mapa[x]);
                }
                mini = std::min(mini, hashowanie(x, i)); // Aktualizacja wartości minimalnej cyfry
            }
            wynik = mapa[x];
        }
    } 
    else {
        glebia++; // Inkrementuj głębokość rekursji
        wynik = mapa[x];
    }
    return wynik;
}

// Główna funkcja programu
int main() 
{
    int n, k, g;
    dajParametry(n, k, g); // Pobranie parametrów gry
    ustaw(); // Inicjalizacja tablicy potęg piątki
    N = n;
    K = k;
    // Pętla dla każdej gry
    for(int i = 0; i < g; ++i) 
    { 
        int stan_obecny = stan_poczatkowy;
        while(!czy_wynik(stan_obecny)) // Dopóki nie znajdziemy rozwiązania
        {
            int najlepsze_pytanie = -1;
            int minimalne_ruchy = maksymalne_ruchy;
            for(int pytanie = 1; pytanie <= n; pytanie++) // Przeszukiwanie najlepszego pytania
            {
                int opcja1 = aktualizuj(stan_obecny, pytanie, true);
                int opcja2 = aktualizuj(stan_obecny, pytanie, false);

                int a = gra(opcja1);
                int b = gra(opcja2);
                int maks = std::max(a, b);

                if (maks < minimalne_ruchy) {
                    minimalne_ruchy = maks;
                    najlepsze_pytanie = pytanie;
                }
            }

            // Zadaj pytanie Ali
            bool odpowiedz = mniejszaNiz(najlepsze_pytanie);

            // Uaktualnij stan gry na podstawie odpowiedzi
            stan_obecny = aktualizuj(stan_obecny, najlepsze_pytanie, odpowiedz);
        }

        // Po wyjściu z pętli, mamy rozwiązanie
        for(int x = 1; x <= n; x++) 
        {
            if (hashowanie(stan_obecny, x) <= k) {
                odpowiedz(x); // Odpowiedz
                break;
            }
        }
    }

    return 0;
}