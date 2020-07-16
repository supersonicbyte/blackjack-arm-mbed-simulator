/**
 * Ugradbeni sistemi - Blackjack
 * ETF Sarajevo 2020g.
 */

#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include <vector>
#include <ctime>
#include <algorithm>
#include <random>
enum { MENU,
    HIGHSCORE,
    ABOUT,
    BET,
    GAME,
    EXIT};

int STATUS = MENU;

/**
 * Implementacija blackjack game klasa
 * 
 */

enum Suit { CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES };
class Card {
    char value;
    Suit suit;

public:
    Card(char value, Suit suit)
    {
        this->value = value;
        this->suit = suit;
    }
    char getValue()
    {
        return value;
    }
    Suit getSuit()
    {
        return suit;
    }
    int getValueInt()
    {
        if (value >= '2' && value <= '9') {
            return (int)value - '0';
        }
        switch (value) {
        case 'T':
            return 10;
        case 'A':
            return 1;
        case 'J':
            return 10;
        case 'Q':
            return 10;
        case 'K':
            return 10;
        }
        return -1; // ilegalna vrijednost
    }
};
int myRandomGenerator(int j) {
   return rand() % j;
}
class Deck {
    std::vector<Card> cards;

public:
    Deck()
    {
        init();
    }
    void init()
    {
        cards.clear();
        Suit suits[] = { CLUBS, DIAMONDS, HEARTS, SPADES };
        char values[] = { 'A', '2', '3', '4',
            '5', '6', '7', '8',
            '9', 'T', 'J', 'Q', 'K' };
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 13; ++j)
                cards.push_back(Card(values[j], suits[i]));
        }
    }
    void shuffle()
    {
        if (cards.size() == 0)
            return;
         std::random_device rd;
         std::mt19937 g(rd());
         std::shuffle(cards.begin(), cards.end(), g);
    }
    Card draw()
    {
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
    bool empty()
    {
        return cards.empty();
    }
};

enum GAMESTATUS { LOSE,
    PLAYING,
    WIN,
    DRAW };
class Blackjack {
    int budget;
    int bet;
    std::vector<Card> playerCards;
    std::vector<Card> dealerCards;
    Deck deck;
    GAMESTATUS status;

public:
    Blackjack()
    {
        init();
    }
    void init()
    {
        status = PLAYING;
        budget = 1000;
        bet = 5;
        deck.init();
        deck.shuffle();
        playerCards.clear();
        dealerCards.clear();
        dealPlayer();
        dealPlayer();
        drawDealer();
        drawDealer();
    }
    void drawDealer()
    {
        dealerCards.push_back(deck.draw());
    }
    void dealPlayer()
    {
        playerCards.push_back(deck.draw());
    }
    std::vector<Card> getDealerCards()
    {
        return dealerCards;
    }
    std::vector<Card> getPlayerCards()
    {
        return playerCards;
    }
    int getBudget()
    {
        return budget;
    }
    void setBudget(int budget)
    {
        this->budget = budget;
    }
    bool isValidBet(int bet)
    {
        return bet <= budget;
    }
    void setBet(int bet)
    {
        if (isValidBet(bet)) {
            this->bet = bet;
        }
    }

    GAMESTATUS getStatus()
    {
        return status;
    }
    void setStatus(GAMESTATUS s)
    {
        status = s;
    }
    int highSum(std::vector<Card> cards)
    {
        int sum = 0;
        bool firstAce = true;
        for (int i = 0; i < cards.size(); ++i) {
            if (cards[i].getValue() == 'A' && firstAce) {
                sum += 11;
            }
            else {
                sum += cards[i].getValueInt();
            }
        }
        return sum;
    }
    int lowSum(std::vector<Card> cards)
    {
        int sum = 0;
        for (int i = 0; i < cards.size(); ++i) {
            sum += cards[i].getValueInt();
        }
        return sum;
    }
    bool isBust(std::vector<Card> cards)
    {
        int low = lowSum(cards);
        if (low > 21) {
            return true;
        }
        return false;
    }
    int handValue(std::vector<Card> cards)
    {
        int high = highSum(cards);
        int low = lowSum(cards);
        if (high <= 21)
            return high;
        return low;
    }

    bool isBlackjack()
    {
        if (handValue(playerCards) == 21)
            return true;
        return false;
    }

    void pay()
    {
        budget += 2 * bet;
    }

    void newRound()
    {
        status = PLAYING;
        bet = 5;
        deck.init();
        deck.shuffle();
        playerCards.clear();
        dealerCards.clear();
        dealPlayer();
        dealPlayer();
        drawDealer();
        drawDealer();
    }

    void draw()
    {
        budget += bet;
    }

    void stand()
    {
        while (handValue(getDealerCards()) < 15) {
            drawDealer();
        }
        if (isBust(getDealerCards())) {
            setStatus(WIN);
            return;
        }
        if (handValue(playerCards) > handValue(dealerCards)) {
            setStatus(WIN);
        }
        else if (handValue(playerCards) == handValue(dealerCards)) {
            setStatus(DRAW);
        }
        else {
            setStatus(LOSE);
        }
    }
};

/**
 * Bitmape za znakove. Nažalost zbog ograničenja online simulatora nije bilo
 * moguće implementirati iscrtavanje svih karata pomoću bitmapa. Naime, problem
 * je bio u nedostatku memorije s obzirom da je kod koji uključuje nizove karata
 * bio predugačak i nije se uspio slati na server. (PayloadTooLargeError)
 * 
 */

/*
0 -> clubs
1 -> diamonds
2 -> hearts
3 -> spades
*/
const uint8_t suitsBmp[4][676] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/**
 * Implementacija funkcija za rad sa displejom
 * 
 */

/* globalne varijable */
Blackjack game;

TS_StateTypeDef TS_State = { 0 };
InterruptIn upButton(p5);
InterruptIn downButton(p7);
InterruptIn selectButton(p6);
/* elementi menija */
const char* menuItems[] = {
    "*New game", "Highscore", "About", "Exit", // selectedItemMenu 0
    "New game", "*Highscore", "About", "Exit", // selectedItemMenu 4
    "New game", "Highscore", "*About", "Exit", // selectedItemMenu 8
    "New game", "Highscore", "About", "*Exit", // selectedItemMenu 12
};
const char* betItems[] = {
    "*5", "20", "50", "100", "500", "Deal", "Quit game", // selectedItemBet 0
    "5", "*20", "50", "100", "500", "Deal", "Quit game", // slectedItemBet 7
    "5", "20", "*50", "100", "500", "Deal", "Quit game", // 14
    "5", "20", "50", "*100", "500", "Deal", "Quit game", // 21
    "5", "20", "50", "100", "*500", "Deal", "Quit game", // 28
    "5", "20", "50", "100", "500", "*Deal", "Quit game", // 35
    "5", "20", "50", "100", "500", "Deal", "*Quit game" // 42
};

const char* gameItems[] = {
    "*Stand", "Hit", // selectedItemGame 0
    "Stand", "*Hit", // 3
};
/* lista top 3 highscore-a */
int highscores[3] = {};
int selectedItemMenu = 0;
int selectedItemBet = 0;
int selectedItemGame = 0;
/* odabrani bet */
int selectedBet = 5;

void updateHighscore()
{
    int bug = game.getBudget();
    for (int i = 0; i < 3; i++) {
        if (bug > highscores[i]) {
            for (int j = 2; j > i; j--)
                highscores[j] = highscores[j - 1];
            highscores[i] = bug;
            break;
        }
    }
}
void showMenu()
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t*)"BLACKJACK US", CENTER_MODE);
    int offset = selectedItemMenu;
    BSP_LCD_DisplayStringAt(0, 65, (uint8_t*)menuItems[offset++], CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)menuItems[offset++], CENTER_MODE);
    BSP_LCD_DisplayStringAt(10, 135, (uint8_t*)menuItems[offset++], CENTER_MODE);
    BSP_LCD_DisplayStringAt(15, 170, (uint8_t*)menuItems[offset], CENTER_MODE);
}
void showBet()
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    char bug[50] = "Budget: ";
    sprintf(bug + 8, "%d", game.getBudget());
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)bug, LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 35, (uint8_t*)"Place your bet", CENTER_MODE);
    int offset = selectedItemBet;
    BSP_LCD_DisplayStringAt(10, 65, (uint8_t*)betItems[offset++], LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 90, (uint8_t*)betItems[offset++], LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 115, (uint8_t*)betItems[offset++], LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 140, (uint8_t*)betItems[offset++], LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 165, (uint8_t*)betItems[offset++], LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 190, (uint8_t*)betItems[offset++], CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 215, (uint8_t*)betItems[offset++], CENTER_MODE);
}

void showAbout()
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t*)"BLACKJACK US - ABOUT", CENTER_MODE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 65, (uint8_t*)"Ugradbeni sistemi", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 90, (uint8_t*)"Mirza Ucanbarlic", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 115, (uint8_t*)"Tarik Horozovic", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 140, (uint8_t*)"ETF Sarajevo 2020g.", LEFT_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(15, 170, (uint8_t*)"*Back", CENTER_MODE);
}

void showHighscore()
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t*)"HIGHSCORE", CENTER_MODE);
    // konverzija int to string
    char first[12] = "1. ";
    char second[12] = "2. ";
    char third[12] = "3. ";
    sprintf(first + 3, "%d", highscores[0]);
    sprintf(second + 3, "%d", highscores[1]);
    sprintf(third + 3, "%d", highscores[2]);
    BSP_LCD_DisplayStringAt(10, 65, (uint8_t*)first, LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 100, (uint8_t*)second, LEFT_MODE);
    BSP_LCD_DisplayStringAt(10, 135, (uint8_t*)third, LEFT_MODE);
    BSP_LCD_DisplayStringAt(15, 170, (uint8_t*)"*Back", CENTER_MODE);
}

void showWarning()
{
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"Invalid bet.", RIGHT_MODE);
    BSP_LCD_DisplayStringAt(0, 125, (uint8_t*)"Please select a lower bet.", RIGHT_MODE);
}

void showCard(Card card, int position, bool dealer = false, bool facedown = false)
{
    int color;
    const int WIDTH = 40;
    const int HEIGHT = 50;
    int SUIT = card.getSuit();
    if (card.getSuit() == CLUBS || card.getSuit() == SPADES)
        color = 0; // black
    else
        color = 0xF800; // red
    int yOffset;
    if (dealer) {
        yOffset = 0;
    }
    else {
        yOffset = 115;
    }
    if (position > 4)
        yOffset += 55;
    int xOffset = (position % 5) * 45 + 5;
    if (!facedown) {
        BSP_LCD_DrawRect(xOffset, yOffset, WIDTH, HEIGHT);
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                if (suitsBmp[SUIT][i * 26 + j] == 1) {
                    BSP_LCD_DrawPixel(j + 7 + xOffset, i + 12 + yOffset, color);
                }
            }
        }
        BSP_LCD_DisplayChar(xOffset + 1, yOffset + 2, card.getValue());
    }
    else {
        BSP_LCD_DrawRect(xOffset, yOffset, WIDTH, HEIGHT);
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_FillRect(xOffset + 5, yOffset + 5, WIDTH - 10, HEIGHT - 10);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    }
}

void showGame(bool hideDealer = true)
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font12);
    int offset = selectedItemGame;
    BSP_LCD_DisplayStringAt(0, 220, (uint8_t*)gameItems[offset++], LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 220, (uint8_t*)gameItems[offset++], CENTER_MODE);
    Card card('4', CLUBS);
    std::vector<Card> playerCards = game.getPlayerCards();
    std::vector<Card> dealerCards = game.getDealerCards();
    for (int i = 0; i < dealerCards.size(); i++) {
        if (i == 0)
            showCard(dealerCards[i], i, true, hideDealer);
        else
            showCard(dealerCards[i], i, true, false);
    }
    for (int i = 0; i < playerCards.size(); i++) {
        showCard(playerCards[i], i);
    }
    if (game.isBlackjack()) {
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"Blackjack, you win! Press select", CENTER_MODE);
    }
    else if (game.isBust(game.getPlayerCards())) {
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"Bust, you lose! Press select", CENTER_MODE);
    }
    else if (game.getStatus() == WIN) {
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"You win! Press select", CENTER_MODE);
    }
    else if (game.getStatus() == LOSE) {
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"You lose! Press select", CENTER_MODE);
    }
    else if (game.getStatus() == DRAW) {
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"Draw! Press select", CENTER_MODE);
    }
}

void showLose()
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"You've lost!", CENTER_MODE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 120, (uint8_t*)"Press select to continue. ", CENTER_MODE);
}

void upButtonPressed()
{
    switch (STATUS) {
    case MENU:
        if (selectedItemMenu == 0) {
            selectedItemMenu = 12;
        }
        else {
            selectedItemMenu -= 4;
        }
        showMenu();
        break;
    case BET:
        if (selectedItemBet == 0) {
            selectedItemBet = 42;
        }
        else {
            selectedItemBet -= 7;
        }
        showBet();
        break;
    case GAME:
        if (selectedItemGame == 0) {
            selectedItemGame = 2;
        }
        else {
            selectedItemGame -= 2;
        }
        showGame();
        break;
    }
}

void downButtonPressed()
{
    switch (STATUS) {
    case MENU:
        if (selectedItemMenu == 12) {
            selectedItemMenu = 0;
        }
        else {
            selectedItemMenu += 4;
        }
        showMenu();
        break;
    case BET:
        if (selectedItemBet == 42) {
            selectedItemBet = 0;
        }
        else {
            selectedItemBet += 7;
        }
        showBet();
        break;
    case GAME:
        if (selectedItemGame == 2) {
            selectedItemGame = 0;
        }
        else {
            selectedItemGame += 2;
        }
        showGame();
        break;
    }
}

void selectButtonPressed()
{
    switch (STATUS) {
    case MENU:
        switch (selectedItemMenu) {
        case 0:
            STATUS = BET;
            game.init();
            showBet();
            break;
        case 4:
            STATUS = HIGHSCORE;
            showHighscore();
            break;
        case 8:
            STATUS = ABOUT;
            showAbout();
            break;
        case 12:
            STATUS = EXIT;
            BSP_LCD_Clear(LCD_COLOR_BLACK);
            break;
        }
        break;

    case HIGHSCORE:
        STATUS = MENU;
        showMenu();
        break;

    case ABOUT:
        STATUS = MENU;
        showMenu();
        break;

    case BET:

        switch (selectedItemBet) {
        case 0:
            selectedBet = 5;
            break;
        case 7:
            selectedBet = 20;
            break;
        case 14:
            selectedBet = 50;
            break;
        case 21:
            selectedBet = 100;
            break;
        case 28:
            selectedBet = 500;
            break;
        case 35:
            if (game.isValidBet(selectedBet)) {
                game.setBet(selectedBet);
                game.setBudget(game.getBudget() - selectedBet);
                STATUS = GAME;
                showGame();
            }
            else {
                showWarning();
            }
            break;
        case 42:
            updateHighscore();
            STATUS = MENU;
            showMenu();
            break;
        default:
            if (game.isValidBet(selectedBet))
                showBet();
            break;
        }

        break;
    case GAME:
        if (game.isBlackjack() || game.getStatus() == WIN) {
            STATUS = BET;
            game.pay();
            game.newRound();
            showBet();
        }
        else if (game.isBust(game.getPlayerCards()) || game.getStatus() == LOSE) {
            if (game.getBudget() < 5) {
                showLose();
                STATUS = ABOUT; // da ne pravimo bespotrebno novo stanje
            }
            else {
                STATUS = BET;
                game.newRound();
                showBet();
            }
        }
        else if (game.getStatus() == DRAW) {
            game.draw();
            game.newRound();
            STATUS = BET;
            showBet();
        }
        else {
            switch (selectedItemGame) {
            case 0:
                game.stand();
                showGame(false);
                break;
            case 2:
                game.dealPlayer();
                showGame();
                break;
            }
        }
        break;
    }
}

int main()
{
    printf("US Blackjack\nMirza Ucanbaric\nTarik Horozovic\nETF Sarajevo 2020g.\n");
    BSP_LCD_Init();
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    showMenu();
    upButton.rise(&upButtonPressed);
    downButton.rise(&downButtonPressed);
    selectButton.rise(&selectButtonPressed);

    while (1) {
        wait_ms(10);
    }
}