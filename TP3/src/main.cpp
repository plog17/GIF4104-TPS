//
//  main.cpp
//

#include "Matrix.hpp"

#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <iostream>
#include <stdexcept>

using namespace std;

// Inverser la matrice par la méthode de Gauss-Jordan; implantation séquentielle.
void invertSequential(Matrix& iA) {

    // vérifier que la matrice est carrée
    assert(iA.rows() == iA.cols());
	// construire la matrice [A I]
	MatrixConcatCols lAI(iA, MatrixIdentity(iA.rows()));

    // traiter chaque rangée
	for (size_t k=0; k<iA.rows(); ++k) {
		// trouver l'index p du plus grand pivot de la colonne k en valeur absolue
		// (pour une meilleure stabilité numérique).
		size_t p = k;
        double lMax = fabs(lAI(k,k));
        for(size_t i = k; i < lAI.rows(); ++i) {
            if(fabs(lAI(i,k)) > lMax) {
                lMax = fabs(lAI(i,k));
                p = i;
            }
        }

        // vérifier que la matrice n'est pas singulière
		if (lAI(p, k) == 0) throw runtime_error("Matrix not invertible");

        // échanger la ligne courante avec celle du pivot
		if (p != k) lAI.swapRows(p, k);

		double lValue = lAI(k, k);
		for (size_t j=0; j<lAI.cols(); ++j) {
			// On divise les éléments de la rangée k
			// par la valeur du pivot.
			// Ainsi, lAI(k,k) deviendra égal à 1.
			lAI(k, j) /= lValue;
		}

		// Pour chaque rangée...
		for (size_t i=0; i<lAI.rows(); ++i) {
			if (i != k) { // ...différente de k
                // On soustrait la rangée k
                // multipliée par l'élément k de la rangée courante
				double lValue = lAI(i, k);
                lAI.getRowSlice(i) -= lAI.getRowCopy(k)*lValue;
			}
		}
	}

	// On copie la partie droite de la matrice AI ainsi transformée
	// dans la matrice courante (this).
	for (unsigned int i=0; i<iA.rows(); ++i) {
		iA.getRowSlice(i) = lAI.getDataArray()[slice(i*lAI.cols()+iA.cols(), iA.cols(), 1)];
	}
}

// Inverser la matrice par la méthode de Gauss-Jordan; implantation MPI parallèle.
void invertParallel(Matrix& iA) {
	//tuto nice : http://mpitutorial.com/tutorials/mpi-broadcast-and-collective-communication/
    //Verification M et carré et génération de la matrice identité
	//reduce pour trouver le max de chaques colonnes + check for singularité
	//swap ??
	//reduce pour la division
	//scatter les copies de rangés avec la multiplication et reduce pour la sum? nop
	// vérifier que la matrice est carrée
	assert(iA.rows() == iA.cols());
	// construire la matrice [A I]
	MatrixConcatCols lAI(iA, MatrixIdentity(iA.rows()));


	for (size_t k=0; k<iA.rows(); ++k) {
		// trouver l'index p du plus grand pivot de la colonne k en valeur absolue
		// (pour une meilleure stabilité numérique).
		size_t p = k;
		double lMax = fabs(lAI(k, k));
		for (size_t i = k; i < lAI.rows(); ++i) {
			if (fabs(lAI(i, k)) > lMax) {
				lMax = fabs(lAI(i, k));
				p = i;
			}
		}
		// vérifier que la matrice n'est pas singulière
		if (lAI(p, k) == 0) throw runtime_error("Matrix not invertible");
	}

}

// Multiplier deux matrices.
Matrix multiplyMatrix(const Matrix& iMat1, const Matrix& iMat2) {

    // vérifier la compatibilité des matrices
    assert(iMat1.cols() == iMat2.rows());
    // effectuer le produit matriciel
    Matrix lRes(iMat1.rows(), iMat2.cols());
    // traiter chaque rangée
    for(size_t i=0; i < lRes.rows(); ++i) {
        // traiter chaque colonne
        for(size_t j=0; j < lRes.cols(); ++j) {
            lRes(i,j) = (iMat1.getRowCopy(i)*iMat2.getColumnCopy(j)).sum();
        }
    }
    return lRes;
}

static int numprocs;

int main(int argc, char** argv) {

	srand((unsigned)time(NULL));
	double startTime, endTime;
	double * send_buffer = NULL;
	double * recv_buffer = NULL;
	Matrix* originalMatrix = NULL;
  Matrix* identity = NULL;
  Matrix* augmented = NULL;
	unsigned int lS = 3;

	if (argc == 2) {
		lS = atoi(argv[1]);
	}
	int my_rank;
	MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);

	if(my_rank == 0){
		originalMatrix = new MatrixRandom(lS, lS);
    identity = new MatrixIdentity(lS);
    augmented = new MatrixConcatCols(*originalMatrix,*identity);
    std::cout << "Original Matrix : " << std::endl << augmented->str() << std::endl;
	}

	int processColumnQty = lS/numprocs;
	Matrix dataMatrix(lS,processColumnQty);
	recv_buffer = new double[lS];
	send_buffer = new double[lS];

  for(int i = 0; i < lS; ++i){
		if(my_rank == 0){
			for(int k = 0; k < lS; ++k)
				send_buffer[k] = originalMatrix->getRowCopy(i)[k];
		}

		MPI_Scatter(send_buffer, processColumnQty, MPI_DOUBLE, recv_buffer, processColumnQty, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		for(int j = 0; j < processColumnQty; j++)
			dataMatrix(i,j) = recv_buffer[j];
	}

	// Begin timing.
	MPI_Barrier(MPI_COMM_WORLD);
	startTime = MPI_Wtime();

	int processingRow = 0;

	for(int i = 0; i < lS; ++i) {
		double swapIndex;
		if (my_rank == processingRow) {
			swapIndex = dataMatrix.getMaxRowIndex(0, processingRow);
      cout<<"get max row index "<<swapIndex<<"-processing "<<processingRow<<endl;
		}
		MPI_Bcast(&swapIndex, 1, MPI_DOUBLE, processingRow, MPI_COMM_WORLD);

    if(swapIndex != processingRow) {
			dataMatrix.swapRows(processingRow, swapIndex);
      cout<<"swap rows"<<swapIndex<<"-"<<processingRow<<endl;
		}

		//Calculate coefficient and send it
		if(my_rank == processingRow){
			for(int j = processingRow; j < lS; j++)
				send_buffer[j] = dataMatrix(0, j) / dataMatrix(0, processingRow);
		}
		MPI_Bcast(send_buffer, lS, MPI_DOUBLE, processingRow, MPI_COMM_WORLD);
		for(int j = 0; j < processColumnQty; j++) {
			for(int k = processingRow + 1; k < lS; k++) {
				dataMatrix(j, k) -= dataMatrix(j, processingRow) * send_buffer[k];
			}
		}

		processingRow++;
		MPI_Barrier(MPI_COMM_WORLD);
	}

	send_buffer = new double[processColumnQty];
	recv_buffer = new double[processColumnQty*numprocs];
	Matrix finaleMatrix(lS, lS);
	for(int i = 0; i < lS; ++i) {
		for (int j = 0; j < processColumnQty; ++j) {
			send_buffer[j] = dataMatrix(i,j);
		}
		MPI_Gather(send_buffer, processColumnQty, MPI_DOUBLE, recv_buffer, processColumnQty, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		if(my_rank == 0){
			for(int j = 0; j < finaleMatrix.cols(); ++j)
				finaleMatrix(i,j) = recv_buffer[j];
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	endTime = MPI_Wtime();
	if(my_rank == 0) {

		// std::cout << finaleMatrix.str() << std::endl;

    Matrix lRes = multiplyMatrix(*originalMatrix, finaleMatrix);
    cout << "Produit des deux matrices:\n" << lRes.str() << endl;

    // A fournir obligatoirement
    std::cout << "Taille de la matrice = " << lS << "x" << lS << std::endl;
    cout << "Erreur: " << lRes.getDataArray().sum() - lS << endl;
    std::cout << "Temps = " << endTime - startTime << std::endl;
	}
	MPI_Finalize ();




	return 0;
}
