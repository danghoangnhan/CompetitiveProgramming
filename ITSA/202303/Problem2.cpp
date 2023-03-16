#include <iostream>
#include<stack>
#include<string>
using namespace std;

bool checking(string sampleString){
    stack<char> mapping;
    for (auto character:sampleString){
        if (character == 'X'){mapping.emplace(character);}
        if (character == 'Y'){
            if (mapping.empty()) return false;
                mapping.pop();
            }
    }
    return mapping.empty();
    
}
int main(){
    int time;
    string sampleInput;
    cin>>time;
    
    while (time--)
    {
        cin>>sampleInput;
        string result = (checking(sampleInput)==true)?"Yes" :"No";
        cout<<result<<endl;
    }
    
    return 0;
}