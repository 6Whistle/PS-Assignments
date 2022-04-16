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

    Node* getRoot(){ return root; }
    void setRoot(Node* i_root){root = i_root; }

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
    map<char, pair<uint8_t, bitset<16> > > table;

    public :
    Huffman(){};
    void makeTable(Node* node, bitset<16> pattern, uint8_t size){
        if(node->getChar())
            table[node->getChar()] = make_pair(size, pattern);

        size++;
        if(node->getLeft())
            makeTable(node->getLeft(), pattern, size);
        if(node->getRight())
            makeTable(node->getRight(), pattern.set(16 - size, 1), size);
    }

    void printTable(){
        for(auto i : table)
            cout << i.first << " : size-" << (unsigned int)(i.second.first) << ", pattern-" << i.second.second << endl;
    }

    void encoding(char* input_file_path){
        ofstream write_table_file, write_file;
        ifstream read_file;
        unsigned int buf = 0;
        int max = 0;

        write_table_file.open("huffman_table.hbs", ios::binary | ios::trunc);
        for(auto i : table){
            unsigned int temp = (unsigned int)(i.first) << 8;
            temp = (temp | (unsigned int)(i.second.first)) << 16;
            temp = temp | (unsigned int)(i.second.second.to_ulong());
            int t_size = 16 + (int)(i.second.first);

            if(max + t_size > 32){
                unsigned int b = temp >> max;
                if(max == 32)
                    b = 0;
                buf = buf | b;
                for(int i = 3; i >=0 ;i--)
                    write_table_file << (unsigned char)(buf >> i * 8);
                buf = temp << (32 - max);
                max = max + t_size - 32;
            }
            else{
                buf = buf | (temp >> max);
                max = t_size + max;
            }
        }
        for(int i = 3; i >= 0; i--)
            write_table_file << (unsigned char)(buf >> i * 8);
        write_table_file.close();

        read_file.open(input_file_path);
        write_file.open("huffman_code.hbs", ios::binary | ios::trunc);
        char ch = 0;
        buf = 0;
        max = 0;
        bool endf = false;

        while(true){
            unsigned int temp;
            unsigned int t_size;


            read_file.get(ch);
            if(read_file.eof()){
                endf = true;
                ch = 127;
            }

            temp = table[ch].second.to_ulong() << 16;
            t_size = table[ch].first;

            if(max + t_size > 32){
                unsigned int b = (temp >> max);
                if(max == 32)
                    b = 0;
                buf = buf | b;
                for(int i = 3; i >= 0; i--){
                    char a = (unsigned char)(buf >> (i*8));
                    write_file.write(&a, 1);
                }

                buf = temp << (32 - max);
                max = max + t_size - 32;
                if(endf == true){
                    for(int i = 3; i >= 0; i--){
                        char a = (unsigned char)(buf >> (i*8));
                        write_file.write(&a, 1);
                    }
                    break;
                }
            }
            else{
                if(read_file.eof()){
                    temp = temp;
                }
                buf = buf | temp >> max;
                max = max + t_size;
                if(endf == true){
                    for(int i = 3; i >= 0; i--){
                        char a = (unsigned char)(buf >> (i*8));
                        write_file.write(&a, 1);
                    }     
                    break;               
                }
            }
        }
        read_file.close();
        write_file.close();
    }


    void makeTableWithFile(char* read_table_path){
        ifstream read_table_file;
        read_table_file.open(read_table_path, ios::binary);
        unsigned int buf = 0;
        char input;
        int max = 0;
        while(true){
            if(max < 8){
                if(read_table_file.eof())
                    break;
                read_table_file.get(input);
                bitset<32> temp = input;
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;
            }
            char ch = buf >> 24;
            buf = buf << 8;
            max -= 8;

            if(max < 8){
                read_table_file.get(input);
                bitset<32> temp = input;
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;
            }
            uint8_t size = (buf >> 24);
            buf = buf << 8;
            max -= 8;

            while(max < size){
                read_table_file.get(input);
                bitset<32> temp = input;
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;
            }
            bitset<16> pattern = buf >> 16;                        
            for(int i = 0; i < 16 - size; i++)
                pattern.set(i, 0);
            buf = buf << size;
            max -= size;

            table[ch] = make_pair(size, pattern);
            cout << ch << " :  size - " << (int)size << " , pattern - " << pattern <<endl;
        }
        read_table_file.close();
    }

    void decoding(char* read_code_path, char* write_file_path){
        ifstream read_code_file;
        ofstream write_file;
        read_code_file.open(read_code_path, ios::binary);
        write_file.open(write_file_path, ios::trunc);

        unsigned int buf = 0;
        char input;
        int max = 0;
        int i = 0;
        read_code_file.seekg(ios::beg);

        while(true){
            i++;

            if(max < i){
                if(read_code_file.eof())
                    break;
                read_code_file.read(&input,1);
                bitset<32> temp = input;
                temp <<= (24 - max);
                for(int j = 0; j < max; j++)
                    temp.set(31-j, 0);
                buf = buf | temp.to_ulong();
                max += 8;
            }

            for(auto j : table){
                bitset<16> temp = buf >> 16;
                for(int k = 0; k < 16 - i; k++)
                    temp.set(k,0);
                temp ^= j.second.second;
                if((i == (int)(j.second.first)) && !(temp.any())){
                    if(j.first == 127){
                        write_file.close();
                        read_code_file.close();
                        return;
                    }
                    write_file << j.first;
                    buf = buf << i;
                    max -= i;
                    i = 0;
                    break;
                }
            }

        }
        write_file.close();
        read_code_file.close();
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
    ascii_count[127]++;
    input_file.close();

    Tree t;
    t.makeTree(ascii_count);
    t.printTree();

    Huffman huf;
    bitset<16> pattern = 0;
    uint8_t size = 0;
    huf.makeTable(t.getRoot(), pattern, size);
    huf.printTable();
    huf.encoding("input_data.txt");

    Huffman read;
    read.makeTableWithFile("huffman_table.hbs");
    read.printTable();
    read.decoding("huffman_code.hbs", "output_data.txt");

}