#include <bits/stdc++.h>
#define  maxn 4000+5
using namespace std;
long long int testCase;
long long int n,k;
long long int offset[maxn];
vector<int>G[26];
string sample;
int main(){
    cin>>testCase;
    for(long long int currentCase=1;currentCase<=testCase;currentCase++){
        cin>>n>>k;
        memset(offset,0, sizeof(offset));
        for(long long int i=0;i<26;i++){G[i].clear();}
        cin>>sample;
        long long int firstIndex=0;
        for(long long int i=sample.length()-1;i>=0;i--){

            long long int asci=(sample[i]-97);
            //cout<<asci<<endl;
            G[asci].push_back(i);
        }
        for(long long int i=0;i<26;i++){

            //cout<<sample<<endl;
            if(sample.size()==0){break;}
            if (G[i].empty())
            {
                if(i==25){
                    long long int first=0;
                    for(first;first<sample.size();first++){
                        if(offset[first]==0){
                            offset[first]=1;
                            G[first].pop_back();
                            break;
                        }
                    }

                }
                else{
                    continue;
                }

            }else{
               
                long long int index = G[i].back();
                G[i].pop_back();
                offset[index] = 1;
                k--;
                n--;
                if(n==0||k==0) break;
            }
            
        }
        if (n == 0)
        {
            cout << "-1" << endl;
        }
        else
        {
            for (long long int i = 0; i < sample.length(); i++){
                if (offset[i] == 0){
                    printf("%c", sample[i]);
                }
            }
            cout << endl;
        }
    }
    return 0;
}