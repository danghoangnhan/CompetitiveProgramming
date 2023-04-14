#include <bits/stdc++.h>

using namespace std;
long long int testCase,x,cnt,sample,maximum;
int main(){
    cin>>testCase;
    while(testCase--){
        cin>>x;
        cnt=0;
        while(x>9){
            sample = x;
            maximum = -1;
            while(sample>0){
                maximum = max(maximum, sample % 10);
                sample/=10;
            }
        x-=maximum;
        cnt++;
        }
        if(x>0){
            cnt++;
        }
        cout<<cnt<<endl;
    }
    return 0;
}