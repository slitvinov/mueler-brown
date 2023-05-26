import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import mmap

xlo, xhi = -1.5, 1.5
ylo, yhi = -1, 2
xrange = np.linspace(xlo, xhi, 100)
yrange = np.linspace(ylo, yhi, 100)
X, Y = np.meshgrid(xrange, yrange)

for X, Y in zip(X, Y):
	print(X, Y)

n = 100_000
period = 1_000
dtype = np.dtype("<f8")
buf = bytearray(2 * n * dtype.itemsize)
mueler_brown.trajectory(n, period, buf)
x, y = np.ndarray((2, n), buffer=buf, order='F')
plt.plot(x, y, ',', alpha=0.01)
