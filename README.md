<h2>Install</h2>

```
python -m pip install .
```

Generate
```
$ python generate.py -h
usage: generate.py [-h] -n NSTEPS -p PERIOD -o OUTPUT

optional arguments:
  -h, --help            show this help message and exit
  -n NSTEPS, --nsteps NSTEPS
                        Number of steps
  -p PERIOD, --period PERIOD
                        Period of recording
  -o OUTPUT, --output OUTPUT
                        Output binary file
```

Generate trajectory on all processes


```
$ cat generate.sh
seq `nproc` | \
    xargs -P 2 -n 1 sh -c '
    GSL_RNG_SEED=$1 python generate.py -n 100000 -p 1000 -o traj.$1.bin
' dummpy
$ sh generate.sh 
$ python view.py -o traj.png traj.*.bin
view.py: traj.png: 200000 points
```
<p align="center" alt="Sampled particle position"><img src="img/traj.png"/></p>

<h2>References</h2>

- Mueller, Klaus, and Leo D. Brown. "Location of saddle points and
minimum energy paths by a constrained simplex optimization procedure."
Theoretica chimica acta 53 (1979): 75-93.
