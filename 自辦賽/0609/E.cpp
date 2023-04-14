#include <bits/stdc++.h>
#define maxn 100000
long long int number;
using namespace std;
long long int arr[maxn];
long long int ans[maxn];
int main(){
    scanf("%lld",&number);
        for(long long int i=0;i<number;i++){
            scanf("%lld",&arr[i]);
        }
        for(long long int i=0;i<number;i++){
            ans[i] = 0;
            long long int sample =0;
                for(long long int j=i+1;j<number;j++){
                    if(arr[j]<arr[i])
                        sample=j;
                }
            if(arr[sample]<arr[i]&&sample>i)ans[i]=sample-i;
        }
        for(long long int i=0;i<number;i++){
            if(i>0){
                cout<<" ";
            }
           cout<<ans[i]; 
        }
        cout<<endl;
    return 0;
}