#include<bits/stdc++.h>
#define LL long long
#define pb push_back
#define mp make_pair
#define x first
#define y second
#define pii pair<int,int>
using namespace std;
void solve(){
    int p,q,r;
    scanf("%d %d %d",&p,&q,&r);
    pii x=mp(3*p+r-2*q,2*(p+r));
    pii y=mp(2*q+p-r,2*(p+r));
    int gcd=__gcd(x.x,x.y);
    x.x/=gcd;
    x.y/=gcd;
    if(x.y<0)x.x=-x.x,x.y=-x.y;
    gcd=__gcd(y.x,y.y);
    y.x/=gcd;
    y.y/=gcd;
    if(y.y<0)y.x=-y.x,y.y=-y.y;
    if(x.x<=0||x.x>=x.y||y.x<=0||y.x>=y.y){
        printf("-1\n");
    }
    else{
        printf("%d/%d %d/%d\n",x.x,x.y,y.x,y.y);
    }
}
int main(){
    int t=1;
    int T=1;
    while(t--){
       // printf("Case #%d: ",T++);
        solve();
    }
}