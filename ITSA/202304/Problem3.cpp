#include <string>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;
class EncodeDecode{
public:
    EncodeDecode(int length){
        this->mapping = new vector<vector<char>>(length);
        for (int i = 0; i < length; i++) {
            (*mapping)[i].resize(length);
        }
    }
    EncodeDecode() {
        delete this->mapping;
    }
    void readEncode(string inputValue){
        for(int i=0;i<inputValue.size();i++){
            int row = i/this->mapping->size();
            int column = i%this->mapping->size();
            (*mapping)[row][column] = inputValue[i];
        }
    }
    void readDecode(string inputValue){
        for(int i=0;i<inputValue.size();i++){
            int column = i/this->mapping->size();
            int row  = i%this->mapping->size();
            (*mapping)[row][column] = inputValue[i];
        }
    }
    string showEncode(){
        string result = "";
        for (int i = 0; i < mapping->size(); i++){
            for (int j = 0; j < (*mapping)[i].size(); j++){
                result+=(*mapping)[i][j];
            }
        }
        return result;
    }
    string showDecode(){
        string result = "";
        for (int i = 0; i < mapping->size(); i++){
            for (int j = 0; j < (*mapping)[i].size(); j++){
                result+=(*mapping)[j][i];
            }
        }
        return result;
    }
    void display(){
        for (int i = 0; i < mapping->size(); i++){
            vector<char> member = (*mapping)[i];
            for (int j = 0; j < member.size(); j++){
                cout<<member[j]<<" ";
            }
            cout<<endl;
        }
    }
private:
vector<vector<char>> *mapping;
};

int main(){
    string inputValue;
    freopen("./input/Problem3.in", "r", stdin);
    freopen("./output/Problem3.out","w",stdout);
    int ende,length;
    while (cin>>ende){
        cin>>length;
        getchar();
        getline(cin, inputValue,'\n');
        EncodeDecode *solve = new EncodeDecode(length);
        switch (ende){
        case 1:
            solve->readDecode(inputValue);
            // solve->display();
            cout<<solve->showEncode()<<endl;
            break; 
        case 0:
            solve->readEncode(inputValue);
            // solve->display();
            cout<<solve->showDecode()<<endl;
            break;
        }
    }   
    return 0;
}