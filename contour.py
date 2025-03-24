import matplotlib.pylab as plt
import numpy as np
import mueler_brown


def sqspace(a, b, n):
    s = (b - a)**0.5
    x = np.linspace(0, s, n)**2
    return x + a


xlo, xhi = -1.5, 1.5
ylo, yhi = -1.0, 2.0
n = 200
xrange = np.linspace(xlo, xhi, n)
yrange = np.linspace(ylo, yhi, n)

Emin0 = -1.4669951720995402e+02
Emin1 = -1.0816672411685234e+02
Emin2 = -8.0767818129659005e+01
kT = 15

E = [[mueler_brown.energy(x, y) for x in xrange] for y in yrange]

levels0 = sqspace(Emin0 + 0.02 * kT, Emin0 + 2.50 * kT, 6)
levels1 = sqspace(Emin1 + 0.02 * kT, Emin1 + 1.00 * kT, 6)
levels2 = sqspace(Emin2 + 0.02 * kT, Emin2 + 1.00 * kT, 6)
levels = np.sort(np.hstack((levels0, levels1, levels2)))

plt.axis("equal")
plt.xlim(xlo, xhi)
plt.ylim(ylo, yhi)

plt.contour(xrange, yrange, E, levels=levels0, colors='r', linestyles='solid')
plt.contour(xrange, yrange, E, levels=levels1, colors='g', linestyles='solid')
plt.contour(xrange, yrange, E, levels=levels2, colors='b', linestyles='solid')

plt.savefig("contour.svg", bbox_inches = "tight")
