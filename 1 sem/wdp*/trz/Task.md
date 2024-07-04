Wzdłuż autostrady, którą możemy sobie wyobrazić jako linię prostą, rozmieszczonych jest n moteli. Motele numerujemy od 1 do n kolejno wzdłuż autostrady. Każdy z moteli należy do jakiejś sieci moteli, którą opisujemy liczbą z zakresu od 1 do n.

Bajtek powiedział Bitkowi, że w trakcie jazdy autostradą trzy razy zatrzymał się na noc w motelu, przy czym za każdym razem nocował w motelu innej sieci. Bitek zastanawia się, w jak dużych odstępach musiały znajdować się te trzy motele. Interesuje go najbliższa i najdalsza trójka moteli.

Formalnie, Bitek chciałby wyznaczyć trzy motele A, B, C położone w tej kolejności wzdłuż autostrady i należące do różnych sieci, dla których:

- maksimum z odległości od A do B i od B do C jest jak najmniejsze (najbliższa trójka),
- minimum z odległości od A do B i od B do C jest jak największe (najdalsza trójka).

Napisz program, który wczyta z stdin liczbę n moteli, a następnie opisuje każdy motel w kolejności wzdłuż autostrady - dla każdego motelu jego numer sieci i odległość od początku autostrady - i wypisze na stdout dwie liczby - jak powyżej, maksimum z minimów odległości i minimum z maksimów odległości trzech moteli należących do różnych sieci. Jeśli Bajtek pomylił się i żadna trójka moteli różnych sieci nie występuje wzdłuż autostrady, obie liczby powinny być równe zeru.

Dokładny format wejścia i wyjścia opisuje przykład poniżej (na końcach znajdują się końce linii).

## Przykład:

### Wejście:

9  
1 2  
2 6  
2 9  
1 13  
1 17  
3 20  
1 26  
3 27  
1 30  

### Wyjście:

7 10  
