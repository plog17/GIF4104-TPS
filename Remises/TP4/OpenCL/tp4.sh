#PBS -A colosse-users
#PBS -l walltime=600
#PBS -l nodes=1:gpus=2

module load compilers/gcc/4.9
module load cuda/6.0.37
cd "${PBS_O_WORKDIR}"

./tp4 Image/landscape_hd.png filters/noyau_flou output1.png
./tp4 Image/landscape_hd.png filters/noyau_flou output2.png 
./tp4 Image/landscape_hd.png filters/noyau_flou output3.png
./tp4 Image/landscape_hd.png filters/noyau_flou output4.png
./tp4 Image/landscape_hd.png filters/noyau_flou output5.png
./tp4 Image/landscape_hd.png filters/noyau_flou output6.png
./tp4 Image/landscape_hd.png filters/noyau_flou output7.png

./tp4 Image/landscape_hd.png filters/noyau_identite output8.png
./tp4 Image/landscape_hd.png filters/noyau_identite output9.png
./tp4 Image/landscape_hd.png filters/noyau_identite output10.png
./tp4 Image/landscape_hd.png filters/noyau_identite output11.png
./tp4 Image/landscape_hd.png filters/noyau_identite output12.png
./tp4 Image/landscape_hd.png filters/noyau_identite output13.png
./tp4 Image/landscape_hd.png filters/noyau_identite output14.png

