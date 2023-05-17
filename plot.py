import numpy as np
import math
import os
import matplotlib.pylab as plt

path = "out.bin"
nbytes = os.path.getsize(path)
dtype = np.dtype("float64")
n = math.isqrt(nbytes // (3 * dtype.itemsize))
with open(path, "rb") as file:
    eng, u, v = np.ndarray((3, n, n), dtype, file.read(), order='F')

mi = np.min(eng)
ma = np.max(eng)
L = ma - mi
levels = np.logspace(0, np.log10(L), 50) + mi
plt.contour(eng, levels=levels)
plt.show()
