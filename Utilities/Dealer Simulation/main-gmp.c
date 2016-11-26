/*
 * dealer.c simulate a dealer's actions with given start cards and a given deck
 * ./dealer UpCard DownCard Ones Twos ... Tens/Faces [Ones Twos ... Tens/Faces]
 *
 * Run this program with 12 or 22 paramaters:
 * Param 1 and 2 are dealer's initial cards, use zero to have these pull from deck 1
 * Params 3..12 are counts of cards in the first deck <#aces, #twos, #threes, ..., #nines, #tens/faces>
 * Params 13..22 are counds of cards in the second deck, used when deck 1 is exhausted
 *
 * This program will iterate through all possible game plays and output probability of each outcome
 *
 * User is responsible to ensure no underflow of cards!
 *
 * NOTE: Passing zeros doesn't really make sense because this program does not check for blackjack
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmp.h>

void play(int count, int aces, int cards, int deck1[11], int countdeck1, int deck2[11], 
          int countdeck2, mpq_t outcomes[27], mpq_t probability)
{
	int mydeck1[11], mydeck2[11]={0}, effective, i;
	mpq_t newprob, num, den;
	mpq_init(newprob);
	mpq_init(num);
	mpq_init(den);

	effective = (aces && count<=11) ? count+10 : count;
	if (effective==21 && cards==2) {
		mpq_add(outcomes[1], outcomes[1], probability);	
		return;
	} else if (effective>=17) {
		mpq_add(outcomes[effective], outcomes[effective], probability);	
		return;
	}

	if (countdeck1) {
		memcpy(mydeck1, deck1, sizeof(deck1[0]) * 11);
		memcpy(mydeck2, deck2, sizeof(deck2[0]) * 11);
	} else {
		memcpy(mydeck1, deck2, sizeof(deck2[0]) * 11);
		countdeck1 = countdeck2;
		countdeck2 = 0;
	}

	for (i=1; i<=10; i++) {
		if (mydeck1[i]) {
			mydeck1[i]--;
			mpq_set(newprob, probability);
			mpq_set_d(num, mydeck1[i]+1);
			mpq_set_d(den, countdeck1);
			mpq_mul(newprob, newprob, num);
			mpq_div(newprob, newprob, den);
			play(count+i, aces+(i==1?1:0), cards+1, mydeck1, countdeck1-1, mydeck2, countdeck2,
			     outcomes, newprob);
			mydeck1[i]++;
		}
	}
}

int main(int argc, char *argv[])
{
	int count, aces, deck1[11]={0}, deck2[11]={0}, i, countdeck1=0, countdeck2=0, cards;
	mpq_t outcomes[27];
	mpq_t one;

	if (argc != 13 && argc != 23) {
		printf("usage: %s UpCard DownCard Ones Twos ... Tens/Faces [Ones Twos ... Tens/Faces]\n", argv[0]);
		exit(1);
	}

	count = atoi(argv[1]) + atoi(argv[2]);
	aces = (atoi(argv[1]) == 1) + (atoi(argv[2]) == 1);
	cards = !!atoi(argv[1]) + !!atoi(argv[2]);
	for (i=1; i<=10; i++) {
		countdeck1 += deck1[i] = atoi(argv[i+2]);
		if (argc == 22) {
			countdeck2 += deck2[i] = atoi(argv[i+12]);
		}
	}

	mpq_init(one);
	mpq_set_ui(one, 1, 1);
	for (i=0; i<27; i++)
		mpq_init(outcomes[i]);
	play(count, aces, cards, deck1, countdeck1, deck2, countdeck2, outcomes, one);

	printf("Outcome\tProbability\n");
	printf("-------\t-----------\n");
	for (i=17; i<=21; i++)
		gmp_printf("%d\t%Qd\n", i, outcomes[i]);
	gmp_printf("BJ\t%Qd\n", outcomes[1]);
	for (i=22; i<=26; i++)
		mpq_add(outcomes[2], outcomes[2], outcomes[i]);
	gmp_printf("Bust\t%Qd\n", outcomes[2]);

	return 0;
}
