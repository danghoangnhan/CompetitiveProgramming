#include<bits/stdc++.h>
using namespace std;
vector<int> v;
vector<int> ans;
void dfs(int x,int &m){
    if(x==v.size())return;
    if(m==0)return;
    dfs(x+1,m);
    if(m==0)return;
    ans.push_back(v[x]);
    m--;
    dfs(x+1,m);
}
int vis[1<<20];
int tot=0;
void check(char *c){
    tot++;
    int sum=0;
    for(int i = 0;c[i]!=0;i++){
        sum=sum*2+c[i]-'0';
    }
    assert(vis[sum]==0);
    vis[sum]=1;
}
int main(){
    char a[25],b[25];
    int m; 
    scanf("%s %s %d",a,b,&m);
    int ori=m;
    int popcount=0;
    for(int i = 0 ; i < 20 ; i++){
        if(a[i]!=b[i])popcount++;
    }
    if(popcount==0){
        if(m)printf("-1\n");
        else return -1;
    }
    else if(popcount%2!=m%2||popcount>m){
        printf("-1\n");
    }
    else{
        m-=popcount;
        vector<int> v2;
        int cnt=0;
        for(int i = 0;i<20;i++){
            if(a[i]!=b[i]){
                if(v2.size()==2)v.push_back(i);
                else v2.push_back(i);
            }
            else{
                v.push_back(i);
            }
        }
        m/=2;
        dfs(0,m);
        //printf("%s\n",a);
        check(a);
        for(auto it:ans){
            a[it]='1'-a[it]+'0';
            printf("%s\n",a);
             check(a);
        }
        a[v2[0]]='1'-a[v2[0]]+'0';
        printf("%s\n",a);
         check(a);
        reverse(ans.begin(),ans.end());
        for(auto it:ans){
            a[it]='1'-a[it]+'0';
            printf("%s\n",a);
         check(a);
        }
        if(v2.size()==2){
            a[v2[1]]='1'-a[v2[1]]+'0';
            printf("%s\n",a);
        check(a);
        }
        for(int i = 0;i<20;i++){
            if(a[i]!=b[i]){
                a[i]=b[i];
                printf("%s\n",a);
             check(a);
            }
        }
        assert(tot==ori+1);
    }
    
}
/*
00000000000000000000
11111111111111111111
20


*/
