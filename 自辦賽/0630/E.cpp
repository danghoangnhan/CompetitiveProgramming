#include <bits/stdc++.h>
using namespace std;
long long int testCase,length;

int main(){
    cin>>testCase;
    while(testCase--){
        cin>>length;
        if(length%2!=0||length==0){
            cout<<0<<endl;
        }
        else{
            cout<<length<<endl;
        }
    }
    return 0;
}