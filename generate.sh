seq `nproc` | \
    xargs -P 2 -n 1 sh -c '
    GSL_RNG_SEED=$1 python generate.py -n 100000 -p 1000 -o traj.$1.bin
' dummpy
