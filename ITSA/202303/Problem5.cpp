#include<iostream>
#include<vector>
using namespace std;
int main(){
    vector<int> ways(21);
    int times,bags;
    cin>>times;
    ways[1]=1;
    ways[2]=2;
    for (int i = 3; i < 21; i++)
    {
        ways[i]=ways[i-1]+ways[i-2];
    }
    while (times--)
    {
        cin>>bags;
        cout<<ways[bags]<<endl;        
    }
    
    return 0;

}