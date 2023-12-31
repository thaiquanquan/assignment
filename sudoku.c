#include <stdio.h>
#include <stdlib.h>
#define BOARD_SIZE 9

typedef struct Cell_impl {
    int row_index;
    int col_index;
    int box_index;
    int num_candidates;
    int candidates[BOARD_SIZE];
    int value; // solved or 0
} Cell;

typedef struct SudokuBoard_impl {
    int solved_counter;
    Cell **data; // 9x9 cell board

    Cell **p_rows[BOARD_SIZE];  // rows pointers
    Cell **p_cols[BOARD_SIZE];  // cols pointers
    Cell **p_boxes[BOARD_SIZE]; // boxes pointers
    Cell *solved_cells[BOARD_SIZE *
                       BOARD_SIZE]; // solved cell pointers (maximum)
} SudokuBoard;

void set_candidate(Cell *cell, int value) {
    cell->candidates[value - 1] = 1;
    cell->num_candidates += 1;
}

void unset_candidate(Cell *cell, int value) {
    cell->candidates[value - 1] = 0;
    cell->num_candidates -= 1;
}

void hidden_singles(SudokuBoard *board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Cell *cell = &(board->data[i][j]);
            if (cell->value == 0) { // If the cell is not solved
                for (int value = 1; value <= BOARD_SIZE; value++) {
                    if (cell->candidates[value - 1] == 1) { // If the value is a candidate
                        int count_in_row = 0, count_in_col = 0, count_in_box = 0;
                        // Check the row
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            if (board->p_rows[i][k]->candidates[value - 1] == 1) {
                                count_in_row++;
                            }
                        }
                        // Check the column
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            if (board->p_cols[j][k]->candidates[value - 1] == 1) {
                                count_in_col++;
                            }
                        }
                        // Check the box
                        for (int k = 0; k < BOARD_SIZE; k++) {
                            if (board->p_boxes[cell->box_index][k]->candidates[value - 1] == 1) {
                                count_in_box++;
                            }
                        }
                        // If the value is unique in the row, column, or box, set the cell to that value
                        if (count_in_row == 1 || count_in_col == 1 || count_in_box == 1) {
                            cell->value = value;
                            unset_candidate(cell, value);
                            board->solved_counter++;
                        }
                    }
                }
            }
        }
    }
}

void naked_pair_triple(SudokuBoard *board) {
    // Your code here
    // 1. Loop through each cell in the board.
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Cell *cell = &(board->data[i][j]);
            // 2. If a cell has exactly 2 or 3 candidates, it might be part of a naked pair or triple.
            if (cell->num_candidates == 2 || cell->num_candidates == 3) {
                // 3. Look for another cell in the same row, column, or box that has the same candidates.
                for (int k = 0; k < BOARD_SIZE; k++) {
                    Cell *other_cell = board->p_rows[i][k];
                    if (other_cell != cell && other_cell->num_candidates == cell->num_candidates) {
                        int is_same = 1;
                        for (int l = 0; l < BOARD_SIZE; l++) {
                            if (cell->candidates[l] != other_cell->candidates[l]) {
                                is_same = 0;
                                break;
                            }
                        }
                    
                        if (is_same) {
                            // 5. Remove these candidates from all other cells in the same row, column, or box.
                            for (int l = 0; l < BOARD_SIZE; l++) {
                                 Cell *remove_cell = board->p_rows[i][l];
                                if (remove_cell != cell && remove_cell != other_cell) {
                                    for (int m = 0; m < BOARD_SIZE; m++) {
                                        if (cell->candidates[m]) {
                                            unset_candidate(remove_cell, m + 1);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void hidden_pair_triple(SudokuBoard *board) {
    // Your code here
    // 1. Loop through each cell in the board.
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Cell *cell = &(board->data[i][j]);
            // 2. For each cell, look at the candidates of all other cells in the same row, column, or box.
            for (int value = 1; value <= BOARD_SIZE; value++) {
                if (cell->candidates[value - 1] == 1) { // If the value is a candidate
                    int count_in_row = 0, count_in_col = 0, count_in_box = 0;
                    Cell *cells_in_row[BOARD_SIZE], *cells_in_col[BOARD_SIZE], *cells_in_box[BOARD_SIZE];
                    // Check the row
                    for (int k = 0; k < BOARD_SIZE; k++) {
                        if (board->p_rows[i][k]->candidates[value - 1] == 1) {
                            cells_in_row[count_in_row++] = board->p_rows[i][k];
                        }
                    }
                    // Check the column
                    for (int k = 0; k < BOARD_SIZE; k++) {
                        if (board->p_cols[j][k]->candidates[value - 1] == 1) {
                            cells_in_col[count_in_col++] = board->p_cols[j][k];
                        }
                    }
                    // Check the box
                    for (int k = 0; k < BOARD_SIZE; k++) {
                        if (board->p_boxes[cell->box_index][k]->candidates[value - 1] == 1) {
                            cells_in_box[count_in_box++] = board->p_boxes[cell->box_index][k];
                        }
                    }
                    // 3. If you find a candidate that only appears in 2 or 3 cells, you might have a hidden pair or triple.
                    if (count_in_row == 2 || count_in_row == 3) {
                        // 4. If these 2 or 3 cells don't have any other candidates, you have a hidden pair or triple.
                        int is_hidden = 1;
                        for (int k = 0; k < count_in_row; k++) {
                            if (cells_in_row[k]->num_candidates != count_in_row) {
                                is_hidden = 0;
                                break;
                            }
                        }
                        // 5. Remove all other candidates from these cells.
                        if (is_hidden) {
                            for (int k = 0; k < count_in_row; k++) {
                                for (int l = 0; l < BOARD_SIZE; l++) {
                                    if (!cell->candidates[l]) {
                                        unset_candidate(cells_in_row[k], l + 1);
                                    }
                                }
                            }
                        }
                    }
                    // Repeat the same process for the column and the box
                }
            }
        }
    }
}

void print_board(SudokuBoard *board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%d ", board->data[i][j].value);
        }
        printf("\n");
    }
}
SudokuBoard* initialize_board(int input_board[BOARD_SIZE][BOARD_SIZE]) {
    SudokuBoard *board = malloc(sizeof(SudokuBoard));
    board->solved_counter = 0;
    board->data = malloc(BOARD_SIZE * sizeof(Cell*));

    for (int i = 0; i < BOARD_SIZE; i++) {
        board->data[i] = malloc(BOARD_SIZE * sizeof(Cell));
        for (int j = 0; j < BOARD_SIZE; j++) {
            Cell cell;
            cell.row_index = i;
            cell.col_index = j;
            cell.box_index = (i / 3) * 3 + j / 3;
            cell.num_candidates = 0;
            cell.value = input_board[i][j]; // Use the input data here
            for (int k = 0; k < BOARD_SIZE; k++) {
                cell.candidates[k] = 1; // Assuming all values are candidates
            }
            board->data[i][j] = cell;
        }
    }

    // Initialize the row, column, and box pointers
    for (int i = 0; i < BOARD_SIZE; i++) {
        board->p_rows[i] = malloc(BOARD_SIZE * sizeof(Cell*));
        board->p_cols[i] = malloc(BOARD_SIZE * sizeof(Cell*));
        board->p_boxes[i] = malloc(BOARD_SIZE * sizeof(Cell*));
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->p_rows[i][j] = &(board->data[i][j]);
            board->p_cols[i][j] = &(board->data[j][i]);
            board->p_boxes[i][j] = &(board->data[(i/3)*3 + j/3][i*3%9 + j%3]);
        }
    }

    return board;
}

void free_board(SudokuBoard *board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        free(board->data[i]);
    }
    free(board->data);
    free(board);
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != BOARD_SIZE * BOARD_SIZE + 1) {
        printf("Usage: %s <81 integers>\n", argv[0]);
        return 1;
    }

    // Your input Sudoku board
    int input_board[BOARD_SIZE][BOARD_SIZE];

    // Convert the command line arguments to integers and store them in the input_board
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            input_board[i][j] = atoi(argv[i * BOARD_SIZE + j + 1]);
        }
    }

    // Initialize your Sudoku board and cells here
    SudokuBoard *board = initialize_board(input_board);

    // Call your algorithms here
    hidden_singles(board);
    naked_pair_triple(board);
    hidden_pair_triple(board);

    // Print the solved board
    print_board(board);

    // Free the memory allocated for the board
    free_board(board);

    return 0;
}
