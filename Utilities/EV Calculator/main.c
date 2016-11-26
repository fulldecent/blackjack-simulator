/*
 * main.c simulate 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	int rank[11];
	int total;
	int count;
} Cards;

/* update these to use the cool industry-standard abbreviations
    int x17; 0=stand on 17 S17, 1=hit on 17 H17 */

typedef struct {
	int standSoft17;         /* 0=hit on soft 17; 1=stand on soft 17 */
	int canDouble;           /* 0=no doubling; 1=double on anything */
	int canDoubleAfterSplit; /* 0=no; 1=yes */
	int canSurrender;        /* 0=can't 1=can */
	int splits;              /* number times you can split */
	int splitAces;           /* number of times you can split aces */
	int hitSplitAces;        /* 0=no; 1=yes */
/*	int lateSurrender;   */    /* 0=no; 1=yes */ /* THIS IS NOT BEING USED */
	int bjPays32;            /* 0=6/5; 1=3/2 */
	int cutCard;             /* after dealing this many cards, no new hands are started */
	                         /* Note: the theoretical maximum number of cards that
	                            can be used in one round is: 
	                            dealer: A A A A A A 6 A A A A A
	                            player: (splits+1) * 21 Aces
	                         */
	int canHit;              /* May set to zero if you can't hit split aces */
} Rules;

/* Input:
 *   situation (0=hard, 1=soft, 2=pair)
 *   player total, with aces as one (2..20)
 *   dealer up card (1..10)
 * Output:
 *   List of moves, in order of preference. If no move is legal, Stand is assumed
 *   The moves are: Hit, Stand, Double, sPlit, suRrender
 */
typedef char Strategy[3][22][11][4];

int is_blackjack(Cards cards)
{
	return cards.count == 2 && cards.total == 11 && cards.rank[1]; 
}

/* Removes a card R from the shoe, or randomly if C=0 */
int pop(Cards *cards, Cards *cards2, int r)
{
	int i;

	if (!cards->count)
		*cards = *cards2;

	if (r==0) {
		i = rand() % cards->count;

		do {
			r++;
			i -= cards->rank[r];
		} while (i >= 0);
	}
	cards->rank[r]--;
	cards->total -= r;
	cards->count--;
	return r;
}


// ############## 

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
		if (10==sscanf(line, "%3s\t%3s\t%3s\t%3s\t%3s\t%3s\t%3s\t%3s\t%3s\t%3s",
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

// ##############

/* Returns expected value of showdown given player hands and bets on each hand
 */
double ev_showdown(Cards deck1, Cards deck2, Cards dealer, Cards player[10], int bet[10], int numhands, Rules rules)
{
	int dealer_effective, player_effective, i;
	double retval=0;

	dealer_effective = dealer.total + (dealer.total < 12 ? !!dealer.rank[1]*10 : 0);
	if (dealer_effective >= 17 && !(dealer_effective == 17 && !rules.standSoft17 && dealer.total == 7)) {
		for (i=0; i<numhands; i++) {
			if (player[i].total > 21) 
				retval -= bet[i];
			if (dealer_effective > 21) {
				retval += bet[i];
				break;
			}
			player_effective = player[i].total + (player[i].total < 12 ? !!player[i].rank[1]*10 : 0);
			if (player_effective < dealer_effective)
				retval -= bet[i];
			else if (player_effective > dealer_effective)
				retval += bet[i];
			/* else retval += 0 */
		}
		return retval;
	}

	if (!deck1.count)
		deck1 = deck2;

	for (i=1; i<=10; i++) {
		if (deck1.rank[i]) {
			deck1.rank[i]--, deck1.count--, deck1.total-=i;
			dealer.rank[i]++, dealer.count++, dealer.total+=i;
			retval += ev_showdown(deck1, deck2, dealer, player, bet, numhands, rules)
			          * (deck1.rank[i]+1) / (deck1.count+1);
			dealer.rank[i]--, dealer.count--, dealer.total-=i;
			deck1.rank[i]++, deck1.count++, deck1.total+=i;
		}
	}
	return retval;
}

// ##############

/* Plays one hand using STRATEGY against representative dealing from given cards
 *
 * Returns payout
 */
double play_one_round(Cards deck1, Cards deck2, Rules rules, Strategy *strategy)
{
	int i, x;
	int c;
	char *ps;

	Cards player[10] = {{{0}}}, dealer = {{0}};
	int curhand = 0, numhands = 1;
	int bet[10] = {1,1,1,1,1,1,1,1,1,1};

	int dealer_up, dealer_down;
	 
	c = pop(&deck1, &deck2, 2);
	player[0].total += c;
	player[0].rank[c]++;
	player[0].count++;

	c = pop(&deck1, &deck2, 4);
	dealer_up = c;

	c = pop(&deck1, &deck2, 10);
	player[0].total += c;
	player[0].rank[c]++;
	player[0].count++;
		
	c = pop(&deck1, &deck2, 0);
	dealer_down = c;

	if ((dealer_up + dealer_down == 11) && (dealer_up == 1 || dealer_down == 1)) {
		if (is_blackjack(player[0]))
			return 0;
		return -1;
	}

	if (is_blackjack(player[0]))
		return rules.bjPays32 ? 3/2.0 : 6/5.0;

	while (curhand < numhands) {
		if (player[curhand].count>=21 || (player[curhand].count==11 && player[curhand].rank[1])) {
			curhand++;
			continue;
		}

		i = (player[curhand].count == 2 && rules.splits >= numhands)? 2 : 0;
		if (i == 2 && player[curhand].rank[1] && rules.splitAces < numhands)
			i = 0;
		if (i == 2) {
			for (x=1; x<=10; x++) {
				if (player[curhand].rank[x] == 2)
					c = x;
				else if (player[curhand].rank[x] != 0) {
					i = 0;
					break;
				}
			}
		}
		if (i==0 && player[curhand].total <=11 && player[curhand].rank[1])
			i = 1;

                ps = (*strategy)[i][player[curhand].total][dealer_up];
		
		for (x=0; x<5; x++) {
			if (ps[x] == 'S' || ps[x] == '\0') {
				curhand++;
				break;
			} else if (ps[x] == 'H' && rules.canHit) {
				c = pop(&deck1, &deck2, 0);
				player[curhand].total += c;
				player[curhand].rank[c]++;
				player[curhand].count++;
				break;
			} else if (ps[x] == 'D' && player[curhand].count == 2 && rules.canDouble) {
				c = pop(&deck1, &deck2, 0);
				player[curhand].total += c;
				player[curhand].rank[c]++;
				player[curhand].count++;
				bet[curhand] = 2;
				curhand++;
				break;
			} else if (ps[x] == 'U' && player[curhand].count == 2 && rules.canSurrender) {
				player[curhand].total = 99;
				curhand++;
			} else if (ps[x] == 'P' && i == 2) {
				if (c == 1 && !rules.hitSplitAces)
					rules.canHit = 0;
				for (i = numhands; i > curhand+1; i--)
					player[i] = player[i-1];
				player[curhand].total = player[curhand+1].total = c;
				player[curhand].rank[c] = player[curhand+1].rank[c] = 1;
				c = pop(&deck1, &deck2, 0);
				player[curhand].total += c;
				player[curhand].rank[c]++;
				c = pop(&deck1, &deck2, 0);
				player[curhand+1].total += c;
				player[curhand+1].rank[c]++;
				player[curhand].count = player[curhand+1].count = 2;
				rules.canDouble = rules.canDoubleAfterSplit;
				rules.canSurrender = 0;
				rules.canHit = rules.hitSplitAces;
				numhands++;
				break; /* This hand will get replayed with the new card */
			} else if (ps[x] == 'R' && player[curhand].count == 2 && numhands == 1)
				return -0.5;
				
		}
	};

	dealer.total = dealer_up + dealer_down;
	dealer.count = 2;
	dealer.rank[dealer_up]++;
	dealer.rank[dealer_down]++;

	return ev_showdown(deck1, deck2, dealer, player, bet, numhands, rules);
}


int main(int argc, char *argv[])
{
	Cards shoe = {{0},0,0}, nocards = {{0},0,0};
	Rules rules;
	Strategy strategy = {{{"X"}}};
srand(time(0));
	rules.standSoft17 = 1;
	rules.canDoubleAfterSplit = 1;
	rules.canDouble = 1;
	rules.canSurrender = 1;
	rules.splits = 3;
	rules.splitAces = 1;
	rules.hitSplitAces = 0;
	rules.bjPays32 = 1;
	rules.cutCard = 999;
	rules.canHit = 1;

	int i;
	double total, rounds;
	
	if (argc != 13) {
		printf("usage: %s Ones Twos ... Tens/Faces CutCard STRATEGY\n", argv[0]);
		puts("\nThis empirically calculates the EV of one hand with a given strategy and deck.");
		exit(1);
	}

	for (i=1; i<=10; i++) {
		shoe.rank[i] = atoi(argv[i]);
		shoe.total += i * atoi(argv[i]);
		shoe.count += atoi(argv[i]);
	}
	rules.cutCard = atoi(argv[11]);

	read_strategy(argv[12], &strategy);

	total = 0;
	rounds = 1;

	while (rounds < 100000) {
		total += play_one_round(shoe, nocards, rules, &strategy);
		rounds++;
	}

	printf("Rounds\tOutcome\n");
	printf("-------\t-----------\n");
	printf("%f\t%.16f\n", rounds, total/rounds);

	return 0;
}
