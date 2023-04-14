#include<bits/stdc++.h>

/*#pragma GCC optimize("Ofast")
#pragma GCC optimize ("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")*/
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
#define MEMS(x) memset(x,-1,sizeof(x))
using namespace std;
#define N 700005
int f[10];
int mod=1e9+7;
int Find(int x){
    if(f[x]==x)return x;
    return f[x]=Find(f[x]);
}
void solve(){
    int n;
    scanf("%d",&n);
    int a[1000005];
    for(int i = 0;i<n;i++){
        scanf("%d",&a[i]);
    }
    sort(a,a+n);
    vector<LL> tmp,v;
    tmp.pb(a[0]);
    tmp.pb(a[n-1]);
    for(int i = 1;i<n-1;i++){
        if(a[i]>0){
            if(a[i-1]<=0){
                tmp.pb(a[i]);
            }
            else{
                v.pb(a[i]);
            }
        }
        else if(a[i]==0){
            if(a[i+1]==0)v.pb(a[i]);
            else tmp.pb(a[i]);
        }
        else if(a[i]<0){
            if(a[i+1]>=0){
                tmp.pb(a[i]);
            }
            else{
                v.pb(a[i]);
            }
        }
    }
    LL Max=0,Min=0;
    for(auto it:v){
        LL Ma=-1e18,Mi=1e18;
        for(auto it2:tmp){
            Ma=max(Ma,it2*it);
            Mi=min(Mi,it*it2);
        }
        Max+=Ma;
        Min+=Mi;
    }
    vector<pair<LL,pii> > edge;
    for(int i=  0;i<tmp.size();i++){
        for(int j = i+1;j<tmp.size();j++){
            edge.pb(mp(tmp[i]*tmp[j],mp(i,j)));
        }
    }
    sort(edge.begin(),edge.end());
    for(int i = 0;i<tmp.size();i++)f[i]=i;
    for(auto it:edge){
        int a=Find(it.y.x),b=Find(it.y.y);
        if(a!=b){
            f[a]=b;
            Min+=it.x;
        }
    }
    reverse(edge.begin(),edge.end());
    for(int i = 0;i<tmp.size();i++)f[i]=i;
    for(auto it:edge){
        int a=Find(it.y.x),b=Find(it.y.y);
        if(a!=b){
            f[a]=b;
            Max+=it.x;
        }
    }
    printf("%lld %lld\n",(Min%mod+mod)%mod,(Max%mod+mod)%mod);
    //vector<int> v;
    
}
int main(){
    int t=1;
    //scanf("%d",&t);
    while(t--){
        //cout<<"Case "<<T++<<":\n";
        solve();
    }
}
/*
1 6 6
2 7 4
3 5 2
4 3 1
5 2 5
6 4 0
7 1 3
*/
/*
100
5
5 4
4 2
4 2 3
5 4 2 3 1
*/