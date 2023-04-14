#include<bits/stdc++.h>
#define mp make_pair
#define pb push_back
#define pll pair<LL,LL>
#define pii pair<int,int>
#define y second
#define x first
#define LL long long 
#define sqr(x) ((x)*(x))
#define pi acosl(-1)
#define MEM(x) memset(x,0,sizeof(x))
using namespace std;
double getdis(pii &p,pii &q){
    return sqrt(sqr(p.x-q.x*1ll)+sqr(p.y-q.y*1ll));
}
void solve(){
    int n;
    scanf("%d",&n);
    pii p[2005];
    for(int i = 0;i<n;i++){
        int x,y;
        scanf("%d %d",&x,&y);
        p[i]=mp(x,y);
    }
    priority_queue<pair<double,pii>,vector<pair<double,pii>>,greater<pair<double,pii> > > pq;
    int vis[2005];
    double ans[2005];
    for(int i = 0;i<n;i++){
        for(int j=i+1;j<n;j++){
            pq.push(mp(getdis(p[i],p[j])/2,mp(i,j)));
        }
    }
    fill(vis,vis+n,0);
    while(!pq.empty()){
        auto pp=pq.top();
        pq.pop();
        double dis=pp.x;
        int a,b;
        tie(a,b)=pp.y;
        if(vis[a]&&vis[b])continue;
        if(!vis[a]&&!vis[b]){
            vis[a]=1;
            vis[b]=1;
            ans[a]=ans[b]=dis;
            for(int i = 0;i<n;i++){
                if(!vis[i]){
                    pq.push(mp(getdis(p[a],p[i])-ans[a],mp(i,a)));
                    pq.push(mp(getdis(p[b],p[i])-ans[b],mp(i,b)));
                }
            }
            
        }
        else{
            if(vis[b])swap(a,b);
            if(abs(ans[a]+dis-getdis(p[a],p[b]))<1e-8){
                ans[b]=dis;
                vis[b]=1;
                for(int i = 0;i<n;i++){
                if(!vis[i]){
                        pq.push(mp(getdis(p[b],p[i])-ans[b],mp(i,b)));
                    }
                }
            }
        }
    }
    double res=0;
    for(int i = 0;i<n;i++)res+=ans[i]*ans[i]*pi;//,printf("%lf\n",ans[i]);
    printf("%.15f\n",res);
}
int main(){
    int t=1;
   // scanf("%d",&t);
    while(t--){
        solve();
    }
}
/*
100
5
5 4
4 2
4 2 3
5 4 2 3 1
*/
