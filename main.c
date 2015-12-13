#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 2
#define CELL_EMPTY '0'
#define CELL_W 'W'
#define CELL_X 'X'
#define CELL_Y 'Y'
#define CELL_Z 'Z'

typedef char **PointerMatrixChar;
typedef char *PointerVectorChar;
typedef int **PointerMatrixInt;
typedef int *PointerVectorInt;
typedef struct {
    char value;
    int cycles;
} CultivationBoardCell;
typedef CultivationBoardCell *PointerVectorCultivationBoardCell;
typedef CultivationBoardCell **PointerMatrixCultivationBoardCell;
typedef struct {
    PointerMatrixCultivationBoardCell board;
    int rows;
    int cols;
} CultivationBoard;
typedef CultivationBoard *PointerCultivationBoard;
typedef struct {
    int empty;
    int nonEmpty;
    int w;
    int x;
    int y;
    int z;
} CellContext;

PointerMatrixChar mallocForMatrixChar(int, int);
PointerVectorChar mallocForVectorChar(int);
void freeMatrixChar(PointerMatrixChar, int);
void freeVectorChar(PointerVectorChar);
void printMatrixChar(PointerMatrixChar, int, int);
PointerCultivationBoard readBoardFromFile(char *);
void freePointerCultivationBoard(PointerCultivationBoard);
int reproductionCycle(PointerCultivationBoard *, PointerCultivationBoard *);
PointerMatrixCultivationBoardCell mallocForMatrixCultivationBoardCell(int, int);
PointerVectorCultivationBoardCell mallocForVectorCultivationBoardCell(int);
void freeMatrixCultivationBoardCell(PointerMatrixCultivationBoardCell, int);
void freeVectorCultivationBoardCell(PointerVectorCultivationBoardCell);
void printCultivationBoard(PointerCultivationBoard);
CellContext getCellContext(PointerCultivationBoard, int, int);
bool checkDeath(CultivationBoardCell, CellContext);
bool checkDeathW(CultivationBoardCell, CellContext);
bool checkDeathX(CultivationBoardCell, CellContext);
bool checkDeathY(CultivationBoardCell, CellContext);
char checkCellIsBorn(CellContext);
bool checkCellWIsBorn(CellContext);
bool checkCellXIsBorn(CellContext);
bool checkCellYIsBorn(CellContext);

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
    PointerCultivationBoard pcbCopy = readBoardFromFile(argv[1]);

    int cycles = 0;
    do {
        cycles++;
        printf("\n\nCycle: %d\n\n", cycles);
        printCultivationBoard(pcb);
    } while (reproductionCycle(&pcb, &pcbCopy) > 0 && cycles < 100);

    freePointerCultivationBoard(pcb);
    freePointerCultivationBoard(pcbCopy);

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

PointerMatrixCultivationBoardCell mallocForMatrixCultivationBoardCell(int rows, int cols)
{
    PointerMatrixCultivationBoardCell temp;

    temp = (PointerMatrixCultivationBoardCell) malloc(rows * sizeof(PointerVectorCultivationBoardCell));
    for (int k = 0; k < rows; ++k) {
        temp[k] = mallocForVectorCultivationBoardCell(cols);
    }

    return temp;
}

PointerVectorCultivationBoardCell mallocForVectorCultivationBoardCell(int size)
{
    return (PointerVectorCultivationBoardCell) malloc(size * sizeof(CultivationBoardCell));
}

void freeMatrixCultivationBoardCell(PointerMatrixCultivationBoardCell m, int rows)
{
    for (int k = 0; k < rows; ++k) {
        freeVectorCultivationBoardCell(m[k]);
    }

    free(m);
}

void freeVectorCultivationBoardCell(PointerVectorCultivationBoardCell v)
{
    free(v);
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

void printCultivationBoard(PointerCultivationBoard pcb)
{
    for (int j = 0; j < pcb->rows; ++j) {
        for (int k = 0; k < pcb->cols; ++k) {
            printf("%c ", (pcb->board)[j][k].value);
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

    pcb->board = mallocForMatrixCultivationBoardCell(pcb->rows, pcb->cols);

    for (int j = 0; j < pcb->rows; ++j) {
        for (int k = 0; k < pcb->cols; ++k) {
            fscanf(fp, "%c", &((pcb->board)[j][k].value));
            fscanf(fp, "%c", &trash);
            (pcb->board)[j][k].cycles = 0;
        }
    }

    fclose(fp);

    return pcb;
}

/**
 * Free pointer to CultivationBoard.
 *
 * @param pcb Pointer to CultivationBoard
 */
void freePointerCultivationBoard(PointerCultivationBoard pcb)
{
    free(pcb->board);
    free(pcb);
}

int reproductionCycle(PointerCultivationBoard *pcbPointer, PointerCultivationBoard *pcbCopyPointer)
{
    PointerCultivationBoard pcb, pcbCopy;
    CellContext context;
    int actions = 0;

    pcb = *pcbPointer;
    pcbCopy = *pcbCopyPointer;

    for (int j = 0; j < pcb->rows; ++j) {
        for (int k = 0; k < pcb->cols; ++k) {
            context = getCellContext(pcb, j, k);

            switch ((pcb->board)[j][k].value) {
                case CELL_W:
                case CELL_X:
                case CELL_Y:
                    if (checkDeath((pcb->board)[j][k], context)) {
                        (pcbCopy->board)[j][k].value = CELL_EMPTY;
                        (pcbCopy->board)[j][k].cycles = 0;
                        actions++;
                    }
                    break;
                case CELL_EMPTY:
                    (pcbCopy->board)[j][k].value = checkCellIsBorn(context);
                    (pcbCopy->board)[j][k].cycles = 0;

                    if (CELL_EMPTY != (pcb->board)[j][k].value) {
                        actions++;
                    }
                    break;
                default:
                    (pcbCopy->board)[j][k] = (pcb->board)[j][k];
            }

            (pcbCopy->board)[j][k].cycles++;
        }
    }

    *pcbPointer = pcbCopy;
    *pcbCopyPointer = pcb;

    return actions;
}

CellContext getCellContext(PointerCultivationBoard pcb, int row, int col)
{
    CellContext context;

    for (int j = row - 1; j <= row + 1; ++j) {
        for (int k = col - 1; k <= col + 1; ++k) {
            if (j < 0 || j >= pcb->rows) { break; }
            if (k < 0 || k >= pcb->cols) { continue; }
            if (j == row && k == col) { continue; }

            switch((pcb->board)[j][k].value) {
                case CELL_EMPTY:
                    context.empty++;
                    break;
                case CELL_W:
                    context.w++;
                    context.nonEmpty++;
                    break;
                case CELL_X:
                    context.x++;
                    context.nonEmpty++;
                    break;
                case CELL_Y:
                    context.y++;
                    context.nonEmpty++;
                    break;
                case CELL_Z:
                    context.z++;
                    context.nonEmpty++;
                    break;
            }
        }
    }

    return context;
}

bool checkDeath(CultivationBoardCell cell, CellContext context)
{
    switch (cell.value) {
        case CELL_W:
            return checkDeathW(cell, context);
        case CELL_X:
            return checkDeathX(cell, context);
        case CELL_Y:
            return checkDeathY(cell, context);
        default:
            return false;
    }
}

bool checkDeathW(CultivationBoardCell cell, CellContext context)
{
    if (cell.cycles >= 10) {
        return true;
    }

    if (context.nonEmpty < 2) {
        return true;
    }

    return false;
}

bool checkDeathX(CultivationBoardCell cell, CellContext context)
{
    if (cell.cycles >= 7) {
        return true;
    }

    if (context.x < 2) {
        return true;
    }

    return false;
}

bool checkDeathY(CultivationBoardCell cell, CellContext context)
{
    if (cell.cycles >= 5) {
        return true;
    }

    if (context.nonEmpty > 4) {
        return true;
    }

    return false;
}

char checkCellIsBorn(CellContext context)
{
    if (checkCellWIsBorn(context)) {
        return CELL_W;
    } else if (checkCellXIsBorn(context)) {
        return CELL_X;
    } else if(checkCellYIsBorn(context)) {
        return CELL_Y;
    } else {
        return CELL_EMPTY;
    }
}

bool checkCellWIsBorn(CellContext context)
{
    return false;
}

bool checkCellXIsBorn(CellContext context)
{
    return false;
}

bool checkCellYIsBorn(CellContext context)
{
    return false;
}
