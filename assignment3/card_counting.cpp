#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <ctime>
#include <list>
#define USINGDECK 3

using namespace std;

class Deck{
    private:
    int deck_count;
    int decks[4][13];

    public:
    Deck(){
        deck_count = 52 * USINGDECK;
        for(int i = 0 ; i < 4; i++)
            for(int j = 0; j < 13; j++)
                decks[i][j] = USINGDECK;
    }

    int Check_Card_Num(void){
        if ((double)deck_count / (52 * USINGDECK) < 0.2){
            deck_count = 52 * USINGDECK;
            for(int i = 0 ; i < 4; i++)
                for(int j = 0; j < 13; j++)
                    decks[i][j] = USINGDECK;
            return true;
        }
        return false;
    }

    bool Give_Card(pair<int, int> &card){
        int num, symbol;
        while(true){
            num = rand() % 13;
            symbol = rand() % 4;
            if(decks[symbol][num] > 0){
                decks[symbol][num]--;
                deck_count--;
                break;
            }
        }
        card = make_pair(symbol, num);
        return num == 0 ? true : false;
    }

    void Print_Deck(void){
        cout << "Deck State" << endl;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 13; j++)
                cout << decks[i][j] << ", ";
            cout << endl;
        }
    }
};

class Player{
    protected:
    int hasA;
    int sum;
    list<pair<int, int> > cards;

    public:
    int win, draw, lose;
    int money;
    int bet;

    Player(){ win = 0; draw = 0; lose = 0; money = 100000; }
    int getHasA(void){ return hasA; }
    int getSum(void){ return sum; }
    void setHasA(int hasA){ this->hasA = hasA; }
    void setSum(int sum){ this->sum = sum; }
    
    void Betting(void){ bet = 500; }

    void Add_Cards(Deck &deck){
        pair<int, int> temp;
        if(deck.Give_Card(temp)){
            hasA++;
            sum += 11;
        }
        else if(temp.second > 9)
            sum += 10;
        else
            sum += temp.second + 1;
        cards.push_back(temp);
    }

    void Setting_Cards(Deck &deck){
        sum = 0;
        hasA = 0;
        cards.clear();
        Add_Cards(deck);
        Add_Cards(deck);
    }

    void Show_Cards(list<pair<int, int> > &user_cards, int state){
        user_cards.clear();
        for(auto i : cards)
            user_cards.push_back(i);
        return;
    }

    int Play_Game(Deck &deck){        
        while(true){
            if(sum > 17)
                return true;
            Add_Cards(deck);
            if(sum > 21 && hasA > 0){
                sum -= 10;
                hasA--;
            }
            else if(sum > 21)
                return false;
        }
    }

    int Game_result(Player dealer){
        if(this->sum > 21 || (this->sum < dealer.sum && dealer.sum <= 21))
            return -1;
        if(dealer.sum > 21 || this->sum > dealer.sum){
            if(this->sum == 21)
                return 2;
            return 1;
        }
        return 0;
    }

    void Cal_Betting(int state){
        if(state < 0){
            money -= bet;
            lose++;
        }
        else if(state == 0)
            draw++;
        else if(state == 1){
            money += bet * 2;
            win++;
        }
        else{
            money += bet * 2.5;
            win++;
        }
    }
};

class Dealer : public Player{
    public:

    void Add_Cards(Deck &deck){
        pair<int, int> temp;
        if(deck.Give_Card(temp))
            sum += 11;
        else if(temp.second > 9)
            sum += 10;
        else
            sum += temp.second + 1;
        cards.push_back(temp);
    }

    void Show_Cards(list<pair<int, int> > &user_cards, int state){
        user_cards.clear();
        for(auto i : cards){
            if(i == cards.front() && state == true)
                continue;
            user_cards.push_back(i);
        }
        return;
    }

    int Play_Game(Deck &deck){        
        while(true){
            if(sum > 17)
                return true;
            Add_Cards(deck);
            if(sum > 21)
                return false;
        }
    }
};

class User : public Player{
    protected:
    int card_count;
    int left_card[4][13];

    public:
    bool before_win = false;
    User(){
        bet = 0;
        card_count = 0;
        for (int i = 0; i < 4; i++){
            for(int j = 0; j < 13; j++)
                left_card[i][j] = 0;
        }
    }

    void Betting(void){
        if(bet == 0)
            bet == 500;
        else if(bet == 500 && before_win == true)
            bet -= 100;
        else if(bet == 500 && before_win == false)
            bet += 100;
        else if(bet > 500){
            if(before_win == true)
                bet = 500;
            else if(bet < 1000)
                bet += 100;
        }
        else{
            if(before_win == false)
                bet = 500;
            else if(bet > 100)
                bet -= 100;
        }
    }

    void Setting_Card_Count(void){
        card_count = 0;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 13; j++)
                left_card[i][j] = 0;
        }
    }

    void Setting_Card_Count(list<pair<int, int> > dealer, list<pair<int, int> > p2){
        for(auto i : dealer){
            left_card[i.first][i.second] += 1;
            card_count++;
        }
        for(auto i : p2){
            left_card[i.first][i.second] += 1;
            card_count++;
        }
        for(auto i : cards){
            left_card[i.first][i.second] += 1;
            card_count++;
        }
    }

    int Prediction(list<pair<int, int> > dealer, list<pair<int, int> > p2){
        int used_cards[13] = { 0 };
        int used_total = 5;
        used_cards[dealer.front().second] += 1;
        for(auto i : p2)
            used_cards[i.second] += 1;
        for(auto i : this->cards)
            used_cards[i.second] += 1;
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 13; j++)
                used_cards[j] += left_card[i][j];
        used_total += this->card_count;
                
        int d_sum = dealer.front().second + 11;
        if(d_sum > 20)
            d_sum = 20;
        if(d_sum == 10)
            d_sum = 21;

        if(d_sum < 17){
            int temp = 21 - d_sum;
            int not_used_sum = 0;
            double prob;
            for(int i = 1; i < temp; temp++)
                not_used_sum += (4 - used_cards[i]);
            prob = (double)(not_used_sum)/(52 * USINGDECK - used_total);

            if(prob < 0.3)
                return false;
        }
        if(this->sum >= d_sum)
            return false;
        return true;
    }

    int Play_Game(Deck &deck, list<pair<int, int> > dealer, list<pair<int, int> > p2){        
        while(this->Prediction(dealer, p2)){
            Add_Cards(deck);
            if(sum > 21 && hasA > 0){
                sum -= 10;
                hasA--;
            }
            if(sum > 21)
                return false;
        }
        return true;
    }

    void Cal_Betting(int state){
        if(state < 0){
            money -= bet;
            before_win = false;
            lose++;
        }
        else if(state == 0){
            before_win = false;
            draw++;
        }
        else if(state == 1){
            money += bet;
            before_win = true;
            win++;
        }
        else{
            money += bet * 1.5;
            before_win = true;
            win++;
        }
    }
};



int main(void){
    srand((unsigned)time(NULL));
    User p1;
    Player p2;
    Dealer dealer;
    Deck deck;

    list<pair<int, int> > p1_cards, p2_cards, dealer_cards;
    int round = 50;
    for(int i = 0; i < round; i++){
        if(deck.Check_Card_Num())
            p1.Setting_Card_Count();
        p1.Setting_Cards(deck);
        p2.Setting_Cards(deck);
        dealer.Setting_Cards(deck);

        deck.Print_Deck();
        p1.Betting();
        p2.Betting();
    
        p2.Show_Cards(p2_cards, false);
        dealer.Show_Cards(dealer_cards, true);

        p1.Play_Game(deck, dealer_cards, p2_cards);
        p2.Play_Game(deck);
        dealer.Play_Game(deck);

        p1.Show_Cards(p1_cards, false);
        p2.Show_Cards(p2_cards, false);
        dealer.Show_Cards(dealer_cards, false);

        for(auto i : p1_cards)
            cout << i.first << '/' << i.second << ", ";
        cout << endl << p1.getHasA() << " " << p1.getSum() << endl;

        for(auto i : p2_cards)
            cout << i.first << '/' << i.second << ", ";
        cout << endl << p2.getHasA() << " " << p2.getSum() << endl;

        for(auto i : dealer_cards)
            cout << i.first << '/' << i.second << ", ";
        cout << endl << dealer.getHasA() << " " << dealer.getSum() << endl;

        p1.Cal_Betting(p1.Game_result(dealer));
        p2.Cal_Betting(p2.Game_result(dealer));
    
        p1.Setting_Card_Count(p2_cards, dealer_cards);
    }
    cout << "Player 1" << endl;
    cout << "잔액 : " << p1.money << endl;
    cout << "win : " << p1.win << endl;
    cout << "lose : " << p1.lose << endl;
    cout << "draw : " << p1.draw << endl << endl;

    cout << "Player 2" << endl;
    cout << "잔액 : " << p2.money << endl;
    cout << "win : " << p2.win << endl;
    cout << "lose : " << p2.lose << endl;
    cout << "draw : " << p2.draw << endl << endl;

    cout << "ROUND : " << round << endl;
}