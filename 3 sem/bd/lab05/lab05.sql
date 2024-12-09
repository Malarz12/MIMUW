WITH initialData AS (
    SELECT * 
    FROM E
    WHERE src = 0
),
pathsV3ToV5 AS (
    SELECT v3.src, COUNT(v5.src) AS c
    FROM E v3, E v4, E v5
    WHERE v3.tgt = v4.src AND v4.tgt = v5.src
    GROUP BY v3.src
)
SELECT SUM(pathsV3ToV5.c) AS result
FROM initialData v1, E v2, pathsV3ToV5
WHERE v1.tgt = v2.src AND v2.tgt = pathsV3ToV5.src;