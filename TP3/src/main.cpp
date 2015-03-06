#include <iostream>
#include <cmath>
#include <mpi.h>
#include <fstream>
#include <ctime>
#include <vector>
#include "Matrix.hpp"

using namespace std;

// Sorts the input row into chunks to be scattered two all the processors.
void sortByProcess(vector<double> list1, double* list2, int count,int size);


int main(int argc, char * argv[])
{

  int rank, size;
  double sTime, eTime, rTime;
  ifstream inFile;
  size_t num_rows;
  size_t num_cols;
  int cur_control = 0;
  double * send_buffer = NULL;
  double * recv_buffer = NULL;
  double ** data = NULL;
  double determinant;
  vector<double> file_buffer;
  //MatrixRandom lA=NULL;
  unsigned int lS = 5;

  // Just get the initialization of the program going.
  MPI_Init(&argc, &argv);
  int trank=MPI::COMM_WORLD::Get_rank();
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Verify program usage.
  if(argc < 2)
  {
    cout << "Usage of program invalid" << endl;
    MPI_Finalize();
    return 0;
  }

  // If the root node create matrix
  //if(!rank)
  //{
  MatrixRandom lA(lS, lS);
  cout<<"Matrix created"<<endl;
  num_rows=5;//size_t(lA.rows);
  //}

  send_buffer = new double[num_rows];
  // Broadcasts the number of rows to each worker.
  MPI_Bcast (&num_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
  cout<<"Broadcasting nb of rows:"<<num_rows<<endl;
  // Square matrix
  num_cols = num_rows;

  // Allocate the memory on each worker.
  cout<<"Allocating memory to worker"<<endl;
  data = new double*[num_cols];
  for(int i = 0; i < num_cols; i++)
    data[i] = new double[num_rows];
    for(int i = 0; i < num_cols; i++)
    {
      for(int j = 0; j < num_rows; j++)
        data[i][j] = 0;
      }
      recv_buffer = new double[num_cols];
      // Scatter the data.
      for(int i = 0; i < num_rows; i++)
      {
        if(!rank)
        {
          for(int j = 0; j < num_rows; j++){
            inFile >> file_buffer[j];
          }
          //sortByProcess(file_buffer, send_buffer, num_rows,size);
          cout<<"Sorting by process"<<endl;
        }
        // Scatters the data so that each process gets the next value for their columns.
        MPI_Scatter(send_buffer, num_cols, MPI_DOUBLE, recv_buffer, num_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        cout<<"Scattering matrix"<<endl;
        for(int j = 0; j < num_cols; j++)
        {
          data[j][i] = recv_buffer[j];
        }
      }
      delete [] recv_buffer;
      delete [] send_buffer;
      // Begin timing.
      MPI_Barrier(MPI_COMM_WORLD);
      sTime = MPI_Wtime();

      // Actual Gaussian code here.
      /*Algorithm for Gaussian elimination (with pivoting):
      Start with all the numbers stored in our NxN matrix A.
      For each column p, we do the following (p=1..N)
      First make sure that a(p,p) is non-zero and preferably large:
      Look at the rows in our matrix below row p.  Look at the p'th
      term in each row.  Select the row that has the largest absolute
      value in the p'th term, and swap the p'th row with that one.
      (optionally, you can only bother to do the above step if
      a(p,p) is zero).
      If we were fortunate enough to get a non-zero value for a(p,p),
      then proceed with the following for loop:
      For each row r below p, we do the following (r=p+1..N)
      row(r) = row(r)  -  (a(r,p) / a(p,p)) * row(p)
      End For
      End For
      */
      send_buffer = new double[num_rows];
      int cur_row = 0;
      int swaps = 0;
      double det_val = 1;
      int cur_index = 0;
      for(int i = 0; i < num_rows; i++)
      {
        cout << "Find which row to swap with: " << cur_row << endl;
        int rowSwap;
        if(cur_control == rank)
        {
          rowSwap = cur_row;
          double max = data[cur_index][cur_row];
          // Find the row to swap with.
          for(int j = cur_row + 1; j < num_rows; j++)
          {
            if(data[cur_index][j] > max)
            {
              rowSwap = j;
              max = data[cur_index][j];
            }
          }
        }
        cout << "Swapping: " << cur_row << "with " << rowSwap << endl;

        // Find out if you need to swap and then act accordingly.

        cout << "Broadcasting and check if swap is needed " << cur_row << endl;
        MPI_Bcast(&rowSwap, 1, MPI_INT, cur_control, MPI_COMM_WORLD);
        if(rowSwap != cur_row)
        {
          cout << "Must swap: " << cur_row << "with " << rowSwap << endl;
          lA.swapRows(cur_row,rowSwap);
          //swap(data, num_cols, cur_row, rowSwap);
          swaps++;
        }

        if(cur_control == rank)
        {
          // Generate the coefficients.
          for(int j = cur_row; j < num_rows; j++)
            send_buffer[j] = data[cur_index][j] / data[cur_index][cur_row];
          }
          // Send and recv the coefficients.
          MPI_Bcast(send_buffer, num_rows, MPI_DOUBLE, cur_control, MPI_COMM_WORLD);
          // Apply the coefficients to the data.
          for(int j = 0; j < num_cols; j++)
          {
            for(int k = cur_row + 1; k < num_rows; k++)
            {
              data[j][k] -= data[j][cur_row] * send_buffer[k];
            }
          }

          // Update the determinant value.
          if(cur_control == rank)
          {
            det_val = det_val * data[cur_index][cur_row];
            cur_index++;
          }

          // Increment the row that we are looking at
          // and increment the counter that tells each process where
          // to recv from. The counter resets to zero to give us a
          // "round robin" communication pattern. Probably not very efficient,
          // but it will do for now.
          cur_control++;
          if(cur_control == size)
            cur_control = 0;
            cur_row++;
          }

          // Reduce all the determinant values from each process
          // with a multiplication operation.
          // Personally I really like the method I used to find the determinant:
          //   1. Each process just keeps multiplying the pivot value into the product.
          //   2. The reduce does a multiply on all of the individual products.
          // So there really is no extra work to find the determinant.
          MPI_Reduce(&det_val, &determinant, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
          // If we did an odd number of row swaps, negate the determinant.
          if(swaps % 2)
            determinant = -determinant;

            // End timing.
            MPI_Barrier(MPI_COMM_WORLD);
            eTime = MPI_Wtime();
            rTime = eTime - sTime;

            // If root node, output the runtime.
            if(!rank)
            {
              cout << "Run Time: " << rTime << endl;
              cout << "Determinant value: " << determinant << endl;
            }

            // A bit of house cleaning.
            delete [] send_buffer;
            for(int i = 0; i < num_cols; i++)
              delete [] data[i];
              delete [] data;

              // Finalize and exit.
              MPI_Finalize();
              return 0;
            }

            void sortByProcess(vector<double> list2, double* list1, int count,int size)
          {
            int index = 0;
            for(int i = 0; i < size; i++)
            {
              for(int j = i; j < count; j += size)
              {
                list1[index] = list2[j];
                index++;
              }
            }
            return;
          }
