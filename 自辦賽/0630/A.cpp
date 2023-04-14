#include <bits/stdc++.h>
#define maxn 20
using namespace std;
long long int testCase,number;
long long int arr[maxn][maxn];
int main(){
    cin>>testCase;
    while(testCase--){
        cin>>number;
        memset(arr,0,sizeof(arr));
        for(long long int i=0;i<number;i++){
            for(long long int j=0;j<number;j++){
                if(i==0||j==0)
                    arr[i][j]=1;
                else
                    arr[i][j]=arr[i-1][j]+arr[i][j-1];
            }
        }
        cout<<arr[number-1][number-1]<<endl;
    }
    return 0;
}