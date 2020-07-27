#include "board.h"
#include "cell.h"
#include "UI.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


ERROR export_board(char* path, Board *B){
    FILE *fptr;
    int i, j;

    fptr =  fopen(path,"w");
    if (!fptr){
        fclose(fptr);
        printf("Unuccessful save.\n");
        return ILLIGEAL_PATH;
    }

    /*write m n to file*/
    fprintf(fptr, "%d %d\n",B->blocks_per_cols, B->blocks_per_rows);
    printf("%d %d\n",B->blocks_per_cols, B->blocks_per_rows);

    /*copy board to file*/
    for (i=0;i<B->num_of_rows;++i){
        for (j=0;j<B->num_of_cols;++j){
            fprintf(fptr, "%d",B->cells[i][j].value);
            if (B->cells[i][j].is_fixed){
                fprintf(fptr, ".");
            }
            if (j!=B->num_of_cols-1){
                fprintf(fptr, " ");
            }
            if (j==B->num_of_cols-1){
                fprintf(fptr, "\n");
            }
        }
    }

fclose(fptr);
printf("Successful save.\n");
return SUCCESS;
}


ERROR import_board(char* path, Board *B, char *info){
    FILE *fptr;
    int n,m;
    int i,j, val, cnt;
    char isFixed;

    fptr =  fopen(path, "r");
    if (!fptr){
        fclose(fptr);
        sprintf(info,"'%s' is not a valid text file path\n",path);
        B->exists = FALSE;
        return ILLIGEAL_PATH;
    }

    /* EXTRACT n, m values */
    cnt = fscanf(fptr, "%d %d", &m, &n);
    if (!n || !m || cnt!=2){
        fclose(fptr);
        sprintf(info, "%s", "The first line should contain 2 integer values that are representing block height and width, respectively.\n");
        B->exists = FALSE;
        return WRONG_FILE_FORMAT_MISSING_VALS_TOP; 
    }

    new_board(B, n*m, m*n, m, n);

    for (i=0; i<m*n; ++i){
        for (j=0; j<m*n; ++j){
            cnt = fscanf(fptr,"%d",&val);
            if (cnt!=1){
                fclose(fptr);
                sprintf(info, "Board with m=%d and n=%d should contain %d values.\n", m,n, m*n*m*n);
                return WRONG_FILE_FORMAT_MISSING_VALS; 
            }   
            if (val<0 || val>n*m){
                fclose(fptr);
                sprintf(info, "Encountered value '%d', when the range is 1-%d", val, m*n);
                return WRONG_FILE_FORMAT_RANGE; 
            }
            B->cells[i][j].value = val;
            isFixed = fgetc(fptr);
            if (isFixed == '.'){ 
                B->cells[i][j].is_fixed = TRUE;
            }
            if (isFixed!= '.' && isFixed!= ' ' && isFixed!= '\t' && isFixed!= '\r' && isFixed!= '\n' && isFixed!= EOF){
                fclose(fptr);
                sprintf(info, "Encountered value '%d', when the range is 1-%d", val, m*n);
                return WRONG_FILE_FORMAT_MISSING_VALS;
            }
        }
    }

    cnt = fscanf(fptr,"%d",&val);
    if (cnt!=0 && cnt!=-1){
        sprintf(info, "Board with m=%d and n=%d should contain %d values.\n", m,n, m*n*m*n);
        fclose(fptr);
    return WRONG_FILE_FORMAT_TOO_MANY_VALS; 
    } 

fclose(fptr);
return SUCCESS;
}
