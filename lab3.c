#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** board = NULL;

    // replace this comment with your code
    fp = fopen(filename, "r");
    board = (int**)malloc(sizeof(int*) * ROW_SIZE);

    for(int row = 0; row < ROW_SIZE; row++){
        board[row] = (int*)malloc(sizeof(int) * COL_SIZE);
    }

    for(int row=0; row < ROW_SIZE; row++){
        for(int colmn = 0; colmn < COL_SIZE; colmn++){
            fscanf(fp, "%d,", &board[row][colmn]);
        }
    }
    fclose(fp);

    return board;
}


void* validate(void* arg){
    
    param_struct* param = (param_struct*) arg;
    int id = param->id;
    int startrow = param->starting_row;
    int endrow = param->ending_row;
    int startcol = param->starting_col;
    int endcol = param->ending_col;
    int arr[9] = {0,0,0,0,0,0,0,0,0};
    int current;

    worker_validation[id] = 1;
    
    for(int rr = startrow; rr <= endrow; rr++){
        for(int cc = startcol; cc <= endcol; cc++){
            current = sudoku_board[rr][cc];
            arr[current - 1] = 1;
        }
    }
    
    for(int i = 0; i < 9; i++)
        if(arr[i] != 1)
        worker_validation[id] = 0;
    
    
}

int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    
    // replace this comment with your code
    pthread_attr_init(&attr);

    tid = (pthread_t*)malloc(sizeof(pthread_t)*NUM_OF_THREADS);
    parameter = (param_struct*)malloc(sizeof(param_struct)*NUM_OF_THREADS);
    worker_validation = (int*)malloc(sizeof(int)*NUM_OF_THREADS);

    int thread_id = 0;
    
    for (int i = 0; i < ROW_SIZE; i++) {
        parameter[thread_id].id = thread_id;
        parameter[thread_id].starting_row = i;
        parameter[thread_id].starting_col = 0;
        parameter[thread_id].ending_row = i;
        parameter[thread_id].ending_col = COL_SIZE - 1;
        pthread_create(&(tid[thread_id]), NULL, validate, &(parameter[thread_id]));
        thread_id++;
    }

    
    for (int i = 0; i < COL_SIZE; i++) {
        parameter[thread_id].id = thread_id;
        parameter[thread_id].starting_row = 0;
        parameter[thread_id].starting_col = i;
        parameter[thread_id].ending_row = ROW_SIZE - 1;
        parameter[thread_id].ending_col = i;
        pthread_create(&(tid[thread_id]), NULL, validate, &(parameter[thread_id]));
        thread_id++;
    }

    
    int row = 0;
    int col = 0;
    for (int i = 0; i < NUM_OF_SUBGRIDS; i++) {
        parameter[thread_id].id = thread_id;
        parameter[thread_id].starting_row = row;
        parameter[thread_id].starting_col = col;
        parameter[thread_id].ending_row = row + 2;
        parameter[thread_id].ending_col = col + 2;
        pthread_create(&(tid[thread_id]), NULL, validate, &(parameter[thread_id]));
        thread_id++;

        
        if (col >= 6) {
            row += 3;
            col = 0;
        } else {
            col += 3;
        }
    }

    
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(tid[i], NULL);
        if (worker_validation[i] != 1) {
            return 0;
        }
    }
    
    free(worker_validation);
    free(tid);

    return 1;
}
