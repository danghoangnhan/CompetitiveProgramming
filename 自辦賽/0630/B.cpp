#include <bits/stdc++.h>
#define maxn 100000
using namespace std;
long long int testCase,length,ans;
long long int arr[maxn],H[maxn],G[maxn];
char c;
int main(){
    scanf("%lld",&testCase);
    while(testCase--){
        scanf("%lld", &length);
        ans=0;
        for(long long int i=0;i<length;i++){
            // scanf("%c", &c);
            cin>>c;
            H[i]=(int)c-48;
        }

        for (long long int i=0;i<length;i++){
            // scanf("%c", &c);
            cin>>c;
            G[i]=(int)c-48;
        }
        for(long long int i=0;i<length;i++){
            arr[i]=H[i]||G[i];
        }
        // cout<<"H ";
        // for (long long int i = 0; i < length; i++)
        // {
        //     cout << H[i];
        // }
        // cout << endl;
        // cout << "G ";
        // for (long long int i = 0; i < length; i++)
        // {
        //     cout << G[i];
        // }
        // cout << endl;
        // cout << "R1 ";
        for (long long int i = 0; i < length; i++)
        {
            cout << arr[i];
        }
        cout << endl;
        for(long long int i=0;i<length;i++){
            for(long long int j=i+1;j<length;j++){
                if ((H[i] !=  H[j] )){
                    if (((H[i] || G[j]) != arr[j]) || ((H[j] || G[i]) != arr[i]))
                        ans++;
                }
            }
        }
        printf("%lld\n",ans);
    }
    return 0;
}