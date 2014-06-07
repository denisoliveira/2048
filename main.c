#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

/* TOP */
#define B_T_R "\342\225\224"
#define B_T_C "\342\225\246"
#define B_T_L "\342\225\227"

/* MIDDLE */
#define B_M_R "\342\225\240"
#define B_M_C "\342\225\254"
#define B_M_L "\342\225\243"

/* BOTTOM */
#define B_B_R "\342\225\232"
#define B_B_C "\342\225\251"
#define B_B_L "\342\225\235"

/* STRIP */
#define B_H "\342\225\220"
#define B_V "\342\225\221"

/* NUMBER COLORS*/
#define T0    "    "
#define T2    "\033[0;31m   2\033[0m"
#define T4    "\033[0;32m   4\033[0m"
#define T8    "\033[0;33m   8\033[0m"
#define T16   "\033[0;34m  16\033[0m"
#define T32   "\033[0;35m  32\033[0m"
#define T64   "\033[0;36m  64\033[0m"
#define T128  "\033[0;37m 128\033[0m"
#define T256  "\033[1;31m 256\033[0m"
#define T512  "\033[1;32m 512\033[0m"
#define T1024 "\033[1;33m1024\033[0m"
#define T2048 "\033[1;34m2048\033[0m"

/* KEYS */
#define KEY_OTHER 0
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4

/* DIMENSIONS */
#define ROWS 4
#define COLUMNS 4

/* SPACE */
#define SPACE_SCORE "                         "


typedef struct {
	int points;
	int max;
} SCORE;

typedef struct {
	int **table;
	int column; 
} ARGS;

typedef struct {
	SCORE *score;
	ARGS *args;
} SUM;

int getArrow(void);
const char* getColor(int);
void printBox(void);
const char *printColor(int);
void printNumber(int **);
void printScore(SCORE *);
void randomNumber(int **);
void actionFix(void *, int **); 
void actionSum(void *, int **, SCORE *); 
void* fixUp(void *);
void* fixDown(void *);
void* fixRight(void *);
void* fixLeft(void *);
void* sumUp(void *);
void* sumDown(void *);
void* sumRight(void *);
void* sumLeft(void *);

int getArrow(void) {
	struct termios tAnt, tNovo;
	int arrow = KEY_OTHER;

	char *key = (char *) malloc(sizeof(char) * 3);
	
	int i = 0;
	for (i = 0; i < 3; i++) {
		*(key + i) = 0;
	}
	
	printf("\033[s");
	printf("\n");

	tcgetattr(STDIN_FILENO, &tAnt);

	tNovo = tAnt;
	tNovo.c_lflag &= ~(ICANON | ECHO);
	tNovo.c_iflag &= ~(ISTRIP | INPCK);
	tcsetattr(STDIN_FILENO, TCSANOW, &tNovo);

	__fpurge(stdin);
	read(STDIN_FILENO, key, sizeof(char) * 3); 

	tcsetattr(STDIN_FILENO, TCSANOW, &tAnt);

	printf("\033[u");

	if (*key == 27 ) {
		if (*(key + 1) == 91)
		switch(*(key + 2)) {
			case 65:
				arrow = KEY_UP;
				break;
			case 66:
				arrow = KEY_DOWN;
				break;
			case 67:
				arrow = KEY_RIGHT;
				break;
			case 68:
				arrow = KEY_LEFT;
				break;
		}
	}

	free(key);
	return arrow;
}

const char* getColor(int number) {
	switch ( number ) {
		case 0 : return T0;
		case 2 : return T2;
		case 4 : return T4;
		case 8 : return T8;
		case 16 : return T16;
		case 32 : return T32;
		case 64 : return T64;
		case 128 : return T128;
		case 256 : return T256;
		case 512 : return T512;
		case 1024 : return T1024;
		case 2048 : return T2048;	
		default : return T0; 
	}
}

void printBox() {
	printf("\033[H\033[2J");
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", B_T_R, B_H, B_H, B_H, B_H, B_T_C, B_H, B_H, B_H, B_H, B_T_C, B_H, B_H, B_H, B_H, B_T_C, B_H, B_H, B_H, B_H, B_T_L);
	printf("%s%s%s%s%s%s%s%s%s\n", B_V, "    ", B_V, "    ", B_V, "    ", B_V, "    ", B_V);
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", B_M_R, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_L);
	printf("%s%s%s%s%s%s%s%s%s\n", B_V, "    ", B_V, "    ", B_V, "    ", B_V, "    ", B_V);
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", B_M_R, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_L);
	printf("%s%s%s%s%s%s%s%s%s\n", B_V, "    ", B_V, "    ", B_V, "    ", B_V, "    ", B_V);
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", B_M_R, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_C, B_H, B_H, B_H, B_H, B_M_L);
	printf("%s%s%s%s%s%s%s%s%s\n", B_V, "    ", B_V, "    ", B_V, "    ", B_V, "    ", B_V);
	printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", B_B_R, B_H, B_H, B_H, B_H, B_B_C, B_H, B_H, B_H, B_H, B_B_C, B_H, B_H, B_H, B_H, B_B_C, B_H, B_H, B_H, B_H, B_B_L);

	printf("\033[s");
	printf("\033[2;32HSCORE");
	printf("\033[u");
}

void printNumber(int **table) {
	int i, j, x, y;

	printf("\033[s");
	printf("\033[H");

	for (i=0, y=2; i<4; i++, y+=2) {
		for (j=0, x=2; j<4; j++, x+=5) {
			printf("\033[%d;%dH%s", y, x, getColor(*(*(table + i) + j)));
		}
	}
	
	printf("\033[u");
}

int decimal(int x) {
	if (x < 9) {
		return 0;
	}
	else {
		return 1 + decimal(x / 10);
	}
}

void printScore(SCORE *score) {
	printf("\033[s");
	printf("\033[H");

	printf("\033[3;24H%s", SPACE_SCORE);
	printf("\033[3;%dH%d", 34 - (decimal(score->points) >> 1), score->points);
	
	printf("\033[u");
}

void randomNumber(int **table) {
	int value, column, row;

	srand(time(NULL));

	do {
		column = rand() % COLUMNS;
		row = rand() % ROWS;
		value = *(*(table + row) + column);
	} while (value != 0);

	value = *(*(table + row) + column) = 2;
}


void* fixUp(void *mArgs) {
	int **table = ((ARGS *) mArgs)->table;
	int column = ((ARGS *) mArgs)->column;

	int i, j, n;

	for (i = 1; i < ROWS; i++) {
		if (*(*(table + i) + column) != 0) {
			n = i;

			for (j = n - 1; j > -1; j--) {
				if (*(*(table + j) + column) == 0) {
					n = j;
				}
			}

			if (n != i) {
				*(*(table + n) + column) = *(*(table + i) + column);
				*(*(table + i) + column) = 0;
			}
		}
	}

        pthread_exit(NULL);
}

void* fixDown(void *mArgs) {
	int **table = ((ARGS *) mArgs)->table;
	int column = ((ARGS *) mArgs)->column;

	int i, j, n;

	for (i = 2; i > -1; i--) {
		if (*(*(table + i) + column) != 0) {
			n = i;

			for (j = n + 1; j < ROWS; j++) {
				if (*(*(table + j) + column) == 0) {
					n = j;
				}
			}

			if (n != i) {
				*(*(table + n) + column) = *(*(table + i) + column);
				*(*(table + i) + column) = 0;
			}
		}
	}

        pthread_exit(NULL);
}

void* fixRight(void *mArgs) {
	int **table = ((ARGS *) mArgs)->table;
	int row = ((ARGS *) mArgs)->column;

	int i, j, n;

	for (i = 2; i > -1; i--) {
		if (*(*(table + row) + i) != 0) {
			n = i;

			for (j = n + 1; j < COLUMNS; j++) {
				if (*(*(table + row) + j) == 0) {
					n = j;
				}
			}

			if (n != i) {
				*(*(table + row) + n) = *(*(table + row) + i);
				*(*(table + row) + i) = 0;
			}
		}
	}

        pthread_exit(NULL);
}

void* fixLeft(void *mArgs) {
	int **table = ((ARGS *) mArgs)->table;
	int row = ((ARGS *) mArgs)->column;

	int i, j, n;

	for (i = 1; i < COLUMNS; i++) {
		if (*(*(table + row) + i) != 0) {
			n = i;

			for (j = n - 1; j > -1; j--) {
				if (*(*(table + row) + j) == 0) {
					n = j;
				}
			}

			if (n != i) {
				*(*(table + row) + n) = *(*(table + row) + i);
				*(*(table + row) + i) = 0;
			}
		}
	}

        pthread_exit(NULL);
}

void actionFix(void *function, int **table) {
	pthread_t *mThreads;
	pthread_attr_t *mAttr;

	int columns;

	mThreads = malloc(COLUMNS * sizeof(pthread_t));
	mAttr = malloc(sizeof(pthread_attr_t));

        pthread_attr_init(mAttr);

	ARGS *mArgs;

	for (columns = 0; columns < COLUMNS; columns++) {
		mArgs = malloc(sizeof(ARGS));
		mArgs->table = table;
		mArgs->column = columns;
		pthread_create((mThreads + columns), mAttr, function, (void *) mArgs);
	}
	
	for (columns = 0; columns < COLUMNS; columns++) {
		pthread_join(*(mThreads + columns), NULL);
	}
	
	pthread_attr_destroy(mAttr);

	free(mThreads);
	free(mAttr);
}

void actionSum(void *function, int **table, SCORE *score) {
	pthread_t *mThreads;
	pthread_attr_t *mAttr;

	int columns;

	mThreads = malloc(COLUMNS * sizeof(pthread_t));
	mAttr = malloc(sizeof(pthread_attr_t));

        pthread_attr_init(mAttr);

	SUM *sum;
	ARGS *mArgs;

	for (columns = 0; columns < COLUMNS; columns++) {
		mArgs = malloc(sizeof(ARGS));
		mArgs->table = table;
		mArgs->column = columns;

		sum = malloc(sizeof(SUM));
		sum->args = mArgs;
		sum->score = score;

		pthread_create((mThreads + columns), mAttr, function, (void *) sum);
	}
	
	for (columns = 0; columns < COLUMNS; columns++) {
		pthread_join(*(mThreads + columns), NULL);
	}
	
	pthread_attr_destroy(mAttr);

	free(mThreads);
	free(mAttr);
}


void *sumUp(void *sum) {
	int **table = ((SUM *) sum)->args->table;
	int column = ((SUM *) sum)->args->column;
	SCORE *score = ((SUM *) sum)->score;
	
	int i;
	
	for (i = 0; i < ROWS - 1; i++) {
		if (*(*(table + i) + column) != 0) {
			if (*(*(table + i + 1) + column) == *(*(table + i) + column)) {
				*(*(table + i) + column) <<= 1;
				score->points += *(*(table + i) + column);
				i++;
				*(*(table + i) + column) = 0;
			}
		}	
	}

        pthread_exit(NULL);
}

void *sumDown(void *sum) {
	int **table = ((SUM *) sum)->args->table;
	int column = ((SUM *) sum)->args->column;
	SCORE *score = ((SUM *) sum)->score;

	int i;

	for (i = ROWS - 1; i > 0; i--) {
		if (*(*(table + i) + column) != 0) {
			if (*(*(table + i - 1) + column) == *(*(table + i) + column)) {
				*(*(table + i) + column) <<= 1;
				score->points += *(*(table + i) + column);
				i--;
				*(*(table + i) + column) = 0;
			}
		}	
	}

        pthread_exit(NULL);
}

void *sumRight(void *sum) {
	int **table = ((SUM *) sum)->args->table;
	int row = ((SUM *) sum)->args->column;
	SCORE *score = ((SUM *) sum)->score;
	
	int i;
	
	for (i = COLUMNS - 1; i > 0; i--) {
		if (*(*(table + row) + i) != 0) {
			if (*(*(table + row) + i - 1) == *(*(table + row) + i)) {
				*(*(table + row) + i) <<= 1;
				score->points += *(*(table + row) + i);
				i--;
				*(*(table + row) + i) = 0;
			}
		}	
	}

        pthread_exit(NULL);
}

void *sumLeft(void *sum) {
	int **table = ((SUM *) sum)->args->table;
	int row = ((SUM *) sum)->args->column;
	SCORE *score = ((SUM *) sum)->score;
	
	int i;

	for (i = 0; i < COLUMNS - 1; i++) {
		if (*(*(table + row) + i) != 0) {
			if (*(*(table + row) + i + 1) == *(*(table + row) + i)) {
				*(*(table + row) + i) <<= 1;
				score->points += *(*(table + row) + i);
				i++;
				*(*(table + row) + i) = 0;
			}
		}	
	}

        pthread_exit(NULL);
}

int main() {
	int i, j, columns, key;

	SCORE *score = (SCORE *) malloc(sizeof(SCORE));
	
	int **table = malloc(ROWS * sizeof(int *));
	*(table) = malloc(ROWS * COLUMNS * sizeof(int));

	for (i = 0; i < ROWS; i++) {
		*(table + i) = (*(table) + i * COLUMNS);
	}

	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			*(*(table + i) + j) = 0;
		}
	}

	randomNumber(table);
	randomNumber(table);

	score->points = 0;
	score->max  = 0;

	printBox();

	printScore(score);
	printNumber(table);

	while ( key = getArrow() ) {
		switch ( key ) {
			case KEY_UP: {
				actionFix(fixUp, table);
				actionSum(sumUp, table, score);
				actionFix(fixUp, table);
				break;
			}

			case KEY_DOWN: {
				actionFix(fixDown, table);
				actionSum(sumDown, table, score);
				actionFix(fixDown, table);
				break;
			}

			case KEY_RIGHT: {
				actionFix(fixRight, table);
				actionSum(sumRight, table, score);
				actionFix(fixRight, table);
				break;
			}

			case KEY_LEFT: {
				actionFix(fixLeft, table);
				actionSum(sumLeft, table, score);
				actionFix(fixLeft, table);
				break;
			}
		}
		
		randomNumber(table);

		printScore(score);
		printNumber(table);
	}

	free(*(table));
	free(table);
	free(score);

	return 0;
}
