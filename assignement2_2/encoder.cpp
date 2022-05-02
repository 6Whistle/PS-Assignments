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
            if(temp)
                delete temp;
        }
    }

    Node* getRoot(){ return root; }
    void setRoot(Node* i_root){root = i_root; }

    //Making each character's BST using priority queue
    void makeTree(map<char, int> input){
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

    //Making BST using priority queue
    void makeTree(map<char, pair<int, map<char, int> > > input){
        priority_queue<Node *, vector<Node *>,  Node> pq;   //priority queue(sort : greater)
        //push all leaf data in queue
        for(auto i : input){
            Node *temp = new Node(i.first, i.second.first);
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
    map<char, map<char, pair<uint8_t, bitset<16> > > > charTable;     //Context adaptive Huffman table

    public :
    Huffman(){};
    //Make table using BST
    void makeHuffmanTable(Node* node, bitset<16> pattern, uint8_t size){
        if(node->getChar())         //if current node is leaf, save leaf's data in Huffman table
            table[node->getChar()] = make_pair(size, pattern);

        size++;     //pattern's size
        if(node->getLeft())
            makeHuffmanTable(node->getLeft(), pattern, size);      //pattern add 0
        if(node->getRight())
            makeHuffmanTable(node->getRight(), pattern.set(16 - size, 1), size);   //pattern add 1
    }

    //Make each character's table using BST
    void makeCharHuffmanTable(char ch, Node* node, bitset<16> pattern, uint8_t size){
        if(node->getChar())         //if current node is leaf, save leaf's data in Huffman table
            charTable[ch][node->getChar()] = make_pair(size, pattern);

        size++;     //pattern's size
        if(node->getLeft())
            makeCharHuffmanTable(ch, node->getLeft(), pattern, size);      //pattern add 0
        if(node->getRight())
            makeCharHuffmanTable(ch, node->getRight(), pattern.set(16 - size, 1), size);   //pattern add 1
    }

    //Make Huffman table and context adaptive Huffman table
    void makeTable(map<char, pair<int, map<char, int> > > prob){
        Tree bst;
        map<char, Tree *> char_bst;
        bitset<16> pattern = 0;
        uint8_t size = 0;

        //Make Tree
        bst.makeTree(prob);
        for(auto i : prob){
            char_bst[i.first] = new Tree();
            char_bst[i.first]->makeTree(i.second.second);
        }

        //Write Table using Tree
        this->makeHuffmanTable(bst.getRoot(), pattern, size);      
        for(auto i : char_bst){
            pattern.reset();
            size = 0;
            this->makeCharHuffmanTable(i.first, char_bst[i.first]->getRoot(), pattern, size);
        }

        //delete Tree
        for(auto i : prob)
            delete char_bst[i.first];

    }


    //Print Huffman table's data
    void printTable(){
        for(auto i : table){
            cout << "Table >> " << i.first << " : size-" << (unsigned int)(i.second.first) << ", pattern-" << i.second.second
                 << ", number-" << charTable[i.first].size() << endl;
            for(auto j : charTable[i.first])
                cout << j.first << " : size-" << (unsigned int)(j.second.first) << ", pattern-" << j.second.second << "\t";
            cout << endl << endl;
        }   
    }

    void writeTableFile(const char* table_file_path){
        ofstream write_table_file, write_adaptive_table_file;
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

            /* input Character table */
            for(auto j : charTable[i.first]){
                temp = (unsigned int)(j.first) << 8;       //temp = [character|size|pattern]
                temp = (temp | (unsigned int)(j.second.first)) << 16;
                temp = temp | (unsigned int)(j.second.second.to_ulong());
                t_size = 16 + (int)(j.second.first);                //temp's size

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
            }
        }
        unsigned int temp = 127 << 24;      //end of table insert
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

    }

    int AddBuffer(ofstream &write_table_file, unsigned int buf, int max, unsigned int temp, int t_size){
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
        return max;
    }

    //Write table file and encode input file
    void encoding(const char* input_file_path, const char* code_file_path){
        ofstream write_table_file, write_file;
        ifstream read_file;
        unsigned int buf = 0;       //32 bits buffer
        int max = 0;                //buffer's current size

        //encode input file
        read_file.open(input_file_path);        //input file
        write_file.open(code_file_path, ios::binary | ios::trunc);      //write encoding file new and binary mode
        char ch_before = 0;
        char ch = 0;                //read char from input file
        buf = 0;                    //32 bit buffer
        max = 0;                    //buffer's current size
        bool endf = false;          //Check end file

        //read 1byte and input transformed pattern using Huffman table
        if(read_file.get(ch).eof())
            return;
        buf = table[ch].second.to_ulong() << 16;
        max = table[ch].first;

        //read and input transformed pattern using Context adaptive huffman table
        while(true){
            unsigned int temp;
            unsigned int t_size;
            ch_before = ch;         //before character
            read_file.get(ch);      //read character
            if(read_file.eof()){    //if read file is end, character = EOD & end file = true
                endf = true;
                ch = 127;
            }

            //character's table value
            temp = charTable[ch_before][ch].second.to_ulong() << 16;
            t_size = charTable[ch_before][ch].first;

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
    map<char, pair<int, map<char, int> > > next_ascii_count;
    char c, c_before;

    input_file.open("training_input.txt");      //open input_data.txt
    if(!input_file.is_open()){
        cout << "File open error" <<endl;
        return -1;
    }
    if(input_file.get(c).eof()){                        //read first character and add probability
        printf("No Data in training file");
        return -1;
    }
    (next_ascii_count[c].first)++;
    c_before = c;                                       //save before character
    while(!input_file.read(&c, 1).eof()){               //count each character's probability in train_input.txt data
        next_ascii_count[c_before].second[c]++;
        (next_ascii_count[c].first)++;
        c_before = c;                                   //save before character
    }
    next_ascii_count[c_before].second[127]++;           //add EOF
    input_file.close();

    //Make train_input
    Huffman huf;
    huf.makeTable(next_ascii_count);
    huf.printTable();
    huf.encoding("training_input.txt", "training_input_code.hbs");
}