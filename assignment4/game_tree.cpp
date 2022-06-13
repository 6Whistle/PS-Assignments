#include <iostream>
#include <time.h>
#include <vector>
using namespace std;

//Monte Carlo Tree Search Node
class Node{
    private:
    pair<int, int> vec;     //input vector
    vector<Node*> child;    //next Node*
    int count, game_count, win_rate;    //Game count and win count
    double prob;            //win probability

    public:
    //Construtor
    Node();
    Node(pair<int, int> vec, int count){
        this->vec = make_pair(vec.first, vec.second);
        this->child.clear();
        this->count = count;
        this->game_count = 0;
        this->win_rate = 0;
        this->prob = 0.5;
    }
    //Destructor
    ~Node(){
        for(auto i : child)
            delete i;
    }

    //Get
    int GetCount(void){ return count; };
    double GetProb(void){ return prob; };
    vector<Node *> GetChild(void){ return child; };
    pair<int, int> GetVec(void){ return vec; };

    //Add child Node in child vector
    void AddChild(Node *child_node){ this->child.push_back(child_node); };

    //Game Count++
    void Update_Game_Count(void){ this->game_count++; };

    //After game, Update Game count and win rate
    void Update_Win_Rate(int state){ 
        Update_Game_Count();        //Update Game count
        if(state == 1)              //Update win count
            this->win_rate++;
        this->prob = (double)win_rate / game_count;     //Update win rate
    }

    //Print Node
    void Print_Node(void) {
        for (int i = 0; i < count; i++)
            cout << "  ";
        cout << '(' << vec.first << ',' << vec.second << ") : " << prob << endl;
        for (int i = 0; i < count; i++)
            cout << "  ";
        cout << "Child {" << endl;
        for (auto i : child) i->Print_Node();

        cout << endl;
        for (int i = 0; i < count; i++)
            cout << "  ";
        cout << "}" << endl;
    }
};

//Monte Carlo Tree Search
class MCTS{
    private:
    Node* head;     //Root of tree
    vector<Node *> routine;     //Current Move Routine

    public:
    //Constructor
    MCTS(){
        head = new Node(make_pair(-1, -1), 0);
        routine.clear();
        routine.push_back(head);
    }

    //Get
    Node* GetHead(void){ return head; };

    //Clear Routine(only head exists)
    void Clear_Routine(void){
        routine.clear();
        routine.push_back(head);
    }
    
    //Add Routine
    void Add_Routine(Node* input){ routine.push_back(input); };

    //Update Node's game count and win rate in routine
    void Cal_Prob_Routine(int state){
        for(auto i : routine)           //Update
            i->Update_Win_Rate(state);
        Clear_Routine();                //Clear routine
    }

    //Update Player's input routine
    void Update_Child(int board[][3], pair<int, int> vec){
        Node *current_child = routine.back();
        int count = current_child->GetCount();

        //if Node's child isn't exist, Make children
        if(current_child->GetChild().empty()){
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++)
                    if(board[i][j] == 0)        //if board space is empty, make new block
                        current_child->AddChild(new Node(make_pair(i, j), count + 1));
            current_child->AddChild(new Node(make_pair(vec.first, vec.second), count + 1)); //make current input block Node
        }

        //routine update
        for(auto i : current_child->GetChild()){
            if(i->GetVec().first == vec.first && i->GetVec().second == vec.second){
                routine.push_back(i);
            }
        }

        return;
    }

    //Select Node which has lowest win rate
    Node *Select_Child(int board[][3]){
        Node *current_child = routine.back();       //Current Node
        int count = current_child->GetCount();      //Current Node's Depth

        //if Node's child isn't exist, Make children
        if(current_child->GetChild().empty())
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++)
                    if(board[i][j] == 0)        //if board space is empty, make new block
                        current_child->AddChild(new Node(make_pair(i, j), count + 1));
        
        Node* min_node = NULL;  //Min node
        double min_prob = 1;    //Min win rate

        //Find min win rate
        for(auto i : current_child->GetChild()){
            if(i->GetProb() <= min_prob){
                min_prob = i->GetProb();
                min_node = i;
            }
        }

        return min_node;
    }

    //Print MCTS
    void Print_MCTS(void) { head->Print_Node(); };
};

//Player(younger Brother)
class Player{
    public:
    //Constructor
    Player(){};

    //Select Random
    pair<int, int> Select(int player, int board[][3]){
        while(true){
            int row = rand() % 3;
            int cal = rand() % 3;
            if(board[row][cal] == 0){
                board[row][cal] = player;   //Check in board
                return make_pair(row, cal);
            }
        }
    }
};

//User(Me)
class User : public Player{
    public:
    MCTS mcts;      //MCTS
    
    //Select using MCTS(Override)
    pair<int, int> Select(int player, int board[][3]){
        Node* select = mcts.Select_Child(board);        //Select Min Node
        board[select->GetVec().first][select->GetVec().second] = player;    //Check in board
        mcts.Add_Routine(select);       //Add routine

        return select->GetVec();
    }
};

class Game{
    private:
    int board[3][3];        //Game board

    public:
    bool end_game;    //end game(true = end, false = not yet)
    int result;       //Result of game
    
    //Constructor
    Game(){ };

    //Clear board and game state
    void Clear_Game(void){
        //Clear board
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                this->board[i][j] = 0;

        //Clear state
        this->end_game = false;
        this->result = 0;
    }

    //Check game is end and update game state
    void Game_result(int turn){
        //Row Win case
        for(int i = 0; i < 3; i++){
            if(board[i][0] != 0 && board[i][0] == board[i][1] && board[i][1] == board[i][2]){
                end_game = true;
                result = board[i][0];
                return;
            }
        }
        //Row Win case
        for(int i = 0; i < 3; i++){
            if(board[0][i] != 0 && board[0][i] == board[1][i] && board[1][i] == board[2][i]){
                end_game = true;
                result = board[0][i];
                return;
            }
        }
        //Cross Win case
        if(board[0][2] != 0 && board[0][2] == board[1][1] && board[1][1] == board[2][0]){
            end_game = true;
            result = board[0][2];
            return;
        }
        if(board[0][0] != 0 && board[0][0] == board[1][1] && board[1][1] == board[2][2]){
            end_game = true;
            result = board[0][0];
            return;
        } 

        //Draw Case
        if(turn == 9)
            end_game = true;
        return;
    }

    //User and Player play game
    int Play(User &p1, Player &p2){
        int turn = 0;
        while(true){
            //User turn
            pair<int, int> temp1 = p1.Select(1, board); //Check in board
            turn++;
            //cout << "Turn " << turn << " : " << temp1.first << ", " << temp1.second << endl;
            Game_result(turn);      //Check game is end
            if(end_game == true)    //if game is end, break loop
                break;

            //Player turn
            pair<int, int> temp2 = p2.Select(2, board); //Check in board
            turn++;
            p1.mcts.Update_Child(board, temp2);     //MCTS update
            //cout << "Turn " << turn << " : " << temp2.first << ", " << temp2.second << endl;
            Game_result(turn);      //Check game is end
            if(end_game == true)    //if game is end, break loop
                break;
        }

        //Print Result
        // for (int i = 0; i < 3; i++) {
        //     for (int j = 0; j < 3; j++)
        //         cout << board[i][j] << " ";
        //     cout << endl;
        // }
        // cout << endl;

        //Update Routine's win rate
        p1.mcts.Cal_Prob_Routine(result);
        return result;
    }

};


int main(void){
    srand((unsigned)time(NULL));
    User user;          
    Player player;      
    Game game;          
    //Game result state
    int round = 0;
    int win = 0, lose = 0, draw = 0;

    //Get round from user
    cout << "Game round : ";
    cin >> round;

    //Play game
    for (int i = 0; i < round; i++) {
        game.Clear_Game();  //game reset
        int temp = game.Play(user, player);     //Play game
        if (temp == 0)      //draw case
            draw++;
        else if (temp == 1) //win case
            win++;
        else                //lose case
            lose++;
    }
    //Print MCTS
    //user.mcts.Print_MCTS();

    //Print Result of games
    cout << "win : " << win << ", lose : " << lose << ", draw : " << draw << ", prob : " << (double)win / round * 100 << endl;
}