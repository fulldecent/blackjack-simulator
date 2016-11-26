/*
 * dealer.c simulate a dealer's actions with given start cards and a given deck
 * ./dealer UpCard DownCard Ones Twos ... Tens/Faces [Ones Twos ... Tens/Faces]
 *
 * Run this program with 12 or 22 paramaters:
 * Param 1 and 2 are dealer's initial cards, use zero to have these pull from deck 1
 * Params 3..12 are counts of cards in the first deck <#aces, #twos, #threes, ..., #nines, #tens/faces>
 * Params 13..22 are counds of cards in the second deck
 *
 * This program will iterate through all possible game plays and output probability of each outcome
 *
 * The deal pulls cards from deck one. If exhausted, then pulls from deck two. You 
 * are responsible to ensure no underflow of cards!
 *
 * NOTE: Passing zeros doesn't really make sense because this program does not check for blackjack
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

void play(int count, int aces, int deck1[10], int countdeck1, int deck2[10], int countdeck2, 
          double outcomes[27], double probability)
{
	int mydeck1[10], mydeck2[10]={0}, effective, i;

for (i=50; i<=countdeck1; i++)
putc(' ', stdout);
printf("%d %d\n",count, aces);

	effective = (aces && count<=11) ? count+10 : count;
	if (effective>=17) {
		outcomes[effective] += probability;
		return;
	}

	if (countdeck1) {
		memcpy(mydeck1, deck1, sizeof(deck1[0]) * 10);
		memcpy(mydeck2, deck2, sizeof(deck2[0]) * 10);
	} else {
		memcpy(mydeck1, deck2, sizeof(deck2[0]) * 10);
		countdeck1 = countdeck2;
		countdeck2 = 0;
	}
i=9;
	for (i=1; i<=10; i++) {
		if (mydeck1[i]) {
			mydeck1[i]--;
//			play(count+i, aces+(i==1?1:0), mydeck1, countdeck1-1, mydeck2, countdeck2,
//			     outcomes, probability * (mydeck1[i]+1) / countdeck1);
			mydeck1[i]++;
		}
	}
}

int main(int argc, char *argv[])
{
	int count, aces, deck1[10]={0}, deck2[10]={0}, i, countdeck1=0, countdeck2=0;
	double outcomes[27]={0};

i=10;
printf("%d/n",i);
deck1[i]--;
printf("%d/n",i);


	
	if (argc != 13 && argc != 23) {
		printf("usage: %s UpCard DownCard Ones Twos ... Tens/Faces [Ones Twos ... Tens/Faces]\n", argv[0]);
		exit(1);
	}

	count = atoi(argv[1]) + atoi(argv[2]);
	aces = (atoi(argv[1]) == 1) + (atoi(argv[2]) == 1);
	for (i=0; i<10; i++) {
		countdeck1 += deck1[i] = atoi(argv[i+3]);
		if (argc == 22) {
			countdeck2 += deck2[i] = atoi(argv[i+13]);
		}
	}

	play(count, aces, deck1, countdeck1, deck2, countdeck2, outcomes, 1.0);

	printf("Outcome\tProbability\n");
	printf("-------\t-----------\n");
	for (i=0; i<27; i++)
		printf("%d\t%f\n", i, outcomes[i]);
//	printf("%d\t%f", i, outcome[22]+outcome[23]+outcome[24]+outcome[25]+outcome[26]);

	return 0;
}
