import scipy
import mueler_brown


def dEsq(x):
	fx, fy = mueler_brown.force(*x)
	return fx**2 + fy**2


print("energy minima")
for x0 in ((-0.558, 1.442), (0.623, 0.028), (-0.050, 0.467)):
	opt = scipy.optimize.minimize(lambda x: mueler_brown.energy(*x), x0, (),
	                              'CG', lambda x: mueler_brown.force(*x))
	print("[%+.16e %+.16e] %+.16e" % (*opt.x, opt.fun))

print("saddle points")
for x0 in ((-0.822, 0.624), (0.212, 0.293)):
	opt = scipy.optimize.minimize(dEsq, x0, (), 'CG')
	print("[%+.16e %+.16e] %+.16e" % (*opt.x, opt.fun))
