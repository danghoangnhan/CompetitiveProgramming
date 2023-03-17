#include<bits/stdc++.h>
#define pb push_back
using namespace std;
struct Dinic{
    static const int MXN = 200005;
    struct Edge{ int v,f,re; Edge(int a,int b,int c):v(a),f(b),re(c){}};
    int n,s,t,level[MXN];
    vector<Edge> E[MXN];
    void init(int _n, int _s, int _t){
        n = _n; s = _s; t = _t;
        for (int i=0; i<=n; i++) E[i].clear();
    }
    void add_edge(int u, int v, int f){
        E[u].pb(Edge(v,f,E[v].size()));
        E[v].pb(Edge(u,0,E[u].size()-1));//direct
    }
    bool BFS(){
        fill(level,level+MXN,-1);
        queue<int> que;
        que.push(s);
        level[s] = 0;
        while (!que.empty()){
            int u = que.front(); que.pop();
            for (auto it : E[u]){
                if (it.f > 0 && level[it.v] == -1){
                    level[it.v] = level[u]+1;
                    que.push(it.v);
                }
            }
        }
        return level[t] != -1;
    }
    int DFS(int u, int nf){
        if (u == t) return nf;
        int res = 0;
        for (auto &it : E[u]){
            if (it.f > 0 && level[it.v] == level[u]+1){
                int tf = DFS(it.v, min(nf,it.f));
                res += tf; nf -= tf; it.f -= tf;
                E[it.v][it.re].f += tf;
                if (nf == 0) return res;
            }
        }
        if (!res) level[u] = -1;
        return res;
    }
    int flow(int res=0){
        while ( BFS() ){
            res += DFS(s,2147483647);
        }
        return res;
    }
}flow;
int main(){
    int n,m;
    scanf("%d %d",&n,&m);
    vector<vector<char> > s(n);
    flow.init(n*m+2,0,n*m+1);
      int tot=0;
    for(int i = 0;i<n ;i++){
        for(int j = 0;j<m;j++){
            char c;
            scanf(" %c",&c);
            s[i].pb(c);
            if(c=='.'){
                tot++;
               // printf("%d %d\n",i,j);
                if((i+j)&1)flow.add_edge(0,i*m+j+1,1);
                else flow.add_edge(i*m+j+1,n*m+1,1);
            }
        }
    }
    int X[4]={0,0,1,-1};
    int Y[4]={1,-1,0,0};
  
    for(int i=0;i<n;i++){
        for(int j = 0;j<m;j++){
            if((i+j)&1)
            for(int k  =0;k<4;k++){
                int x=i+X[k],y=j+Y[k];
                if(x>=0&&x<n&&y>=0&&y<m){
                    flow.add_edge(i*m+j+1,x*m+y+1,1);
                }
            }
        }
    }
    printf("%d\n",tot-flow.flow());
    queue<int> q;
    q.push(0);
    
    int vis[100005]={};
    vis[0]=1;
    while(!q.empty()){
        int x=q.front();
        q.pop();
        for(auto it:flow.E[x]){
            if(it.f&&!vis[it.v]){
                q.push(it.v);
                vis[it.v]=1;
            }
        }
    }
    for(int i = 0;i<n;i++){
        for(int j = 0;j<m;j++){
            if(s[i][j]=='.'){
                if((i+j)&1){
                    if(vis[i*m+j+1])s[i][j]='C';
                }
                else{
                    if(!vis[i*m+j+1])s[i][j]='C';
                }
            }
        }
    }
    
    for(auto it:s){
        for(auto it2:it)printf("%c",it2);
        printf("\n");
    }

}
