import math
import random

def monte_carlo(n: int, k: int):
    number_of_all_points: int = 0
    number_of_points_in_circle: int = 0
    pi_estimation: float = 0

    for i in range(n):
        random_x: float = random.uniform(-1, 1)
        random_y: float = random.uniform(-1, 1)

        number_of_all_points += 1
        if random_x ** 2 + random_y ** 2 <= 1: number_of_points_in_circle += 1

        pi_estimation = 4 * number_of_points_in_circle / number_of_all_points

        if i % k == 0 and i != 0:
            print("i = {0}: {1}".format(str(i), str(pi_estimation)))

    print("Difference: {0}".format(str(abs(pi_estimation - math.pi))))


n: int = int(input("Enter N: "))
k: int = int(input("Enter K: "))
monte_carlo(n, k)