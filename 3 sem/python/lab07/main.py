import numpy as np

def M_w(wA_grid, wB_grid, x1, x2):
    return 1 / (1 + np.exp(-(wA_grid[:, :, None] * x1 + wB_grid[:, :, None] * x2)))

data = np.array([
    [1.5, 2.1, 0],
    [3.0, 1.5, 1],
    [2.8, 3.0, 1],
    [1.2, 3.5, 0],
    [4.0, 1.0, 1]
])

x1 = data[:, 0]
x2 = data[:, 1]
target = data[:, 2]

wA_values = np.arange(0, 1.1, 0.1)
wB_values = np.arange(2, 3.1, 0.1)
wA_grid, wB_grid = np.meshgrid(wA_values, wB_values, indexing='ij')

M_w_values = M_w(wA_grid, wB_grid, x1, x2)

errors = (target - M_w_values) ** 2

result_matrix = errors.mean(axis=2)

print("Result:  ")
print(result_matrix)
