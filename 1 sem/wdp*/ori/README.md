## Origami Layer Calculation Program

### Project Overview
This project is designed to calculate the number of paper layers at a given point on a folded origami sheet. The program reads descriptions of origami sheets and queries from standard input, then computes how many layers of paper exist at specific points after folding.

### Input
The input consists of two main parts:
1. **Number of sheets and queries**: 
   - The first line contains two integers, `n` (the number of sheets) and `q` (the number of queries).
2. **Sheet descriptions**: 
   - The next `n` lines describe each sheet using one of the following formats:
     - Rectangle: `P x1 y1 x2 y2`, where `(x1, y1)` is the bottom-left corner and `(x2, y2)` is the top-right corner.
     - Circle: `K x y r`, where `(x, y)` is the center and `r` is the radius.
     - Folded sheet: `Z k x1 y1 x2 y2`, where sheet `k` is folded along a line passing through `(x1, y1)` and `(x2, y2)`.

3. **Queries**: 
   - The final `q` lines represent the queries, each asking how many layers are present on sheet `k` at point `(x, y)`: `k x y`.

### Output
For each query, the program outputs the number of layers at the specified point.

### Example Input
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

### Solution Description
The solution to the problem is implemented using a combination of geometry and recursive logic to simulate the folding of origami sheets and calculate the number of layers at specific points.

1. **Sheet Representation**: Each sheet is stored as a structure. Depending on the type (`P` for rectangle, `K` for circle, `Z` for folded sheet), different properties are used to describe the sheet.
   - For rectangles, we store the coordinates of the bottom-left and top-right corners.
   - For circles, we store the center point and the radius.
   - For folded sheets, we store a reference to the original sheet that was folded, and the coordinates defining the line along which it was folded.

2. **Point-in-Shape Calculation**: 
   - For rectangles, the program checks if a point is inside the boundaries defined by the corners.
   - For circles, the program calculates the distance from the point to the circle's center and compares it to the radius.

3. **Folding Simulation**: 
   - When a sheet is folded along a line, the program uses reflection geometry to determine the corresponding point on the opposite side of the fold.
   - The recursive function calculates the number of layers by determining whether the point falls on one side of the fold or the other. If it’s on the left side of the fold, the point is reflected and the recursive process repeats for both sides of the fold.

4. **Layer Counting**: 
   - The number of layers at a point is determined by recursively unfolding the sheet and checking both the original and reflected points.
   - For each query, the program determines how many layers exist by traversing the folding structure and summing the number of layers at each stage.

This recursive approach allows the program to handle complex folding scenarios and ensures accurate layer counting at any given point on the folded sheet.
