#include<bits/stdc++.h>
#define LL long long
#define pb push_back
#define pll pair<LL,LL>
#define mp make_pair
#define x first
#define y second
#define pii pair<int,int>
using namespace std;
double dp[12][12];
double p1,p2;
double DP(int a,int b){
    if(a==11)return 1;
    if(b==11)return 0;
    if(a==10&&b==10){
        return p1*(1-p2)/(p1*(1-p2)+(1-p1)*p2);
    }
    if(dp[a][b]>0)return dp[a][b];
    if((a+b)%4<=1)
    dp[a][b]=DP(a+1,b)*p1+DP(a,b+1)*(1-p1);
    else
    dp[a][b]=DP(a+1,b)*(1-p2)+DP(a,b+1)*p2;
    //printf("%d %d %lf\n",a,b,dp[a][b]);
    return dp[a][b];
}
double dp2[12][12];
double v1,v2;
double DP2(int a,int b){
    if(a==4)return 1;
    if(b==4)return 0;
    if(dp2[a][b]>0)return dp2[a][b];
    if((a+b)%2){
        dp2[a][b]=DP2(a+1,b)*v2+DP2(a,b+1)*(1-v2);
    }
    else{
        dp2[a][b]=DP2(a+1,b)*v1+DP2(a,b+1)*(1-v1);
    }
    return dp2[a][b];
}
void solve(){
    scanf("%lf %lf",&p1,&p2);
    for(int i = 0;i<12;i++){
        for(int j = 0;j<12;j++){
            dp[i][j]=-1;
        }
    }
    v1=DP(0,0);
    swap(p1,p2);
    for(int i = 0;i<12;i++){
        for(int j = 0;j<12;j++){
            dp[i][j]=-1;
            dp2[i][j]=-1;
        }
    }
    v2=(1-DP(0,0));
    printf("%.100lf\n",DP2(0,0));
}
int main(){
    int t=1;
    scanf("%d",&t);
    while(t--){
        solve();
    }
}
/*
 
*/