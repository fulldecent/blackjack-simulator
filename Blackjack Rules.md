Blackjack Rules
===============

This article fully documents the rules of the game of blackjack, with particular
attention to the dealer’s protocol for placing or removing cards on the table.
We consider the insurance bet but currently ignore any other table bets (“sucker
bets”).

We also describe a blackjack game notation file format.

Game Process
------------

[TO DO]

Deviations
----------

Dealers may deviate from the correct protocol in several ways.

[TO DO: HOW TO DOCUMENT THIS]

Rules Used by Each Casino
-------------------------

[see data file]

Blackjack Game Notation (v0.1.0)
--------------------------------

We document gameplay using a certain format of text file.

Here is an example

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[Event "Cash Game"]
[Site "The Atlantic Club Casino Hotel"]
[Date "2016-06-02"]
[Rules "6deck splitany minbet25 maxbet100"]
SHOE
2s6s10c10c10s4s6h4s
8hks3h9h2d2dkhad7sahjc
9s7h3d6c5ckc7c4h
2s10h8s2c6h8cah10s6s5h
3h3c4hqc5c2d2c7s5d5h4djh
...
SHOE
10has8hacadqdjdacacqc
9djc5s9skd2c6c2c7hjs
7d2dks7das10d3skc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Rules

The rules line is a space-separated list of rules that apply to this game, use
the exact spelling below. Contact me if you have seen other variations.

-   `8deck` — the shoe has 8 decks of cards

-   `6deck` — the shoe has 6 decks of cards

-   `4deck` — the shoe has 4 decks of cards

-   `2deck` — the shoe has 2 decks of cards

-   `1deck` — the shoe has 1 deck of cards

-   `splitany` — you can split any pair

-   `splitthreeaces` — if you split aces three times (four total hands) you may
    no longer split

-   `minbetXXX` — the minimum bet is XXX units of currency

-   `maxbetXXX` — the maximum bet is XXX units of currency

-   `doa` — doubling-down on any first two cards is allowed

-   `d10` — doubling-down is allowed only when the first two cards sum to a
    total of 10 or 11

-   `s17` — the dealer stands on soft 17 (a soft 17 hand contains an ace and can
    be treated as a total of 7 or 17)

-   `h17` — the dealer hits on soft 17

-   `das` — doubling-down-after-splitting is allowed

-   `lsr` — late-surrender is allowed

Typical house rules (source: Yui-Bin Chen, 1995):

-   Las Vegas strip:

    -   DOA, S17, no DAS, no LSR,

    -   natural blackjack paid 3:2,

    -   one card for Ace-splitting and no replitting Ace's, and

    -   replitting up to 4 hands

-   Downtown Las Vegas

    -   DOA, H17, no DAS, no LSR,

    -   natural blackjack paid 3:2,

    -   one card for Ace-splitting and no replitting Ace's, and

    -   replitting up to 4 hands

-   Atlantic City

    -   DOA, S17, DAS,

    -   natural blackjack paid 3:2,

    -   one card for Ace-splitting and no replitting Ace's, and

    -   no replitting

-   Reno

    -   D10, H17, no DAS,

    -   natural blackjack paid 3:2,

    -   one card for Ace-splitting and no replitting Ace's, and

    -   replitting up to 4 hands

[TODO: need standard acronyms for all these rules]

### Notes

Lines starting with `;` are notes, they have no interpreted meaning, put
whatever you want in there.

### Players numbers

You may track players that enter and exit the game with a unique number. Players
are named with `B` and number. If you don’t track players, you can also just use
`B` for those players.

### Shoe

When a new shoe is dealt, enter one line with `SHOE`. Preferably, your file
should start with a new shoe.

### Hands

All gameplay begins with a player action may result in cards being dealt. Each
possible action is listed below and the dealt cards are formatted as
[a-k][h,s,d,c]. Each hand is documented on one line with no spacing. Document
each card when dealt and in order.

-   `B[player number]-[units]` — Player number PLAYER\_NUMBER places ante for
    UNITS units of currency (example: `B3-25`)

-   `$` — The final bet is complete (starting cards dealt)

    -   Note a `*` before the dealer’s upcard

-   `I` — Player pays insurance

-   `N` — Player does not pay insurance

-   `U` — Player surrenders (no cards dealt, play skips to next player)

-   `R` — Player does not surrender (no cards dealt, play skips to next player)

-   `S` — Stand (no cards dealt, play skips to next hand)

-   `H` — Hit (one card dealt) — if player busts then “stand” after card dealt

-   `D` — Double down (one card dealt) then indicate “stand” after card dealt

-   `P` — Split

    -   Print the existing card (again) and then the new dealt card

    -   Continue play with this hand, which will necessarily end with a stand

    -   Repeat the process for any other splits
