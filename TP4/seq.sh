#PBS -A colosse-users
#PBS -l walltime=600
#PBS -l nodes=1:gpus=2

module load compilers/gcc/4.9
cd "${PBS_O_WORKDIR}"

./test Image/landscape_hd.png filters/noyau_flou output.png
./test Image/landscape_hd.png filters/noyau_flou output.png
./test Image/landscape_hd.png filters/noyau_flou output.png


./test Image/landscape_hd.png filters/noyau_identite output.png
./test Image/landscape_hd.png filters/noyau_identite output.png
./test Image/landscape_hd.png filters/noyau_identite output.png

