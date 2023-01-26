#ifndef TYPES_H
#define TYPES_H

typedef struct {
	int points;
	int max;
} SCORE;

typedef struct {
	int **table;
	int dimension;
	int *status;
} ARGS;

typedef struct {
	SCORE *score;
	ARGS *args;
} SUM;

#endif