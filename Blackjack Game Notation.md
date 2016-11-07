# Blackjack Game Notation

BGN is a plain text computer-processible format for recording blackjack games.
The file captures:

  1. Game rule variants
  2. All player action
  3. All cards dealt from the shoe, in order
  4. Activity at the table across multiple shoes

Hopefully this will be helpful for people studying and analyzing the game.

## Example

```
[Site "The Atlantic Club Casino Hotel"]
[Date "2016-06-02"]
[Rules "6deck splitany minbet25 maxbet100"]
SHOE
Bharry50B50^6d^kh^td^4s^3d*ahSS
```

## Tag pairs

The file must begin with *tag pairs*. These three must be present, and in the
order here; but you may add more. Tags pairs come before any other content in
the file.

 1. `Site`: the location of the event, like `New York City, NY USA`
 2. `Date`: the starting date of the game, in YYYY-MM-DD format ("ISO 8601"),
    or use `??` if unknown
 3. `Rules`: a space-separated collection of rule variations (see
    [Blackjack Rules](Blackjack Rules.md) for detail)
 4. *Other*: you may add other tag pairs for additional information as needed

*Include a quote (`"`) in the value by escaping it as (`\"`). Include a backslash
(`\`) as (`\\`).*

## Comments

Any line starting with a semicolon (`;`) is a comment and will be ignored by
automated programs interpreting a BGN file.

## Players names

You may identify players that enter and exit the game with a unique name. Names
are any quantity of letters (a to z). We record players ("`B`etters") like
`Bharry`. If you don’t track players, you can also just use `B` for those
players.

## Shoe

When a new shoe is dealt, enter one line with `SHOE`. Your file will be most
useful if it starts with a new shoe.

## Gameplay

Each round of blackjack is recorded as one (long) line of text.

### Cards

Cards are recorded like `as`, `6d`, `7h`, `td`, `jc`. Yes, suit matters. A card
is recorded immediately when dealt. The dealer's hole card is recorded with a
`*` before the card. All other cards are recorded with a `^` before the card.

### Bets

Each round starts with bets. Record bets in player order (first base to third
base) as `B[player name][currency units]`. So for example, if there are two
players and both bet 50 units of currency, this is:

    Bharry50B50

#### Insurance

*Insurance and other side bets are NOT discussed in this version of this
document. May consider notating with `I` / `N`.*

### Initial hand

When bets are done, the dealer deals an initial hand. We know the order cards
will be dealt (first base to third base then dealer), so simply record each
dealt card.

*Note: a dealer's upcard is supposed to be the first card they deal themselves.
But if they accidentally use their second card as the upcard then mark that one
with the asterisk.*

    ^6d^kh^td^4s^3d*ah

### Surrender

*Surrender and other side bets are NOT discussed in this version of this
document. May consider notating with `U` / `N`.*

### Player action

Player action starts from first base and and continues to third base. Each
player action is recorded with notation below.

 - Stand / bust

    * `S` ends the current hand

 - `H` — Hit (record the card dealt, if bust then record `S`)

    * `H^5d` hit a five of diamonds
    * `H^10dS` hit and then busted ("stand")

 - `D[currency units]` — Double down (record the card dealt then `S`)

    * `D10^3dS` doubled down for 10 units and got `3d`

 - `P` — Split

    * One of the two cards will create a new hand, record that card again
    * Continue play with this hand, which will necessarily end with a stand
    * Repeat this process for the other card
    * `P^ad^kdS^ad^kdS` a really lucky split
    * `P^ad^4sH^kdS^ad^3hH^9cS` a double busted split
    * `P^ad`...`S^ah^acP`... respliting an ace
