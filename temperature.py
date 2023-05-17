import numpy as np
import os
import matplotlib.pylab as plt
import mmap

nfields = 4
path = "out.bin"
nbytes = os.path.getsize(path)
dtype = np.dtype("float64")
n = nbytes // (nfields * dtype.itemsize)
with open(path, "r+b") as file:
    buf = mmap.mmap(file.fileno(), 0)
    x, y, vx, vy = np.ndarray((nfields, n), dtype, buf, order='F')

Ekin = (vx**2 + vy**2)/2
plt.plot(x, y, '-o', alpha=0.1)
print(n, np.mean(Ekin))
plt.show()
