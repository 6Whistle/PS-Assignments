#include <iostream>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>
#include <bitset>

using namespace std; 

//Huffman Decoder/Encoder
class Huffman{
    private :
    map<char, pair<uint8_t, bitset<16> > > table;       //Huffman table
    map<char, map<char, pair<uint8_t, bitset<16> > > > charTable;     //Context adaptive Huffman table

    public :
    Huffman(){};

    //Print Huffman table's data
    void printTable(){
        for(auto i : table){
            cout << "Table >> " << i.first << " : size-" << (unsigned int)(i.second.first) << ", pattern-" << i.second.second
                 << ", number-" << charTable[i.first].size() << endl;
            //print context adaptive Huffman table's data
            for(auto j : charTable[i.first])
                cout << j.first << " : size-" << (unsigned int)(j.second.first) << ", pattern-" << j.second.second << "\t";
            cout << endl << endl;
        }   
    }

    //make table using given file
    void makeTableWithFile(const char* read_table_path){
        ifstream read_table_file;
        read_table_file.open(read_table_path, ios::binary);     //read table file binary mode
        unsigned int buf = 0;       //32 bits buffer
        char input;                 //read 8 bits
        int max = 0;                //buffer's current size
        uint8_t h_size;
        while(true){
            if(max < 8){            //if buffer's size < character's size
                read_table_file.get(input);     //read character
                bitset<32> temp = input;        //buffer = [buffer|temp]
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;                       //buffer's size update
            }
            char ch = buf >> 24;                //read 8 bits from buffer
            if(ch == 127)                       //if meet EOF, break loop
                break;
            buf = buf << 8;
            max -= 8;

            if(max < 8){            //if buffer's size < pattern_size's size(8 bit)
                read_table_file.get(input);     //read character
                bitset<32> temp = input;        //buffer = [buffer|temp]
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;                       //buffer's size update
            }
            uint8_t size = (buf >> 24);         //read 8 bits from buffer
            buf = buf << 8;
            max -= 8;

            while(max < size){          //while buffer's size < pattern_size
                read_table_file.get(input);     //read character
                bitset<32> temp = input;        //buffer = [buffer|temp]
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;                       //buffer's size update
            }
            bitset<16> pattern = buf >> 16;     //read pattern size bits from buffer
            for(int i = 0; i < 16 - size; i++)
                pattern.set(i, 0);
            buf = buf << size;
            max -= size;

            table[ch] = make_pair(size, pattern);   //input data in table

            if(max < 8){            //if buffer's size < character Huffman num(8 bit)
                read_table_file.get(input);     //read character
                bitset<32> temp = input;        //buffer = [buffer|temp]
                temp <<= (24 - max);
                for(int i = 0; i < max; i++)
                    temp.set(31-i, 0);
                buf = buf | temp.to_ulong();
                max += 8;                       //buffer's size update
            }
            int ch_count = (buf >> 24);         //read 8 bits from buffer
            buf = buf << 8;
            max -= 8;

            for(int i = 0; i < ch_count; i++){
                if(max < 8){            //if buffer's size < character's size
                    read_table_file.get(input);     //read character
                    bitset<32> temp = input;        //buffer = [buffer|temp]
                    temp <<= (24 - max);
                    for(int j = 0; j < max; j++)
                        temp.set(31-j, 0);
                    buf = buf | temp.to_ulong();
                    max += 8;                       //buffer's size update
                }
                char h_ch = buf >> 24;                //read 8 bits from buffer
                buf = buf << 8;
                max -= 8;

                if(max < 8){            //if buffer's size < pattern_size's size(8 bit)
                    read_table_file.get(input);     //read character
                    bitset<32> temp = input;        //buffer = [buffer|temp]
                    temp <<= (24 - max);
                    for(int j = 0; j < max; j++)
                        temp.set(31-j, 0);
                    buf = buf | temp.to_ulong();
                    max += 8;                       //buffer's size update
                }
                h_size = (buf >> 24);         //read 8 bits from buffer
                buf = buf << 8;
                max -= 8;

                while(max < h_size){          //while buffer's size < pattern_size
                    read_table_file.get(input);     //read character
                    bitset<32> temp = input;        //buffer = [buffer|temp]
                    temp <<= (24 - max);
                    for(int j = 0; j < max; j++)
                        temp.set(31-j, 0);
                    buf = buf | temp.to_ulong();
                    max += 8;                       //buffer's size update
                }
                bitset<16> h_pattern = buf >> 16;     //read pattern size bits from buffer
                for(int j = 0; j < 16 - h_size; j++){
                    h_pattern.set(j, 0);                    
                }
                    
                buf = buf << h_size;
                max -= h_size;

                charTable[ch][h_ch] = make_pair(h_size, h_pattern);   //input data in table
            }
        }
        read_table_file.close();
    }

    //decode file using table and write that data in file
    void decoding(const char* read_code_path, const char* write_file_path){
        ifstream read_code_file;
        ofstream write_file;
        read_code_file.open(read_code_path, ios::binary);       //open encoding file binary mode
        write_file.open(write_file_path, ios::trunc);           //write file new mode

        unsigned int buf = 0;               //32 bits buffer
        char input;                         //read character
        char ch_before = 127;
        int max = 0;                        //buffer's current size
        int i = 0;                          //current pattern's size
        read_code_file.seekg(ios::beg);

        while(true){
            if(max < i){    //if buffer's size < pattern's size
                if(read_code_file.eof())    //if end of file, break loop
                    break;
                read_code_file.read(&input,1);  //read 8 bits
                bitset<32> temp = input;        //buffer = [buffer|temp]
                temp <<= (24 - max);
                for(int j = 0; j < max; j++)
                    temp.set(31-j, 0);
                buf = buf | temp.to_ulong();
                max += 8;                       //buffer's size update
            }

            //compare first character with all elements in table
            for(auto j : table){
                if(i != (int)(j.second.first))      //if pattern size is different, continue
                    continue;

                bitset<16> temp = buf >> 16;        //buffer XOR table's pattern
                for(int k = 0; k < 16 - i; k++)
                    temp.set(k,0);
                temp ^= j.second.second;

                //if same pattern is same
                if(!temp.any()){
                    if(j.first == 127){         //if end of file, return
                        write_file.close();
                        read_code_file.close();
                        return;
                    }
                    write_file << j.first;      //if not end of file, write character in file
                    buf = buf << i;
                    max -= i;                   //buffer's size update and read pattern 0
                    i = 0;
                    ch_before = j.first;
                    break;
                }
            }
            if(ch_before != 127)
                break;
            i++;
        }
        i = 0;

        //compare first character with all elements in adaptive Huffman table
        while(true){
            bool state = false;
            if(max < i){    //if buffer's size < pattern's size
                if(read_code_file.eof())    //if end of file, break loop
                    break;
                read_code_file.read(&input,1);  //read 8 bits
                bitset<32> temp = input;        //buffer = [buffer|temp]
                temp <<= (24 - max);
                for(int j = 0; j < max; j++)
                    temp.set(31-j, 0);
                buf = buf | temp.to_ulong();
                max += 8;                       //buffer's size update
            }

            //compare with all elements in table[before character]
            for(auto j : charTable[ch_before]){
                if(i != (int)(j.second.first))      //if pattern size is different, continue
                    continue;

                bitset<16> temp = buf >> 16;        //buffer XOR table's pattern
                for(int k = 0; k < 16 - i; k++)
                    temp.set(k,0);
                temp ^= j.second.second;

                //if same pattern is same
                if(!temp.any()){
                    if(j.first == 127){         //if end of file, return
                        write_file.close();
                        read_code_file.close();
                        return;
                    }
                    write_file << j.first;      //if not end of file, write character in file
                    ch_before = j.first;
                    buf = buf << i;
                    state = true;
                    max -= i;                   //buffer's size update and read pattern 0
                    i = 0;
                    break;
                }
            }
            if(state != true)
                i++;
        }
        write_file.close();
        read_code_file.close();
    }
};

int main(){
    Huffman huf;                           //Huffman table
    huf.makeTableWithFile("huffman_table.hbs");    //Make table using table file
    huf.printTable();                    //Print table
    huf.decoding("huffman_code.hbs", "output_data.txt");       //Decode file using encoding file
}