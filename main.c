/**
 * Laboratory 3
 * 2015-02
 *
 * @author Jesús Urrutia
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define N 2
#define MENU_SIZE 4
#define MAX_CYCLES 5000000
#define MAX_CYCLES_WITHOUT_ACTIONS 12
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
typedef struct {
    char option;
    char description[255];
} MenuOption;

void printMenu(MenuOption *, int);
char readMenuOption(MenuOption *, int);
bool isValidOption(char, MenuOption *, int);
void writeCultivationBoardToFile(FILE *, PointerCultivationBoard);
void pause();
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

    if (argc < 2) {
        printf("Error, specific input file.");
        exit(1);
    }

    MenuOption menu[MENU_SIZE] = {
        { .option = 'n', .description = "Next cycle" },
        { .option = 'a', .description = "All cycles" },
        { .option = 'f', .description = "Cycles to file" },
        { .option = 'q', .description = "Quit" },
    };

    PointerCultivationBoard pcb = readBoardFromFile(argv[1]);
    PointerCultivationBoard pcbCopy = readBoardFromFile(argv[1]);

    int cycles = 0;
    char option;
    bool allCycles = false;
    FILE *fp;
    int actions = 0;
    int cyclesWithoutActions = 0;

    do {
        cycles++;

        actions = reproductionCycle(&pcb, &pcbCopy);

        cyclesWithoutActions = (0 == actions)
            ? cyclesWithoutActions + 1
            : 0;

        // printf("Actions: %d - Cycles without actions: %d\n", actions, cyclesWithoutActions);

        // if (cyclesWithoutActions > 0) {
        //     exit(1);
        // }

        if (NULL != fp) {
            writeCultivationBoardToFile(fp, pcb);
            continue;
        }

        printf("\n\nCycle: %d\n\n", cycles);
        printCultivationBoard(pcb);

        if (allCycles) {
            continue;
        }

        printMenu(menu, MENU_SIZE);
        option = readMenuOption(menu, MENU_SIZE);

        switch (option) {
            case 'a':
                allCycles = true;
                break;
            case 'q':
                printf("\n\nGood bye ...\n");
                exit(0);
                break;
            case 'f':
                fp = fopen("Salida.txt", "w");
                if (NULL == fp) {
                    printf("\nError, output file is not write.\n");
                }
                writeCultivationBoardToFile(fp, pcb);
                break;
        }
    } while ((actions > 0 || cyclesWithoutActions < MAX_CYCLES_WITHOUT_ACTIONS)
             && cycles < MAX_CYCLES);

    if (NULL != fp) {
        fclose(fp);
    } else {
        printf("Not more cycles.\n");
    }

    freePointerCultivationBoard(pcb);
    freePointerCultivationBoard(pcbCopy);

    return 0;
}

/**
 * Write current board to file.
 *
 * @param fp  File pointer to file for write board
 * @param pcb Pointer to CultivationBoard has write
 */
void writeCultivationBoardToFile(FILE *fp, PointerCultivationBoard pcb)
{
    for (int j = 0; j < pcb->rows; ++j) {
        for (int k = 0; k < pcb->cols; ++k) {
            fprintf(fp, "%c ", (pcb->board)[j][k].value);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n\n");
}

/**
 * Print menu on screen.
 *
 * @param menu     Options of menu
 * @param menuSize Number of items on menu
 */
void printMenu(MenuOption *menu, int menuSize)
{
    printf("\n\n");
    for (int k = 0; k < menuSize; ++k) {
        printf("%c) %s | ", menu[k].option, menu[k].description);
    }
    printf("\n\n");
}

/**
 * Read menu option and validate from stdin.
 *
 * @param  menu     Options of menu
 * @param  menuSize Number of items on menu
 * @return          Option read
 */
char readMenuOption(MenuOption *menu, int menuSize)
{
    char option, trash;
    bool isValid;

    do {
        printf("Enter a option: ");
        scanf("%c%c", &option, &trash);
        isValid = isValidOption(option, menu, menuSize);

        if (!isValid) {
            printf("\nOption: %c is no valid option. Retry ...\n", option);
        }
    } while (!isValid);

    return option;
}

/**
 * Check if option is valid.
 *
 * @param  option   Option to valid
 * @param  menu     Options of menu
 * @param  menuSize Number of options on menu
 * @return          True if option is valid
 */
bool isValidOption(char option, MenuOption *menu, int menuSize)
{
    for (int k = 0; k < menuSize; ++k) {
        if (tolower(option) == menu[k].option) {
            return true;
        }
    }

    return false;
}

/**
 * Wait for user press enter.
 */
void pause()
{
    printf("Press enter to continue ...");
    getchar();
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
 * Memory alloc for Matrix of CultivationBoard.
 *
 * @param  rows Number of rows for matrix
 * @param  cols Number of cols for matrix
 * @return      Pointer to memory allocated
 */
PointerMatrixCultivationBoardCell mallocForMatrixCultivationBoardCell(int rows, int cols)
{
    PointerMatrixCultivationBoardCell temp;

    temp = (PointerMatrixCultivationBoardCell) malloc(rows * sizeof(PointerVectorCultivationBoardCell));
    for (int k = 0; k < rows; ++k) {
        temp[k] = mallocForVectorCultivationBoardCell(cols);
    }

    return temp;
}

/**
 * Memory alloc for Vector of CultivationBoard.
 *
 * @param  size Size of vector
 * @return      Pointer to memory allocated
 */
PointerVectorCultivationBoardCell mallocForVectorCultivationBoardCell(int size)
{
    return (PointerVectorCultivationBoardCell) malloc(size * sizeof(CultivationBoardCell));
}

/**
 * Free memory allocated for matrix CultivationBoardCell.
 *
 * @param m    Pointer to matrix CultivationBoardCell
 * @param rows Number of rows of matrix CultivationBoardCell
 */
void freeMatrixCultivationBoardCell(PointerMatrixCultivationBoardCell m, int rows)
{
    for (int k = 0; k < rows; ++k) {
        freeVectorCultivationBoardCell(m[k]);
    }

    free(m);
}

/**
 * Free memory allocated for vector CultivationBoardCell.
 *
 * @param v Pointer to vector CultivationBoardCell
 */
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

/**
 * Print CultivationBoard.
 *
 * @param pcb Pointer to CultivationBoard
 */
void printCultivationBoard(PointerCultivationBoard pcb)
{
    for (int j = 0; j < pcb->rows; ++j) {
        for (int k = 0; k < pcb->cols; ++k) {
            printf("%c ", (pcb->board)[j][k].value);
        }
        printf("\n");
    }
}

/**
 * Read CultivationBoard from file.
 *
 * @param  fileName Path of file read
 * @return          Pointer to CultivationBoard with data read
 */
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

/**
 * Process reproduction cycle.
 *
 * Calculate state cell by cell and copy result in to auxiliar board
 * Change reference alternate with CultivationBoard and CultivationBoardCopy
 *
 * @param  pcbPointer     Pointer to PointerCultivationBoard original
 * @param  pcbCopyPointer Pointer to PointerCultivationBoard auxiliar
 * @return                Number of actions ocurring in current cycle
 */
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
            // printf(
            //     "Value: %c - Cycles: %d - Row: %d - Col: %d - ",
            //     (pcb->board)[j][k].value,
            //     (pcb->board)[j][k].cycles,
            //     j,
            //     k
            // );
            // printf(
            //     "Empty: %d - Non Empty: %d - W: %d - X: %d - Y: %d - Z: %d\n",
            //     context.empty,
            //     context.nonEmpty,
            //     context.w,
            //     context.x,
            //     context.y,
            //     context.z
            // );

            switch ((pcb->board)[j][k].value) {
                case CELL_W:
                case CELL_X:
                case CELL_Y:
                    if (checkDeath((pcb->board)[j][k], context)) {
                        (pcbCopy->board)[j][k].value = CELL_EMPTY;
                        (pcbCopy->board)[j][k].cycles = 0;
                        actions++;
                    } else {
                        (pcbCopy->board)[j][k] = (pcb->board)[j][k];
                    }
                    break;
                case CELL_EMPTY:
                    (pcbCopy->board)[j][k].value = checkCellIsBorn(context);
                    (pcbCopy->board)[j][k].cycles = 0;

                    if (CELL_EMPTY != (pcbCopy->board)[j][k].value) {
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

/**
 * Calculate context of one CultivationBoardCell.
 *
 * @param  pcb Pointer to CultivationBoardCell
 * @param  row Row coordinate to evaluate context
 * @param  col Col coordinate to evaluate context
 * @return     Context of CultivationBoardCell
 */
CellContext getCellContext(PointerCultivationBoard pcb, int row, int col)
{
    CellContext context = {
        .empty = 0,
        .nonEmpty = 0,
        .w = 0,
        .x = 0,
        .y = 0,
        .z = 0
    };

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
                    break;
            }
        }
    }

    return context;
}

/**
 * Check of cell is death.
 *
 * @param  cell    CultivationBoardCell to evaluate
 * @param  context Context of CultivationBoardCell to evaluate
 * @return         True if cell is death
 */
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

/**
 * Check of cell type W is death.
 *
 * @param  cell    CultivationBoardCell to evaluate
 * @param  context Context of CultivationBoardCell to evaluate
 * @return         True if cell is death
 */
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

/**
 * Check of cell type X is death.
 *
 * @param  cell    CultivationBoardCell to evaluate
 * @param  context Context of CultivationBoardCell to evaluate
 * @return         True if cell is death
 */
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

/**
 * Check of cell type Y is death.
 *
 * @param  cell    CultivationBoardCell to evaluate
 * @param  context Context of CultivationBoardCell to evaluate
 * @return         True if cell is death
 */
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

/**
 * Check if cell is born.
 *
 * @param  context Context for cell to evaluate
 * @return         Value of cell born
 */
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

/**
 * Check if cell W is born.
 *
 * @param  context Context of cell to evaluate
 * @return         True if cell W is born
 */
bool checkCellWIsBorn(CellContext context)
{
    if (2 == context.w || 3 == context.w) {
        return true;
    }

    return false;
}

/**
 * Check if cell X is born.
 *
 * @param  context Context of cell to evaluate
 * @return         True if cell X is born
 */
bool checkCellXIsBorn(CellContext context)
{
    if (1 == context.x && 1 == context.y) {
        return true;
    }

    return false;
}

/**
 * Check if cell Y is born.
 *
 * @param  context Context of cell to evaluate
 * @return         True if cell Y is born
 */
bool checkCellYIsBorn(CellContext context)
{
    if (2 == context.y || 3 == context.y) {
        return true;
    }

    return false;
}
