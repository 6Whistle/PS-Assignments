#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
#include <queue>
#include <bitset>

using namespace std; 

class Node{
    private :
    Node *left;
    Node *right;
    char ch;
    int count;

    public :
    Node(){ left = NULL; right = NULL, ch = '\0'; count = 0; }
    Node(Node* i_left, Node* i_right){
        left = i_left, right = i_right; ch = '\0'; 
        count = i_left->getCount() + i_right->getCount();
        }
    Node(char i_ch, int i_count){ left = NULL; right = NULL; ch = i_ch; count = i_count; }
    ~Node(){};

    Node* getLeft(void){ return left; }
    Node* getRight(void){ return right; }
    char getChar(void){ return ch; }
    int getCount(void){ return count; }
    void setLeft(Node* i_left){ left = i_left; }
    void setRight(Node* i_right) { right = i_right; }
    void setChar(char i_ch){ ch = i_ch; }
    void setCount(int i_count){ count = i_count; }

    bool operator()(Node* a, Node* b){ return (a->getCount() > b->getCount()); }
    void print(){ 
        if(left) left->print();
        cout << ch << " : " << count << endl;
        if(right) right->print();
    }
};

class Tree{
    private:
    Node* root;
    public:
    Tree(){ root = NULL; }
    ~Tree(){ 
        queue<Node *> q;
        q.push(root);

        while(q.size()){
            Node* temp = q.front();
            q.pop();
            if(temp->getLeft())
                q.push(temp->getLeft());
            if(temp->getRight())
                q.push(temp->getRight());
            delete temp;
        }
    }

    void makeTree(map<char, int>input){
        priority_queue<Node *, vector<Node *>,  Node> pq;
        for(auto i : input){
            Node *temp = new Node(i.first, i.second);
            pq.push(temp);
        }
    
        while(pq.size() > 1){
            Node *a = pq.top();
            pq.pop();
            Node *b = pq.top();
            pq.pop();
            Node *temp = new Node(a,b);
              pq.push(temp);
        }

        root = pq.top();
    }

    void printTree(){ root->print(); }
    
};

class Huffman{
    private :
    map<char, pair<uint16_t, bitset<8> > > table;

    public :
    Huffman(){};
    void makeTable(Node* node, bitset<8> pattern, uint16_t size){
        pattern.
    }

};

int main(){
    ifstream input_file;
    input_file.open("input_data.txt");
    map<char, int> ascii_count;

    if(!input_file.is_open()){
        cout << "File open error" <<endl;
        return -1;
    }
    
    while(!input_file.eof()){
        char c;
        input_file.get(c);
        ascii_count[c]++;
    }

    Tree t;
    t.makeTree(ascii_count);


    t.printTree();
}