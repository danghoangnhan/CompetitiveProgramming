#include <bits/stdc++.h>
using namespace std;
long long int testCase;
string input,ans;
int main(){
    cin>>testCase;
    for(long long int i=1;i<=testCase;i++){
        cin>>input;
        ans=input;
        for(long long int j=0;j<ans.size();j++){
            if (ans[j] == 'A' || ans[j] == 'E' || ans[j] == 'I' || ans[j] == 'O' || ans[j] == 'U' || ans[j] == 'Y' ||
                ans[j] == 'a' || ans[j] == 'e' || ans[j] == 'i' || ans[j] == 'o' || ans[j] == 'u' || ans[j] == 'y'){
                ans=ans.substr(0,j)+ans.substr(j+1,ans.size());
                //cout<<ans<<endl;
                j--;
            }
            else if (input[j] != '#')
            {
                //cout<<char(input[j]+32)<<endl;
                if(input[j]>=65){
                    input[j]+=32;
                }
                ans = ans.substr(0, j) + '#' + ans.substr(j, ans.size());
                j++;
                //cout << ans << endl;
            }
        }
        cout<<ans<<endl;
    }
    return 0;
}