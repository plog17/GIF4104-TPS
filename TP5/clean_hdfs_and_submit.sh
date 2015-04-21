echo "Cleaning local output folder."
rm -r output
echo "Cleaning hdfs."
rm -rf /scratch/colosse-users/$USER
echo "Submitting magpie hadoop script."
msub magpie_hadoop.msub.ram
