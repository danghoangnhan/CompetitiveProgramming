#include <bits/stdc++.h>

using namespace std;
long long int testCase;
string sample;
int main(){
    cin>>testCase;
    while(testCase--){
        cin>>sample;
        if (sample.size()%2==0)
        {
            cout << "female" << endl;
        }
        else{
            cout << "male" << endl;
        }
    }
    return 0;
}
