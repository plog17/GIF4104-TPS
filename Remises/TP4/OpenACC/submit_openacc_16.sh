# Shell à utiliser pour la tâche
#PBS -S /bin/bash                                                                                                                                                                                                  
#PBS -N openacc_compile       # Nom de la tâche                                                                                                                                                                    
#PBS -A colosse-users  # Identifiant Rap; ID                                                                                                                                                                       
#PBS -l nodes=1:gpus=16                                                                                                                                                                                             
#PBS -l walltime=60    # Durée en secondes  

# Change le répertoire courant sur le noeud de calcul
cd "${PBS_O_WORKDIR}"

# Charge les modules nécessaires
##module load compilers/intel/14.0 #compilateur intel

# Évite à OpenMP de lancer 16 threads, car il y a 8 seulement coeurs physiques
export OMP_NUM_THREADS=8
export PGI_ACC_DEBUG=0
export PGI_ACC_TIME=0

# Programme à exécuter
/home/user46/GIF4104-TPS/TP4_OpenACC/src/tp4-openacc Image/landscape_hd.png filters/noyau_flou