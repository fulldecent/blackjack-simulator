Blackjack Rules
===============

This article fully documents the rules of the game of blackjack, with particular
attention to the dealer’s protocol for placing or removing cards on the table.
We consider the insurance bet but currently ignore any other table bets (“sucker
bets”).

See [Blackjack Game Notation](Blackjack Game Notation.md) for a file format
that records Blackjack games.

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

Typical house rules (source: [Yui-Bin Chen, 1995](https://groups.google.com/forum/#!topic/rec.gambling.blackjack/9CA7HYzl80s)):

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
