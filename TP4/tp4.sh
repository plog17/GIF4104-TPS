#PBS -A colosse-users
#PBS -l walltime=600
#PBS -l nodes=1:gpus=2

module load cuda/6.0.37
cd "${PBS_O_WORKDIR}"

./test Image/yoda.png filters/noyau_identite src/convolve.cl

./test Image/yoda.png filters/noyau_identite src/convolveOpt.cl
