/* gcc @opcje ori.c -o ori.e -lm */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Structures:
typedef struct {
  double x;
  double y;
}
point; // Structure defining points and linear functions.

typedef struct {
  char type;
  int sheet_number;
  point bottom_left;
  point top_right;
  double circle_diameter;
}
sheet; // Structure defining sheets.

typedef struct {
  point point;
  int sheet_number;
}
query; // Structure defining queries.

// Functions:

// Function checks if a number is close enough to zero to be considered zero
int is_zero(double x) {
  if (fabs(x) < 1e-10) return 1;
  else return 0;
}

// Function calculates square of a given number.
double square(double x) {
  return x * x;
}

// Function checks if a point belongs to a given shape.
int belongs_to(sheet sheet1, point point1) {
  if (sheet1.type == 'K') {
    if (square(sheet1.bottom_left.x - point1.x) + square(sheet1.bottom_left.y - point1.y) <= square(sheet1.circle_diameter)) {
      return 1;
    }
    return 0;
  } else {
    if (point1.x > (sheet1.top_right).x || point1.x < (sheet1.bottom_left).x ||
      point1.y > (sheet1.top_right).y || point1.y < (sheet1.bottom_left).y) {
      return 0;
    }
    return 1;
  }
}

// Function using dot product to determine position of a point relative to a line passing through two points.
double dot_product(point point1, point point2, point point3) {
  double result = point1.x * (point2.y - point3.y) + point2.x * (point3.y - point1.y) + point3.x * (point1.y - point2.y);
  return result;
}

// Function creates a point from given x and y coordinates.
point create_point(double x, double y) {
  point result;
  result.x = x;
  result.y = y;
  return result;
}

// Function determines equation of a linear function based on two points it passes through.
point create_line(point point1, point point2) {
  point result;
  result.x = (point1.y - point2.y) / (point1.x - point2.x);
  result.y = point1.y - (result.x * point1.x);
  return result;
}

// Function determines equation of a line perpendicular to another line passing through a specified point.
point create_perpendicular(point line1, point point1) {
  point result;
  result.x = -1 / line1.x;
  result.y = point1.y - result.x * point1.x;
  return result;
}

// Function determines intersection point of two lines.
point find_intersection(point line1, point line2) {
  point result;
  result.x = (line2.y - line1.y) / (line1.x - line2.x);
  result.y = line1.x * result.x + line1.y;
  return result;
}

// Function calculates symmetric reflection of a point relative to a line passing through two points.
point reflection(point point1, point first_point, point second_point) {
  point result;
  if (is_zero(first_point.y - second_point.y)) {
    result.x = point1.x;
    result.y = point1.y + 2 * (first_point.y - point1.y);
  } else if (is_zero(first_point.x - second_point.x)) {
    result.y = point1.y;
    result.x = point1.x + 2 * (first_point.x - point1.x);
  } else {
    point line1 = create_line(first_point, second_point);
    point line2 = create_perpendicular(line1, point1);
    point intersection_point = find_intersection(line1, line2);

    result.x = 2 * intersection_point.x - point1.x;
    result.y = 2 * intersection_point.y - point1.y;
  }
  return result;
}

// Recursive function determining the number of layers at a given point (seems to work, do not change).
int count_layers(int sheet_number, point point1, sheet * sheets) {
  int result = 0;
  if (sheets[sheet_number - 1].type == 'Z') {
    point point2 = (sheets[sheet_number - 1]).bottom_left;
    point point3 = (sheets[sheet_number - 1]).top_right;
    point point4 = reflection(point1, point2, point3);

    int nowy_numer = (sheets[sheet_number - 1]).sheet_number;

    if (is_zero(dot_product(point2, point3, point1))) {
      result = count_layers((sheets[nowy_numer]).sheet_number, point1, sheets);
    } else if (dot_product(point2, point3, point1) > 0) {
      result = count_layers((sheets[nowy_numer]).sheet_number, point4, sheets);
      result = result + count_layers((sheets[nowy_numer]).sheet_number, point1, sheets);
    }
  } else {
    result = belongs_to(sheets[sheet_number - 1], point1);
  }
  return result;
}

// Function displaying results.
void results(int number_of_queries, sheet * sheets) {
  int result;
  query * queries;
  queries = (query * ) malloc((unsigned long) number_of_queries * sizeof(query));
  for (int i = 0; i < number_of_queries; i++) // Fetching queries.
  {
    double x, y;
    int sheet_number;

    scanf("%d %lf %lf", & sheet_number, & x, & y);
    point point1 = create_point(x, y);
    queries[i].sheet_number = sheet_number;
    queries[i].point = point1;
  }

  for (int i = 0; i < number_of_queries; i++) // Printing answers.
  {
    result = count_layers(queries[i].sheet_number, queries[i].point, sheets);
    printf("%d\n", result);
  }
  free(queries);
}
// Fetching and displaying data.
int main() {
  // Fetching information about the number of queries and sheets.
  int number_of_sheets, number_of_queries, result;
  sheet * sheets;
  scanf("%d %d", & number_of_sheets, & number_of_queries);

  sheets = (sheet * ) malloc((unsigned long) number_of_sheets * sizeof(sheet));
  // Fetching data about sheets.
  for (int i = 0; i < number_of_sheets; i++) {
    sheet sheet1;
    point point1, point2;
    double x, y, radius;
    int numer;
    char type;

    scanf(" %c", & type);
    sheet1.type = type;
    if (type == 'P') // Fetching information about rectangle.
    {
      scanf("%lf %lf", & x, & y);
      point1 = create_point(x, y);

      scanf("%lf %lf", & x, & y);
      point2 = create_point(x, y);

      sheet1.bottom_left = point1;
      sheet1.top_right = point2;

      sheets[i] = sheet1;
    } else if (type == 'K') // Fetching information about circle.
    {
      scanf("%lf %lf %lf", & x, & y, & promien);
      point1 = create_point(x, y);

      sheet1.bottom_left = point1;
      sheet1.circle_diameter = radius;

      sheets[i] = sheet1;
    } else if (type == 'Z') // Fetching information about curved line bending the given sheet.
    {
      scanf("%d", & numer);

      scanf("%lf %lf", & x, & y);
      point1 = create_point(x, y);

      scanf("%lf %lf", & x, & y);
      point2 = create_point(x, y);

      sheet1.sheet_number = numer;
      sheet1.bottom_left = point1;
      sheet1.top_right = point2;

      sheets[i] = sheet1;
    }
  }

  results(number_of_queries, sheets);
  free(sheets);
  return 0;
}
