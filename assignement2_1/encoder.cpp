#include <iostream>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <bitset>

using namespace std; 

/* This class is node of BST */
class Node{
    private :
    Node *left;         //left child node
    Node *right;        //right child node
    char ch;            //character
    int count;          //num of character(probability)

    public :
    Node(){ left = NULL; right = NULL, ch = '\0'; count = 0; }
    //When left and right node is given
    Node(Node* i_left, Node* i_right){
        left = i_left, right = i_right; ch = '\0'; 
        count = i_left->getCount() + i_right->getCount();       //left + right probability
        }
    //When current node is leaf
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

    //Compare greater
    bool operator()(Node* a, Node* b){ return (a->getCount() > b->getCount()); }
    //print BST
    void print(){ 
        if(left) left->print();
        cout << ch << " : " << count << endl;
        if(right) right->print();
    }
};

/* Binary search Tree is used for finding each character's pattern and size */
class Tree{
    private:
    Node* root;     //Root node
    public:
    Tree(){ root = NULL; }
    //Delete BST using queue
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

    Node* getRoot(){ return root; }
    void setRoot(Node* i_root){root = i_root; }

    //Making BST using priority queue
    void makeTree(map<char, int>input){
        priority_queue<Node *, vector<Node *>,  Node> pq;   //priority queue(sort : greater)
        //push all leaf data in queue
        for(auto i : input){
            Node *temp = new Node(i.first, i.second);
            pq.push(temp);
        }
    
        //Making BST
        while(pq.size() > 1){
            Node *a = pq.top();
            pq.pop();
            Node *b = pq.top();
            pq.pop();
            Node *temp = new Node(a,b);
              pq.push(temp);
        }

        //Save root
        root = pq.top();
    }

    //print BST
    void printTree(){ root->print(); } 
};

//Huffman Decoder/Encoder
class Huffman{
    private :
    map<char, pair<uint8_t, bitset<16> > > table;       //Huffman table

    public :
    Huffman(){};
    //Make table using BST
    void makeTable(Node* node, bitset<16> pattern, uint8_t size){
        if(node->getChar())         //if current node is leaf, save leaf's data in Huffman table
            table[node->getChar()] = make_pair(size, pattern);

        size++;     //pattern's size
        if(node->getLeft())
            makeTable(node->getLeft(), pattern, size);      //pattern add 0
        if(node->getRight())
            makeTable(node->getRight(), pattern.set(16 - size, 1), size);   //pattern add 1
    }

    //Print Huffman table's data
    void printTable(){
        for(auto i : table)
            cout << i.first << " : size-" << (unsigned int)(i.second.first) << ", pattern-" << i.second.second << endl;
    }

    //Write table file and encode input file
    void encoding(const char* input_file_path){
        ofstream write_table_file, write_file;
        ifstream read_file;
        unsigned int buf = 0;       //32 bits buffer
        int max = 0;                //buffer's current size

        //Write table in huffman_table.hbs
        write_table_file.open("huffman_table.hbs", ios::binary | ios::trunc);       //binary mode, clear and write
        for(auto i : table){        //Write all table element
            unsigned int temp = (unsigned int)(i.first) << 8;       //temp = [character|size|pattern]
            temp = (temp | (unsigned int)(i.second.first)) << 16;
            temp = temp | (unsigned int)(i.second.second.to_ulong());
            int t_size = 16 + (int)(i.second.first);                //temp's size

            if(max + t_size > 32){      //if buffer + temp size > 32 bits
                unsigned int b = temp >> max;       //write 32 bit buffer
                if(max == 32)
                    b = 0;
                buf = buf | b;
                for(int i = 3; i >=0 ;i--)      
                    write_table_file << (unsigned char)(buf >> i * 8);
                buf = temp << (32 - max);       //buf = temp's not written bits
                max = max + t_size - 32;        //change buffer's size
            }
            else{
                buf = buf | (temp >> max);      //buffer = [buffer|temp]
                max = t_size + max;             //change buffer's size
            }
        }
        unsigned int temp = 127 << 24;
        int t_size = 8;
        if(max + t_size > 32){      //if buffer + temp size > 32 bits
            unsigned int b = temp >> max;       //write 32 bit buffer
            if(max == 32)
                b = 0;
            buf = buf | b;
            for(int i = 3; i >=0 ;i--){            //write last element in buffer
                char a = (unsigned char)(buf >> (i*8));
                write_table_file.write(&a, 1);        
            }
            buf = temp << (32 - max);       //buf = temp's not written bits
            max = max + t_size - 32;        //change buffer's size
        }
        else{
            buf = buf | (temp >> max);      //buffer = [buffer|temp]
            max = t_size + max;             //change buffer's size
        }
        for(int i = 3; i >= 0; i--){            //write last element in buffer
            char a = (unsigned char)(buf >> (i*8));
            write_table_file.write(&a, 1);        
        }
        write_table_file.close();

        //encode input file
        read_file.open(input_file_path);        //input file
        write_file.open("huffman_code.hbs", ios::binary | ios::trunc);      //write encoding file new and binary mode
        char ch = 0;                //read char from input file
        buf = 0;                    //32 bit buffer
        max = 0;                    //buffer's current size
        bool endf = false;          //Check end file

        while(true){
            unsigned int temp;
            unsigned int t_size;

            read_file.get(ch);      //read character
            if(read_file.eof()){    //if read file is end, character = EOD & end file = true
                endf = true;
                ch = 127;
            }

            //character's table value
            temp = table[ch].second.to_ulong() << 16;
            t_size = table[ch].first;

            //if buffer and temp size > 32 bit
            if(max + t_size > 32){
                unsigned int b = (temp >> max);     //write 32 bits buffer
                if(max == 32)
                    b = 0;
                buf = buf | b;
                for(int i = 3; i >= 0; i--){
                    char a = (unsigned char)(buf >> (i*8));
                    write_file.write(&a, 1);
                }

                buf = temp << (32 - max);       //buffer = temp's not written value
                max = max + t_size - 32;        //change buffer's size
                if(endf == true)                //if end of file, break loop
                    break;
            }
            else{
                buf = buf | temp >> max;        //buffer = [buffer|temp]
                max = max + t_size;             //change buffer's size
                if(endf == true)                //if end of file, break loop
                    break;               
            }
        }
        for(int i = 3; i >= 0; i--){            //write last element in buffer
            char a = (unsigned char)(buf >> (i*8));
            write_file.write(&a, 1);
        }
        read_file.close();          //close read_file and write_file
        write_file.close();
    }
};

int main(){
    ifstream input_file;
    input_file.open("test_input3.txt");      //open input_data.txt
    map<char, int> ascii_count;             //character's probability

    if(!input_file.is_open()){
        cout << "File open error" <<endl;
        return -1;
    }
    
    while(!input_file.eof()){               //count each character's probability in input_data
        char c;
        input_file.get(c);
        ascii_count[c]++;
    }
    ascii_count[127]++;                     //EOD character add
    input_file.close();

    Tree t;                                 //BST using character's probability
    t.makeTree(ascii_count);
    //t.printTree();                        //print BST

    Huffman huf;                            //Huffman table
    bitset<16> pattern = 0;
    uint8_t size = 0;
    huf.makeTable(t.getRoot(), pattern, size);      //Make table using BST
    huf.printTable();                     //Print table           
    huf.encoding("test_input3.txt");         //Make table file and encoding file
}