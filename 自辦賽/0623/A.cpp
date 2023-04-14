#include <bits/stdc++.h>

using namespace std;
long long int testCase;
long long int N,a,b;
int main(){
    cin>>testCase;
    for(long long int currentTest=1;currentTest<=testCase;currentTest++) {
        cin>>N>>a>>b;
        if(min(a,b)>=N){
            cout<<"KoB"<<endl;
        }
        else{
            cout<<"LC"<<endl;
        }
    }
    return 0;
}