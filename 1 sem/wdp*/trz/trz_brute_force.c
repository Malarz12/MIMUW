#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <limits.h>

// Function that returns the maximum value of two.
int max(int n1, int n2) {
  if (n1 > n2) return n1;
  else return n2;
}

// Function that returns the minimum value of two.
int min(int n1, int n2) {
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
    int result = 0;
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            for(int k = j + 1; k < n; k++) {
                if(t1[i] != t1[j] && t1[k] != t1[i] && t1[k] != t1[j]) {
                    result = max(result, min(t2[j] - t2[i], t2[k] - t2[j]));
                }
            }
        }
    }
    return result;
}

// Function that finds the nearest three motels
int closest(int t1[], int t2[], int n) {
    int result = INT_MAX;
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            for(int k = j + 1; k < n; k++) {
                if(t1[i] != t1[j] && t1[k] != t1[i] && t1[k] != t1[j]) {
                    result = min(result, max(t2[j] - t2[i], t2[k] - t2[j]));
                }
            }
        }
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
