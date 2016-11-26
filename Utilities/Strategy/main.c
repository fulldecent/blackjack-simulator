/*
 * Read and print a strategy file
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Input:
 *   situation (0=hard, 1=soft, 2=pair)
 *   player total, with aces as one (2..20)
 *   dealer up card (1..10)
 * Output:
 *   List of moves, in order of preference. Must always end with H or S
 *   The moves are: Hit, Stand, Double, sPlit, sUrrender
 */
typedef char Strategy[3][22][11][4];

/* Reads strategy in FILENAME to STRATEGY. 
 * Returns 1 on success, 0 on failure
 */
int read_strategy(char *filename, Strategy *strategy)
{
	int i, j, k;
	char line[200], *pc;
	FILE *fp;

	/* Fill in bogus values, assert these are never used */
	for (i=0; i<3; i++)
		for (j=0; j<22; j++)
			for (k=0; k<11; k++)
				memcpy((*strategy)[i][j][k], "X", 2);
	
	fp = fopen(filename, "r");
	if (fp == NULL) {
		puts("ERROR: Could not open strategy file");
		exit(1);
	}
	
	i=0, j=5, k=2;
	while (fgets(line, sizeof line, fp)) {
		for (pc=line+strlen(line)-1; pc>=line; pc--)
			if (*pc=='#')
				*pc=0x00; 
		if (10==sscanf(line, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
		           (*strategy)[i][j][2], (*strategy)[i][j][3], (*strategy)[i][j][4],
		           (*strategy)[i][j][5], (*strategy)[i][j][6], (*strategy)[i][j][7],
		           (*strategy)[i][j][8], (*strategy)[i][j][9], (*strategy)[i][j][10], (*strategy)[i][j][1])) {
			j += i<2? 1:2;
			if (i==0 && j==22)
				i=1, j=3;
			else if (i==1 && j==12)
				i=2, j=4;
			else if (i==2 && j==22)
				j=2;
		}
		
	}

	return 1;
}

void print_strategy(Strategy strategy)
{
	int j, k;

	puts("HARD\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   10\t   A");
	puts("-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------");
	for (j=5; j<22; j++) {
		printf("%d",j);	
		for (k=2; k<12; k++) {
			printf("\t  %s", strategy[0][j][(k-1)%10+1]);
			if (k==11)
				puts("");
		}
	}

	puts("");
	puts("SOFT\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   10\t   A");
	puts("-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------");
	for (j=3; j<12; j++) {
		printf("%d",j+10);	
		for (k=2; k<12; k++) {
			printf("\t  %s", strategy[1][j][(k-1)%10+1]);
			if (k==11)
				puts("");
		}
	}

	puts("");
	puts("PAIR\t   2\t   3\t   4\t   5\t   6\t   7\t   8\t   9\t   10\t   A");
	puts("-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------\t-------");
	for (j=4; j<24; j+=2) {
		if (j/2==11) {
			printf("A,A");
			for (k=2; k<12; k++) {
				printf("\t  %s", strategy[2][2][(k-1)%10+1]);
				if (k==11)
					puts("");
			}
		} else { 
			printf("%d,%d",j/2, j/2);
			for (k=2; k<12; k++) {
				printf("\t  %s", strategy[2][j][(k-1)%10+1]);
				if (k==11)
					puts("");
			}
		}
	}
}

int main(int argc, char *argv[])
{
	Strategy strategy = {{{"X"}}};

	if (argc != 2) {
		printf("usage: %s strategy.txt\n", argv[0]);
		exit(1);
	}

	read_strategy(argv[1], &strategy);
	print_strategy(strategy);

	return 0;
}
