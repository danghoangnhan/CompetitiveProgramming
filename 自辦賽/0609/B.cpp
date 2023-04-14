#include <bits/stdc++.h>
#define maxn 1000000+1
using namespace std;
long long int N, M,ans;
long long int arr[maxn];
int main()
{
    cin>>M>>N;
    ans=0;
    for(long long int i=0;i<M;i++){
        cin>>arr[i];
    }
    arr[M]=1;
    for(long long int i=0;i<M+1;i++){
        cout<<arr[i]<<" "   ;
    }
    cout<<endl;
    long long int i;
    for (i = 0; i < M; i++)
    {
       if(arr[i+1]>=arr[i]){
           ans += arr[i + 1] - arr[i];
       } else{
           ans += (N-arr[i]+1)+(arr[i+1]-1);
       }
    }
    if(i>1)
        ans += (N - arr[i]+1);

    cout<<ans<<endl;
    return 0;
}