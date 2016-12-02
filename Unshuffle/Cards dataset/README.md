# Card Datasets

In our research we worked with casino dealers to simulate the casino rules they were trained with. This was done outside of a casino and videotaped. We used a setup with cards and table very similar to what they usually work with.

All of the raw data is here.

TODO: Update this to the standardized blackjack game notation at https://github.com/fulldecent/blackjack-simulator/blob/master/Blackjack%20Game%20Notation.md

## Count the cards in the data file

```sh
cat Cards\ Begin.txt | sed -e 's|[<>].{1,2}[csdh]||g' -e 's/#.//' | grep -o '[123456789jqka]0\?[csdh]' | sort | uniq -c
```

♠♣♥♦

