#include <bits/stdc++.h>

using namespace std;
long long int testCase;
string sample,check="OWO";
int main(){
    cin>>testCase;
    while(testCase--){
        cin>>sample;
        long long int cnt=0;
        long long int index=0;
        for(long long int x=0;x<sample.size()-2;x++){
            if(sample[x]!='O')
                continue;
            for(long long int y =x+1;y<sample.size()-1;y++){
                if (sample[y] != 'W')
                    continue;
                for (long long int z = y + 1; z < sample.size(); z++){
                    if (sample[z] == 'O'){
                        cnt++;
                    }
                        
                }
            }
        }
        cout<<cnt<<endl;
    }
    return 0;
}