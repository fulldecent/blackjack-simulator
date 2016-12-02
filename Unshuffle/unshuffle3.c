/*
 * Reads PRE and POST shuffle files and conjectures about them.
 *
 * We are seeking 2 contiguous "sets of cards" in PRE that add up exactly to
 * a set of cards in POST.
 *
 * Uses 64-bit data types
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

/* Search constraints */
#define MIN_PRE_SET 10
#define MAX_PRE_SET 60
#define MIN_POST_SET 20
#define MAX_POST_SET 100
#define DECKS 6
#define FUZZ 2            /* Search for almost matches, allow input error up to FUZZ entries*/

/* Each card is a 64-bit number with only one bit set
   ah = 1<<0 ... kh = 1<<12
   as = 1<<13 ... hs = 1<<25
   ad = 1<<26 ... kd = 1<<38
   ac = 1<<39 ... kc = 1<<51
 */ 
void readfile(char *fname, uint64_t output[], int *len)
{
	FILE *fin;
	char line[200];
	int i=0;
	int rank;

	if ((fin = fopen(fname, "rb"))) {
		fseek(fin, 0L, SEEK_END);
		*len = ftell(fin);
		rewind(fin);
		printf("Opened file %s with %d bytes\n", fname, *len);
	} else {
		printf("Could not open file %s\n", fname);
		exit(1);
	}

	*len = 0;

	while (fgets(line, sizeof line, fin) != NULL) {
		for (i=0; i<strlen(line); i++) {
			if (isspace(line[i]))
				continue;
			else if (line[i] == '#' || line[i] == '=') {
				i = 999;
				continue;
			}
			else if (line[i] >= '2' && line[i] <= '9')
				rank = line[i] - '0';
			else if (line[i] == '1' && line[i+1] == '0') {
				rank = 10;
				i++;
			}
			else if (line[i] == '1' && line[i+1] != '0') {
				puts("ERROR: Found a 1 not followed by a 0");
				exit(1);
			}
			else if (line[i] == 'j')
				rank = 11;
			else if (line[i] == 'q')
				rank = 12;
			else if (line[i] == 'k')
				rank = 13;
			else if (line[i] == 'a')
				rank = 1;
			else {
				puts("ERROR: Invalid data file, seeking rank");
				exit(1);
			}
			i++;
//			printf("%d", rank);

			if (line[i] == 'h') 
				output[(*len)++] = (uint64_t) 1 << (rank - 1);
			else if (line[i] == 's')
				output[(*len)++] = (uint64_t) 1 << (rank - 1 + 13);
			else if (line[i] == 'd')
				output[(*len)++] = (uint64_t) 1 << (rank - 1 + 26);
			else if (line[i] == 'c')
				output[(*len)++] = (uint64_t) 1 << (rank - 1 + 39);
			else {
				puts("ERROR: Invalid data file, seeking suit");
				exit(1);
			}
//			putc(line[i], stdout);
//			puts("");

			if (*len > 52*DECKS) {
				puts("ERROR: Too many cards");
				exit(1);
			}
		}
	}
	printf("Read %d cards\n", *len);
}

/* Given an array of cards, cache "card sets" for each START location and LENGTH
   in the cards. The card set is DECKS 64-bit words long.

   For [C = 0], this is a logical OR of all cards in the partial deck
   [C = 1] is the logical OR of all cards that exist twice
   [C = 2] is the logical OF of all cards that exist three times, etc...
*/
void buildsets(uint64_t cards[], int cards_len, uint64_t cards_sets[52*DECKS][MAX_POST_SET][DECKS]) {
	int i, j, k;

	for (i=0; i<cards_len; i++) {
		/* For j=1 */
		cards_sets[i][1][0] = cards[i];
		for (k=1; k<DECKS; k++)
			cards_sets[i][1][k] = 0;

		for (j=2; j<100 && i+j-1<cards_len; j++) {
			for (k=0; k<DECKS; k++)
				cards_sets[i][j][k] = cards_sets[i][j-1][k];

			for (k=0; cards_sets[i][j][k] & cards[i+j-1]; k++);

			if (k>=DECKS) {
				printf("ERROR: There are %d decks but more than %d of one specific rank & suit!", DECKS, DECKS);
				exit(1);
			}

			cards_sets[i][j][k] |= cards[i+j-1];
		}
	}
}

/* MULTISET FUNCTIONS */

/* Returns number of differences between SETA and SETB */
int multiset_dist(uint64_t seta[DECKS], uint64_t setb[DECKS]) {
	uint64_t x, retval=0;
	int i;

        for (i=0; i<DECKS; i++) {
		x = seta[i] ^ setb[i];

		/* From Knuth... somewhere. Returns number of bits set */
		x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
		x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
		x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
		retval += (x * 0x0101010101010101ULL) >> 56;
	}
        return retval;
}

/* Returns number of cards left in SETA after subtracting SETB */
int multiset_subtract_dist(uint64_t seta[DECKS], uint64_t setb[DECKS]) {
	uint64_t x, retval=0;
	int i;

        for (i=0; i<DECKS; i++) {
		x = seta[i] & ~setb[i];

		/* From Knuth... somewhere. Returns number of bits set */
		x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
		x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
		x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
		retval += (x * 0x0101010101010101ULL) >> 56;
	}
        return retval;
}

/* Returns number of cards different between SETA+SETB and SETC */
int multiset_add_dist(uint64_t seta[DECKS], uint64_t setb[DECKS], uint64_t setc[DECKS]) {
	uint64_t setab[DECKS];
	uint64_t x, retval=0;
	uint64_t carry, temp;
	int a, b, i;

	memcpy(setab, seta, sizeof(seta[0]) * DECKS);

	for (b=0; b<DECKS; b++) {
		carry = setb[b];
		for (a=b; a<DECKS; a++) {
			temp = carry & setab[a];
			setab[a] |= carry;
			carry = temp;
		}
		if (carry) {
			printf("ERROR: Adding two sets of cards and there's more than %d of one card\n", DECKS);
			exit(1);
		}
	}

        for (i=0; i<DECKS; i++) {
		x = setab[i] ^ setc[i];

		/* From Knuth... somewhere. Returns number of bits set */
		x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
		x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
		x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
		retval += (x * 0x0101010101010101ULL) >> 56;
	}
        return retval;
}
/* =========== */

/* Returns 1 if SETA \ SETB is not empty */
int multiset_subtract(uint64_t seta[DECKS], uint64_t setb[DECKS]) {
	int i;
	for (i=0; i<DECKS; i++)
		if (seta[i] & ~setb[i])
			return 1;
	return 0;
}

/* Returns 1 if SETA == SETB */
int multiset_equal(uint64_t seta[DECKS], uint64_t setb[DECKS]) {
	return !memcmp(seta, setb, sizeof(seta[0]) * DECKS);
}

/* Returns 1 if SETA + SETB = SETC */
int multiset_add_equal(uint64_t seta[DECKS], uint64_t setb[DECKS], uint64_t setc[DECKS]) {
	uint64_t setab[DECKS] = {0};
	uint64_t carry, temp;
	int a, b;

	for (a=0; a<DECKS; a++) {
		setab[a] = seta[a];
	}

	for (b=0; b<DECKS; b++) {
		carry = setb[b];
		for (a=b; a<DECKS; a++) {
			temp = carry & setab[a];
			setab[a] |= carry;
			carry = temp;
		}
		if (carry) {
			puts("ERROR: Carry past number of decks!!");
			exit(1);
		}
	}

	return multiset_equal(setab, setc);
}

void multiset_print(uint64_t set[DECKS]) {
	int i,j;

	printf("H a234567890jqk  S a234567890jqk  D a234567890jqk  C a234567890jqk");
	for (i=0; i<DECKS; i++) {
		if (!set[i])
			break;
		printf("\n  ");
		for (j=0; j<52; j++) {
			putc('0'+(set[i] >> j)%2, stdout);
			if (j%13 == 12)
				printf("    ");
		}
	}
	puts("\n");
}

int main(int argc, char *argv[])
{
	uint64_t pre_cards[52*DECKS], post_cards[52*DECKS];
	int pre_cards_len, post_cards_len, i, j, k, f, z;
	int post_start, post_len, pre_start, pre_len;
	int hits_total=0;

	/* We are creating an index by [starting card] and [set size] */
	uint64_t pre_cards_sets[52*DECKS][MAX_POST_SET][6]; /* ~2MB */
	uint64_t post_cards_sets[52*DECKS][MAX_POST_SET][6]; 

	if (argc != 3) {
		printf("usage: %s PRE_file POST_file\n", argv[0]);
		exit(1);
	}

	readfile(argv[1], pre_cards, &pre_cards_len);
	readfile(argv[2], post_cards, &post_cards_len);
	buildsets(pre_cards, pre_cards_len, pre_cards_sets);
	buildsets(post_cards, post_cards_len, post_cards_sets);

	/* HERE IS THE SEARCH */
	for (post_start=0; post_start+MIN_POST_SET <= post_cards_len; post_start++) {
		for (post_len=MIN_POST_SET; post_len<=MAX_POST_SET && post_start+post_len<=post_cards_len; post_len++) {

			/* CANDIDATES are sets of cards in PRE that don't include anything that
			isn't in POST. (plus or minus FUZZ)
			*/
			int candidates[MAX_PRE_SET+1][52*DECKS]; /* CAUTION, SORTED BY [LENGTH] */
			int candidates_len[MAX_PRE_SET+1] = {0};
			int candidates_total = 0;
			int lookups_total = 0;

			for (pre_start=0; pre_start+MIN_PRE_SET <= pre_cards_len; pre_start++) {
				for (pre_len=MAX(MIN_PRE_SET,post_len-MAX_PRE_SET); pre_len<=MIN(MAX_PRE_SET,post_len-MIN_PRE_SET) && pre_len+pre_start<=pre_cards_len; pre_len++) {
					if (multiset_subtract_dist(pre_cards_sets[pre_start][pre_len], 
					                           post_cards_sets[post_start][post_len]) <= FUZZ) {
						candidates[pre_len][candidates_len[pre_len]++] = pre_start;
						candidates_total++;
					} else {
						break;  /* If this set wont "fit", then any bigger one wont either */
					}
				}
			}

/*
			printf("LENGTH\t # CANDIDATES FOR POST SLICE [%d, %d] -- %d cards\nLength\tNumber of candidates\n", post_start, post_start+post_len-1, post_len);
			for (i=0; i<=MAX_PRE_SET; i++)
				if (candidates_len[i])
					printf("[%d]\t%d\n", i, candidates_len[i]);
*/

			/* Find a pair of PRE slices that add up to POST +- FUZZ */

			for (i=MIN_PRE_SET; i<=MAX_PRE_SET; i++) {
				lookups_total += candidates_len[i] * candidates_len[post_len-i];
				for (j=0; j<candidates_len[i]; j++) {
					for (f=-FUZZ; f<=FUZZ; f++) {
						for (k=0; k<candidates_len[post_len-i+f]; k++) {
							/* Check if the sets of cards overlap */
							if ((candidates[post_len-i][k] <= candidates[i][j] + i) &&
							    (candidates[post_len-i][k] + post_len-i >= candidates[i][j]))
								continue;
	
							/* Skip symmetries */
							if (candidates[i][j] > candidates[post_len-i][k])
								continue;

							z = multiset_add_dist(pre_cards_sets[candidates[i][j]][i],
							                      pre_cards_sets[candidates[post_len-i][k]][post_len-i],
							                      post_cards_sets[post_start][post_len]);

							if (z <= FUZZ) {
								hits_total++;
								printf("  HIT: PRE[%d,%d] + PRE[%d,%d] ~= POST[%d,%d]  (FUZZ=%d)\n", 
								       candidates[i][j], i, candidates[post_len-i][k], post_len-i, post_start, post_len, z);
							}
						}
					}
				}
			}

//			printf("SEEKING  post_start %03d  post_len %03d  candidates %05d  lookups %06d\n", 
//			        post_start, post_len, candidates_total, lookups_total);
		}
	}

	printf("TOTAL HITS  %d\n", hits_total);

	return 0;
}
