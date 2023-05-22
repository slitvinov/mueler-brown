import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import mmap

n = 1_000_000
period = 10
nbytes = 2 * n * 8
with open("o.bin", "wb") as f:
    f.seek(nbytes - 1, 0)
    f.write(b'\0')
with open("o.bin", "r+b") as f:
    mm = mmap.mmap(f.fileno(), nbytes, mmap.ACCESS_WRITE)
    mueler_brown.trajectory(n, period, mm)
x, y = np.ndarray((2, n), buffer=mm, order='F')
plt.plot(x, y, ',', alpha=0.5)
plt.show()
