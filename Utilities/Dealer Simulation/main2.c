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

typedef struct {
    int rank[11];
    int total;
    int count;
} Cards;

void dealer_outcomes(Cards dealer, Cards deck1, Cards deck2, double outcomes[27], double probability)
{
	int effective, i;

	effective = (dealer.rank[1] && dealer.total<=11) ? dealer.total+10 : dealer.total;
	if (effective==21 && dealer.count==2) {
		outcomes[1] += probability;
		return;
	} else if (effective>=17) {
		outcomes[effective] += probability;
		return;
	}

	if (!deck1.count)
		deck1 = deck2;

	for (i=1; i<=10; i++) {
		if (deck1.rank[i]) {
			deck1.rank[i]--, deck1.count--, deck1.total-=i;
			dealer.rank[i]++, dealer.count++, dealer.total+=i;
			dealer_outcomes(dealer, deck1, deck2, outcomes, probability * (deck1.rank[i]+1) / (deck1.count+1));
			dealer.rank[i]--, dealer.count--, dealer.total-=i;
			deck1.rank[i]++, deck1.count++, deck1.total+=i;
		}
	}
}

int main(int argc, char *argv[])
{
	Cards dealer={{0},0,0}, deck1={{0},0,0}, deck2={{0},0,0};
	int i;
	double outcomes[27]={0};
	
	if (argc != 13 && argc != 23) {
		printf("usage: %s UpCard DownCard Ones Twos ... Tens/Faces [Ones Twos ... Tens/Faces]\n", argv[0]);
		exit(1);
	}

	dealer.rank[atoi(argv[1])]++;
	dealer.rank[atoi(argv[2])]++;
	dealer.total = atoi(argv[1]) + atoi(argv[2]);
	dealer.count = !!atoi(argv[1]) + !!atoi(argv[2]);
	for (i=1; i<=10; i++) {
		deck1.rank[i] = atoi(argv[i+2]);
		deck1.total += i * atoi(argv[i+2]);
		deck1.count += atoi(argv[i+2]);
		if (argc == 22) {
			deck2.rank[i] = atoi(argv[i+12]);
			deck2.total += i * atoi(argv[i+12]);
			deck2.count += atoi(argv[i+12]);
		}
	}

	dealer_outcomes(dealer, deck1, deck2, outcomes, 1.0);

	printf("Outcome\tProbability\n");
	printf("-------\t-----------\n");
	for (i=17; i<=21; i++)
		printf("%d\t%.16f\n", i, outcomes[i]);
	printf("BJ\t%.16f\n", outcomes[1]);
	printf("Bust\t%.16f\n", outcomes[22]+outcomes[23]+outcomes[24]+outcomes[25]+outcomes[26]);

	return 0;
}
