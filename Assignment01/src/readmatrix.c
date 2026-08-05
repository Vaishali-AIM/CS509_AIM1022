#include <stdio.h>
#include "readmatrix.h"
void rm(FILE *fp,int rows, int cols, int mn[rows][cols])
{
    for (int i = 0; i < rows; i++)  
      {
        for (int j = 0; j < cols; j++) 
          {
            if (fscanf(fp, "%d", &mn[i][j]) != 1) {
                printf("ERROR: expected %d integers for matrix, but got fewer.\n", rows * cols);
                fclose(fp);
                return;
            }
          }
       }
 }