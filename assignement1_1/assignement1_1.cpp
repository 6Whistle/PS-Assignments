#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <map>

using namespace std;

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
        cout << c;
    }

    for(auto i = ascii_count.begin(); i != ascii_count.end(); i++)
        cout << (int)(i->first) << " : " <<  i->second << endl;
     
}