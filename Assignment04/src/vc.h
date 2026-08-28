#ifndef VC_H
#define VC_H


int *vertexColoring(int V, const int *rowPtr, const int *colIndex, int *numColorsUsed);


int isValidColoring(int V, const int *rowPtr, const int *colIndex, const int *colors);

#endif 