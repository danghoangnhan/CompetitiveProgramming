#include <bits/stdc++.h>
using namespace std;
long long int R, C;
string arr[41][41];

int main(){
    cin>>R>>C;
    long long int cnt =0;
    for(long long int i=0;i<R;i++){
            if(i%2==0){
                for(long long int j=0;j<C;j++){
                    cout<<"X";
                }
            }
            else if(cnt%2==0){
                for(long long int j=0;j<C-1;j++){
                    cout<<".";
                }
                cout<<"X";
                cnt++;
            }else{
                cout << "X" ;
                for (long long int j = 0; j < C - 1; j++)
                {
                    cout << ".";
                }
                cnt++;
            }
        cout<<endl;
        }
    return 0;
}