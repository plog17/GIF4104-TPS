//
//  Matrix.cpp
//

#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include <string>
#include <valarray>
#include <cassert>
#include <iostream>

// La classe Matrix est dérivée de std::valarray. Cette dernière est
// similaire à std::vector, sauf qu'elle alloue exactement la quantité
// de mémoire nécessaire (au lieu de 2^n dans std::vector) et qu'elle
// contient des fonctions pour accéder facilement à des régions (splice)
// du vecteur.
class Matrix  {

public:
    // Construire matrice iRows x iCols et initialiser avec des 0.
    Matrix() {}

    // Construire matrice iRows x iCols et initialiser avec des 0.
    Matrix(std::size_t iRows, std::size_t iCols) : mData(0., iRows*iCols), mRows(iRows), mCols(iCols) {}

    // Affecter une matrice de même taille; s'assurer que les tailles sont identiques.
    Matrix& operator=(const Matrix& iMat) {
        assert(mRows == iMat.mRows && mCols == iMat.mCols);
        mData = iMat.mData;
        return *this;
    }

    // Accéder à la case (i, j) en lecture/écriture.
    inline double& operator()(std::size_t iRow, std::size_t iCol) {
        return mData[(iRow*mCols)+iCol];
    }

    // Accéder à la case (i, j) en lecture seulement.
    inline const double& operator()(size_t iRow, size_t iCol) const {
        return mData[(iRow*mCols)+iCol];
    }

    // Retourner le nombre de colonnes.
    inline std::size_t cols(void) const { return mCols; }

    // Retourner le nombre de lignes.
    inline std::size_t rows(void) const { return mRows; }

    // Retourner le tableau d'une colonne de la matrice.
    std::valarray<double> getColumnCopy(size_t iCol) const {
        assert(iCol < mCols);
        return mData[std::slice(iCol, mRows, mCols)];
    }

    // Retourner la slice d'une colonne de la matrice.
    std::slice_array<double> getColumnSlice(size_t iCol) {
        assert(iCol < mCols);
        return mData[std::slice(iCol, mRows, mCols)];
    }

    // Retourner la slice d'une colonne de la matrice.
    const std::slice_array<double> getColumnSlice(size_t iCol) const {
        assert(iCol < mCols);
        return const_cast<Matrix*>(this)->mData[std::slice(iCol, mRows, mCols)];
    }

    // Retourner le tableau d'une rangée de la matrice.
    std::valarray<double> getRowCopy(size_t iRow) const {
        assert(iRow < mRows);
        return mData[std::slice(iRow*mCols, mCols, 1)];
    }

    // Retourner la slice d'une rangée de la matrice.
    std::slice_array<double> getRowSlice(size_t iRow) {
        assert(iRow < mRows);
        return mData[std::slice(iRow*mCols, mCols, 1)];
    }

    // Retourner la slice d'une rangée de la matrice.
    const std::slice_array<double> getRowSlice(size_t iRow) const {
        assert(iRow < mRows);
        return const_cast<Matrix*>(this)->mData[std::slice(iRow*mCols, mCols, 1)];
    }

    int getMaxRowIndex(int ColumnIndex, int rowOffset=0);

    // Accéder au tableau interne de la matrice en lecture/écriture.
    std::valarray<double>& getDataArray(void) { return mData; }

    // Accéder au tableau interne de la matrice en lecture seulement.
    const std::valarray<double>& getDataArray(void) const { return mData; }

    // Permuter deux rangées de la matrice.
    Matrix& swapRows(size_t iR1, size_t iR2);

    // Permuter deux colonnes de la matrice.
    Matrix& swapColumns(size_t iC1, size_t iC2);

    // Représenter la matrice sous la forme d'une chaîne de caractères.
    // Pratique pour le débuggage...
    std::string str(void) const;

protected:
    // Nombre de rangées et de colonnes.
    std::size_t mRows, mCols;
    std::valarray<double> mData;

};

// Construire une matrice identité.
class MatrixIdentity : public Matrix {
public:
    MatrixIdentity(size_t iSize);
};

// Construire une matrice aléatoire [0,1) iRows x iCols.
// Utiliser srand pour initialiser le générateur de nombres.
class MatrixRandom : public Matrix {
public:
    MatrixRandom(size_t iRows, size_t iCols);
};

// Construire une matrice en concaténant les colonnes de deux matrices de même hauteur.
class MatrixConcatCols : public Matrix {
public:
    MatrixConcatCols(const Matrix& iMat1, const Matrix& iMat2);
};

// Construire une matrice en concaténant les rangées de deux matrices de même largeur.
class MatrixConcatRows : public Matrix {
public:
    MatrixConcatRows(const Matrix& iMat1, const Matrix& iMat2);
};

// Insérer une matrice dans un flot de sortie.
std::ostream& operator<<(std::ostream& oStream, const Matrix& iMat);

#endif
