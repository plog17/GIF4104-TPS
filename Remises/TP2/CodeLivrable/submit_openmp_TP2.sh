# Shell à utiliser pour la tâche
#PBS -S /bin/bash
#PBS -N TACHE_EXAMPLE   # Nom de la tâche
#PBS -A colosse-users  # Identifiant Rap-ID
#PBS -l nodes=1:ppn=8   # Nombre de noeuds et nombre de processus par noeud
#PBS -l walltime=300    # Durée en secondes

# Change le répertoire courant sur le noeud de calcul
cd "${PBS_O_WORKDIR}"

# Charge les modules nécessaires
##module load compilers/intel/14.0 #compilateur intel

# Évite à OpenMP de lancer 16 threads, car il y a 8 seulement coeurs physiques
export OMP_NUM_THREADS=8

# Programme à exécuter
/home/user46/TP2_Livrable/CodeLivrable/TP2 Image/exemple.png filters/noyau_flou test.jpg
