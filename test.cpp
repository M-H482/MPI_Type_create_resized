#include "mpi.h"
#include <iostream>
using namespace std;

void printMat(int n, int* mat)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%02d ", *mat++);
        }
        printf("\n");
    }
    printf("\n");
}

void init(int rank, int n, int* mat)
{
    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                *mat++ = i * 10 + j;
            }
        }
    } else if (rank == 1) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                *mat++ = -1;
            }
        }
    }
}
int main(int argc, char** argv)
{
    int rank, nprcs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprcs);

    int n = 9;
    int mat[n][n];

    init(rank, n, &mat[0][0]);

    int count = 2, blocklength = 3, stride = n;
    MPI_Datatype block_t, new_type;
    MPI_Aint exetent;

    MPI_Type_vector(2, 3, n, MPI_INT, &block_t);
    MPI_Type_commit(&block_t);

    MPI_Type_extent(block_t, &exetent);
    if (rank == 0) {
        cout << "before MPI_Type_create_resized, exetent of block_t is " << exetent << endl;
    }

    MPI_Type_create_resized(block_t, 0, (2 * n + 3) * sizeof(int), &new_type);
    MPI_Type_commit(&new_type);

    MPI_Type_extent(block_t, &exetent);
    if (rank == 0) {
        cout << "after MPI_Type_create_resized, exetent of new_type is " << exetent << endl;
    }

    if (rank == 0) {
        cout << "rank 0 init mat:" << endl;

        MPI_Send(mat, 3, new_type, 1, 777, MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Status sta;
        MPI_Recv(mat, 3, new_type, 0, 777, MPI_COMM_WORLD, &sta);

        cout << "rank 1 recv mat: (-1 is initial value)" << endl;
        printMat(n, &mat[0][0]);
    }

    MPI_Finalize();
    return 0;
}