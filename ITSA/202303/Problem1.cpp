#include<iostream>
#include<set>
#include<string>
using namespace std;

int main(){
    int times;
    string inputSample;
    string result;
    cin>>times;
    while (times--)
    {
        set<char> duplicate;
        getline(cin,inputSample);
        result = "";
        for (auto character:inputSample)
        {
            if (duplicate.count(character))continue;
            result+=character;
            duplicate.insert(character);
            cout <<inputSample.size()<<" "<<result.size()<<endl;
        }
    }
    
    return 0;
}