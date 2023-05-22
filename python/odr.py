import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import scipy.odr
import mmap

n = 10_000
period = 1_000
nbytes = 2 * n * 8
with open("o.bin", "wb") as f:
    f.seek(nbytes - 1, 0)
    f.write(b'\0')
with open("o.bin", "r+b") as f:
    mm = mmap.mmap(f.fileno(), nbytes, mmap.ACCESS_WRITE)
    mueler_brown.trajectory(n, period, mm)
x, y = np.ndarray((2, n), buffer=mm, order='F')


def f0(p, x, y):
    return p[0] + p[1] * x + p[2] * x**2 + p[3] * x**3 + p[4] * x**4 + y


def f(p, D):
    x, y = D
    return f0(p, x, y)


xlo, xhi = -1.5, 1.5
ylo, yhi = -1, 2

model = scipy.odr.Model(f, implicit=1)
data = scipy.odr.Data([x, y], 1)

odr = scipy.odr.ODR(data, model, 5 * [0])
fit = odr.run()

xrange = np.linspace(xlo, xhi, 100)
yrange = np.linspace(ylo, yhi, 100)
X, Y = np.meshgrid(xrange, yrange)
p = fit.beta
plt.xlim(xlo, xhi)
plt.ylim(ylo, yhi)
plt.plot(x, y, ',', alpha=0.5)
plt.contour(X, Y, f0(p, X, Y), [0])
plt.savefig("odr.png")
