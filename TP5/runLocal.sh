echo "Runnning all tasks"

echo "Cleaning"
./scripts/clean.sh

echo "Compiling"
./scripts/compile.sh

echo "Running Hadoop locally"
./scripts/launch.sh

