WITH co_authors AS (
    SELECT DISTINCT A.Autor AS Autor1, B.Autor AS Autor2
    FROM Autorstwo2 A
    JOIN Autorstwo2 B ON A.Praca = B.Praca
    WHERE A.Autor <> B.Autor
),
hierarchical_query AS (
    SELECT Autor2 AS Autor, LEVEL - 1 AS Level2
    FROM co_authors
    START WITH Autor2 = 'Pilipczuk Mi'
    CONNECT BY NOCYCLE PRIOR Autor2 = Autor1 and LEVEL < 7
),
result_with_levels AS (
    SELECT Autor, MIN(Level2) AS MinLevel
    FROM hierarchical_query
    GROUP BY Autor
)
SELECT
    a.ID AS Autor,
    r.MinLevel AS Poziom
FROM
    Autorzy2 a
LEFT JOIN
    result_with_levels r ON a.ID = r.Autor
ORDER BY
    Poziom, Autor;
