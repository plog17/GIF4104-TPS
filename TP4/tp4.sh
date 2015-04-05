#PBS -A colosse-users
#PBS -l walltime=600
#PBS -l nodes=1:gpus=2

module load compilers/gcc/4.9
module load cuda/6.0.37
cd "${PBS_O_WORKDIR}"

./test Image/landscape_hd.png filters/noyau_flou src/convolve.cl
./test Image/landscape_hd.png filters/noyau_flou src/convolve.cl
./test Image/landscape_hd.png filters/noyau_flou src/convolve.cl
./test Image/landscape_hd.png filters/noyau_flou src/convolve.cl

./test Image/landscape_hd.png filters/noyau_flou src/convolveNonOptimised.cl
./test Image/landscape_hd.png filters/noyau_flou src/convolveNonOptimised.cl
./test Image/landscape_hd.png filters/noyau_flou src/convolveNonOptimised.cl
./test Image/landscape_hd.png filters/noyau_flou src/convolveNonOptimised.cl

./test Image/landscape_hd.png filters/noyau_identite src/convolve.cl
./test Image/landscape_hd.png filters/noyau_identite src/convolve.cl
./test Image/landscape_hd.png filters/noyau_identite src/convolve.cl
./test Image/landscape_hd.png filters/noyau_identite src/convolve.cl

./test Image/landscape_hd.png filters/noyau_identite src/convolveNonOptimised.cl
./test Image/landscape_hd.png filters/noyau_identite src/convolveNonOptimised.cl
./test Image/landscape_hd.png filters/noyau_identite src/convolveNonOptimised.cl
./test Image/landscape_hd.png filters/noyau_identite src/convolveNonOptimised.cl
