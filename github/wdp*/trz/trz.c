/* gcc @opcje trz.c -o trz.e*/

#include<stdio.h>

#include<stdlib.h>

#include<stdint.h>

// Function that returns the maximum value of two.
int fmax(int n1, int n2) {
  if (n1 > n2) return n1;
  else return n2;
}

// Function that returns the minimum value of two.
int fmin(int n1, int n2) {
  if (n1 > n2) return n2;
  else return n1;
}

// Function that checks if there are three different motels.
int if_exist(int t1[], int n) {
  int three_different[3];
  three_different[0] = t1[0];
  int j = 1;
  for (int i = 0; i < n; i++) {
    if (j == 1 && t1[i] != three_different[0]) {
      three_different[1] = t1[i];
      j++;
    } else if (j == 2 && t1[i] != three_different[0] && t1[i] != three_different[1]) {
      three_different[2] = t1[i];
      j++;
    }
  }

  if (j >= 3) return 1;
  else return 0;
}

// Function that finds three most distant motels.
int furthest(int t1[], int t2[], int n) {
  int first_three_different[3] = {-1, -1, -1};

  int last_three_different[3] = {-1, -1, -1};

  int first_three_different_distance[3] = {-1, -1, -1};

  int last_three_different_distance[3] = {-1, -1, -1};

  int j = 1;

  first_three_different[0] = t1[0];
  first_three_different_distance[0] = t2[0];
  // This fragment searches for the first three different motels.
  for (int i = 1; i < n; i++) {
    if (j == 1 && (t1[i] != first_three_different[0])) {
      first_three_different[1] = t1[i];
      first_three_different_distance[1] = t2[i];
      j++;
    } else if (j == 2 && (t1[i] != first_three_different[0]) && (t1[i] != first_three_different[1])) {
      first_three_different[2] = t1[i];
      first_three_different_distance[2] = t2[i];
      j++;
    }
  }
  // This fragment searches for the last three different motels.
  j = 1;
  last_three_different[0] = t1[n - 1];
  last_three_different_distance[0] = t2[n - 1];
  for (int i = n - 2; i >= 0; i--) {
    if (j == 1 && (t1[i] != last_three_different[0])) {
      last_three_different[1] = t1[i];
      last_three_different_distance[1] = t2[i];
      j++;
    } else if (j == 2 && (t1[i] != last_three_different[0]) && (t1[i] != last_three_different[1])) {
      last_three_different[2] = t1[i];
      last_three_different_distance[2] = t2[i];
      j++;
    }
  }
  /* This fragment is used to find such a 'middle motel' that the minimum distance 
   * from one of the first three different motels to it and the distance from it to one of the last three different motels 
   * will be possibly the maximum. All these motels will be different from each other.
   */
  int result = 0;
  for (int i = 0; i < n; i++) {
    for (int h = 0; h < 3; h++) {
      for (int k = 0; k < 3; k++) {
        if ((first_three_different[h] != last_three_different[k]) && (last_three_different[k] != t1[i]) && (first_three_different[h] != t1[i])) {
          result = fmax(result, fmin(last_three_different_distance[k] - t2[i], t2[i] - first_three_different_distance[h]));
        }
      }
    }
  }
  return result;
}

// Function that finds the nearest three motels
int closest(int t1[], int t2[], int n) {
  int left = 0;
  int middle = 1;
  int right = 0;

  int h = 0;
  int i = 0;
  int result = INT32_MAX;
  while (left < n - 2) {
    while (t1[left] == t1[left + 1]) {
      left++;
      middle++;
    }
    while ((t1[middle] == t1[left]) && middle < n - 1) middle++;
    h = middle;
    while ((t1[middle] == t1[middle + 1]) && middle < n - 1) middle++;

    while ((t1[middle] == t1[right] || t1[right] == t1[left]) && right < n) right++;
    i = h;
    while (i <= middle && right < n && middle < right && left < middle) {
      if (fmax(t2[i] - t2[left], t2[right] - t2[i]) < result) {
        result = fmax(t2[i] - t2[left], t2[right] - t2[i]);
      }
      i++;
    }
    left = middle;
  }
  return result;
}

/* Function that prints the minimum and maximum. Using the 'if_exist' function, 
 * it also checks if there are three different motels. If not, it prints 0 0.
 */
void printResult(int t1[], int t2[], int n) {
  if (if_exist(t1, n) == 1) printf("%d %d", closest(t1, t2, n), furthest(t1, t2, n));
  else printf("%d %d", 0, 0);
}

int main() {
  // Data input
  int n;
  scanf("%d", & n);
  if (n > 0) {
    size_t size = (size_t) n * sizeof(int);
    int * t1 = (int * ) malloc(size);
    int * t2 = (int * ) malloc(size);
    if (t1 == NULL) {
      return 0;
    }
    if (t2 == NULL) {
      return 0;
    }
    for (int i = 0; i < n; i++) {
      scanf("%d", & t1[i]);
      scanf("%d", & t2[i]);
    }
    printResult(t1, t2, n);
    free(t1);
    free(t2);
  }
  return 0;
}