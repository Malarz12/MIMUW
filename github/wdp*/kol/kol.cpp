#include  "kol.h"
#include  <cstdlib>
#include  <cassert>
#include  <cstdio>
#include  <cstdlib>
#include  <vector>
#include  <iostream>

using namespace std;
//strutura danych przedstawiająca listę dwukierunkową oraz jej konstruktor
typedef struct lista {
  interesant * head;
  interesant * tail;
}
lista;
lista stworz_liste() {
  lista wynik;

  wynik.head = (interesant * ) malloc(sizeof(interesant));
  wynik.tail = (interesant * ) malloc(sizeof(interesant));

  wynik.head -> l1 = NULL;
  wynik.head -> l2 = wynik.tail;

  wynik.tail -> l1 = NULL;
  wynik.tail -> l2 = wynik.head;

  return wynik;
}
//dwie zmienne globalne. vector ,kolejki, - przechowuje informacje o kolejkach do wszystkich stanowisk,
// a int 'numer począkowy' - słuzy do wskazywania, jako ktry pojawił się interesant w urzędzie
vector < lista > kolejki;
int akt_numerek;

//Zbior wszystkich funkcji pomocniczych:
//Funkcje zwracające int'a
int numerek(interesant * i) {
  //zwraca numer porządkowy danego interesanta
  return i -> val;
}
//Funkcje zwracające bool'a
bool czy_pusta(const lista l) {
  //wskazuje czy lista jest pusta
  if (l.head -> l2 == l.tail) return true;
  else return false;
}
//Funkcje typu void:
void polacz(interesant * el, interesant * old, interesant * nw) {
  //łączy dwa elemnty
  if (el) {
    if (el -> l1 == old) {
      el -> l1 = nw;
    } else {
      el -> l2 = nw;
    }
  }
}
void dodaj_na_koniec(lista & l, interesant * i) {
  //dodaje osobę do kolejki
  i -> l2 = l.tail;
  i -> l1 = l.tail -> l2;
  polacz(l.tail -> l2, l.tail, i);
  l.tail -> l2 = i;
}
void usun_interesanta(interesant * el) {
  //usuwa daną osobę z kolejki
  polacz(el -> l1, el, el -> l2);
  polacz(el -> l2, el, el -> l1);
}
void usun_z_przodu(lista & l) {
  //usuwa pierwszą osobę z kolejki
  if (czy_pusta(l) == false) {
    usun_interesanta(l.head -> l2);
  }
}
void odwroc(lista & l) {
  //odwraca listę
  interesant * tmp = l.head;
  l.head = l.tail;
  l.tail = tmp;
}
void polacz_listy(lista & l1, lista & l2) {
  //dodaje listę na koniec drugiej
  interesant * b = l1.tail;
  interesant * f = l2.head;
  polacz(b -> l2, b, f -> l2);
  polacz(f -> l2, f, b -> l2);
  b -> l2 = f;
  f -> l2 = b;
  l1.tail = l2.tail;
  l2.head = f;
  l2.tail = b;
}
void otwarcie_urzedu(int m) {
  //fukcja ustwiająca rozmiar vectora 'kolejki', na dany 'dzień'
  kolejki.resize(m);
  for (int i = 0; i < m; i++) {
    kolejki[i] = stworz_liste();
  }
  akt_numerek = 0;
}
void zmiana_okienka(interesant * i, int k) {
  //słuzy do zmiany połozenia jednego interesnanta między kolejkami
  usun_interesanta(i);
  dodaj_na_koniec(kolejki[k], i);
}
void zamkniecie_okienka(int k1, int k2) {
  //zamyka dane okno i przenosi wszystkich czekających do drugiego
  if (k1 != k2) {
    polacz_listy(kolejki[k2], kolejki[k1]);
  }
}
void naczelnik(int k) {
  //odwraca kolejkę do danego stoiska
  odwroc(kolejki[k]);
}

//Funkcje zwracjące strukturę 'interesant'
interesant * head_value(const lista l) {
  //funkcja zwracająca pierwszego interesanta w danej kolejce
  return l.head -> l2;
}
interesant * tail_value(const lista l) {
  //funkcja zwracjąca ostatniego interesanta w danej kolejce
  return l.tail -> l2;
}
interesant * nowy_interesant(int k) {
  //funkcja tworząca nowego interesanta
  interesant * i = (interesant * ) malloc(sizeof(interesant));
  i -> val = akt_numerek++;
  dodaj_na_koniec(kolejki[k], i);
  return tail_value(kolejki[k]);
}
interesant * obsluz(int k) {
  //funkcja zwracająca pierwszego interesanta z danej kolejki i go z niej usuwająca
  if (czy_pusta(kolejki[k]) == false) {
    interesant * i = head_value(kolejki[k]);
    usun_z_przodu(kolejki[k]);
    return i;
  } else {
    return NULL;
  }
}

//Funkcje zwracające tablice interesantow
vector < interesant * > fast_track(interesant * i1, interesant * i2) {
  /*funkcja tworzy wektor obsluzonych interesantow i ewentualnie łączye dwie 'odrębnie części danej kolejki'*/
  //wynik programu
  vector < interesant * > wynik;

  //vector zaweirający dwa wvector interesant, ktore są mozliwymi wynikami
  vector < vector < interesant * >> v1(2);
  v1[0].push_back(i1);
  v1[1].push_back(i1);
  //vector zaweirający wskaźniki słuzace do przeszukania kolejki
  vector < interesant * > v2(2);
  v2[0] = i1;
  v2[1] = i1;
  //tablica zawierająca kierunki
  int kierunki[2] = {
    1,
    -1
  };

  //pętla słuy do przeszukania kolejki i znaleźienia połozenia i2 względem i1
  while ((v2[0] == i2 || v2[1] == i2) == false) {
    for (int i = 0; i < 2; i++) {
      if (kierunki[i] == 1) {
        if (v2[i] -> l1 != NULL) {
          if (v2[i] -> l1 -> l1 == v2[i]) {
            kierunki[i] = -1;
          }
          v1[i].push_back(v2[i] -> l1);
          v2[i] = v2[i] -> l1;
        }
      } else {
        if (v2[i] -> l2 -> l2 == v2[i]) {
          kierunki[i] = 1;
        }
        v1[i].push_back(v2[i] -> l2);
        v2[i] = v2[i] -> l2;
      }
    }
  }
  //Sprawdzenie, ktory vector powinien być wynikiem oraz ewentualne połączenie 'przerwanej' listy
  if (v2[0] == i2) wynik = v1[0];
  else wynik = v1[1];

  if (v2[0] != i2 && kierunki[1] == -1) {
    polacz(i1 -> l1, i1, i2 -> l2);
    polacz(i2 -> l2, i2, i1 -> l1);
  } else if (v2[0] != i2 && kierunki[1] == 1) {
    polacz(i1 -> l1, i1, i2 -> l1);
    polacz(i2 -> l1, i2, i1 -> l1);
  } else if (v2[0] == i2 && kierunki[0] == -1) {
    polacz(i1 -> l2, i1, i2 -> l2);
    polacz(i2 -> l2, i2, i1 -> l2);
  } else if (v2[0] == i2 && kierunki[0] == 1) {
    polacz(i1 -> l2, i1, i2 -> l1);
    polacz(i2 -> l1, i2, i1 -> l2);
  }
  //zwracanie wyniku
  return wynik;

}
vector < interesant * > zamkniecie_urzedu() {
  //funkcja zwalnia pamieć we wszystkich listach w vectorze 'kolejki' oraz zwraca vector zawierający wszsytkich pozostałych interesantow
  vector < interesant * > wszyscy;
  for (int i = 0; i < (int) kolejki.size(); i++) {
    while (czy_pusta(kolejki[i]) == false) {
      wszyscy.push_back(head_value(kolejki[i]));
      usun_z_przodu(kolejki[i]);
    }

    free(kolejki[i].tail);
    free(kolejki[i].head);
  }

  return wszyscy;
}