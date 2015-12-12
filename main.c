#include <stdio.h>
#include <stdlib.h>

#define N 2

typedef char **PointerMatrixChar;
typedef char *PointerVectorChar;
typedef int **PointerMatrixInt;
typedef int *PointerVectorInt;
typedef struct {
    PointerMatrixChar board;
    int rows;
    int cols;
} CultivationBoard;
typedef CultivationBoard *PointerCultivationBoard;

PointerMatrixChar mallocForMatrixChar(int, int);
PointerVectorChar mallocForVectorChar(int);
void freeMatrixChar(PointerMatrixChar, int);
void freeVectorChar(PointerVectorChar);
void printMatrixChar(PointerMatrixChar, int, int);
PointerCultivationBoard readBoardFromFile(char *);

int main(int argc, char **argv)
{
    PointerMatrixChar matrix;
    matrix = mallocForMatrixChar(N, N);
    for (int j = 0; j < N; ++j) {
        for (int k = 0; k < N; ++k) {
            matrix[j][k] = (char) ('j' + k);
        }
    }
    printMatrixChar(matrix, N, N);
    freeMatrixChar(matrix, N);

    PointerCultivationBoard pcb = readBoardFromFile(argv[1]);
    printMatrixChar(pcb->board, pcb->rows, pcb->cols);
    free(pcb);

    return 0;
}

/**
 * Memory allocate for a PointerMatrixChar type.
 *
 * @param  rows Number of rows for allocate
 * @param  cols Number of colums for allocate
 * @return      Pointer whith allocated memory
 */
PointerMatrixChar mallocForMatrixChar(int rows, int cols)
{
    PointerMatrixChar temp;

    temp = (PointerMatrixChar) malloc(rows * sizeof(PointerVectorChar));
    for (int k = 0; k < rows; ++k) {
        temp[k] = mallocForVectorChar(cols);
    }

    return temp;
}

/**
 * Memory allocate for PointerVectorChar type.
 *
 * @param  size Number of element on vector
 * @return      Pointer with allocated memory
 */
PointerVectorChar mallocForVectorChar(int size)
{
    return (PointerVectorChar) malloc(size * sizeof(char));
}

/**
 * Free memory allocated for MatrixChar.
 *
 * @param m    Pointer to free
 * @param rows Number of rows in matrix
 */
void freeMatrixChar(PointerMatrixChar m, int rows)
{
    for (int k = 0; k < rows; ++k) {
        freeVectorChar(m[k]);
    }

    free(m);
}

/**
 * Free memory allocated for VectorChar.
 *
 * @param v Pointer to free
 */
void freeVectorChar(PointerVectorChar v)
{
    free(v);
}

/**
 * Print MatrixChar with printf.
 *
 * @param m    Pointer to matrix
 * @param rows Number of rows matrix
 * @param cols Number of columns of matrix
 */
void printMatrixChar(PointerMatrixChar m, int rows, int cols)
{
    for (int j = 0; j < rows; ++j) {
        for (int k = 0; k < cols; ++k) {
            printf("%c ", m[j][k]);
        }
        printf("\n");
    }
}

PointerCultivationBoard readBoardFromFile(char *fileName)
{
    FILE *fp;
    int rows, cols;
    PointerCultivationBoard pcb;
    char trash;

    pcb = (PointerCultivationBoard) malloc(sizeof(CultivationBoard));

    fp = fopen(fileName, "r");

    if (NULL == fp) {
        return NULL;
    }

    fscanf(fp, "%d %d%c", &(pcb->rows), &(pcb->cols), &trash);

    pcb->board = mallocForMatrixChar(pcb->rows, pcb->cols);

    for (int j = 0; j < pcb->rows; ++j) {
        for (int k = 0; k < pcb->cols; ++k) {
            fscanf(fp, "%c", &((pcb->board)[j][k]));
            fscanf(fp, "%c", &trash);
        }
    }

    fclose(fp);

    return pcb;
}
