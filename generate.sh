seq `nproc` | xargs -P `nproc` -n 1 sh -c '
    GSL_RNG_SEED=$1 python generate.py -n 1000000 -p 1000 -o traj.$1.bin
' dummy
