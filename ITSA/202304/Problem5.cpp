#include<math.h>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>  

using namespace std;

vector<int> split(string line,string delimiter){
    vector<int> result;
    string token;
    size_t pos = 0;

    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        result.push_back(stoi(token));
        line.erase(0, pos + delimiter.length());
    }
    result.push_back(stoi(line));
    return result;
}

int main(){
    int cases;
    freopen("./input/Problem5.in", "r", stdin);
    freopen("./output/Problem5.out","w",stdout);
    cin>>cases;
    string input;
    vector<int> stocks;
    while (cases--){
        getchar();
        getline(cin, input);
        stocks = split(input," ");
        for (int i = 0; i < stocks.size(); i++)
        {
            if ((i ==0 && stocks[i]>stocks[i+1])
            ||(i ==stocks.size()-1 && stocks[i]>stocks[i-1])
            ||(stocks[i]>stocks[i-1] && stocks[i]>stocks[i+1])){
                cout << i;
                if(i<stocks.size()-1){
                    cout<<" ";
                }
                else{
                    cout<<endl;
                }
            }
        }
    }
    return 0;
}