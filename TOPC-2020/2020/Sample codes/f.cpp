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
struct node{
    node *l,*r;
    int a,b;
    LL sum;
    LL pro;
    LL val;
    node(int _a,int _b):l(NULL),r(NULL),a(_a),b(_b),sum(0),pro(1),val(-1){}
}*root;
int mod=1e9+7;
void build(node *n){
    if(n->a==n->b)return;
    int mid=(n->a+n->b)/2;
    n->l=new node(n->a,mid);
    n->r=new node(mid+1,n->b);
    build(n->l);
    build(n->r);
}
int Sum(node *n){
    if(n->val!=-1){
        return (n->b-n->a+1)*n->val%mod*n->pro%mod;
    }
    else{
        return n->sum*n->pro%mod;
    }
}

void pull(node *n){
    n->sum=(Sum(n->l)+Sum(n->r))%mod;
}
void push(node *n){
    if(n->val!=-1){
        n->l->val=n->val;
        n->l->pro=1;
        n->r->val=n->val;
        n->r->pro=1;
        n->val=-1;
    }
    n->l->pro*=n->pro;
    n->l->pro%=mod;
    n->r->pro*=n->pro;
    n->r->pro%=mod;
    n->pro=1;
}
void mul(node *n,int l,int r,int k){
    if(n->a>=l&&n->b<=r){
        n->pro*=k;
        n->pro%=mod;
        return ;
    }
    if(n->b<l||n->a>r)return;
    push(n);
    mul(n->l,l,r,k);
    mul(n->r,l,r,k);
    pull(n);
}
int query(node *n,int l,int r){
    if(n->a>=l&&n->b<=r){
        return Sum(n);
    }
    if(n->b<l||n->a>r)return 0;
    push(n);
    int res=(query(n->l,l,r)+query(n->r,l,r))%mod;
    pull(n);
    return res;
}
void assign(node *n,int l,int r,int k){
    if(n->a>=l&&n->b<=r){
        n->val=k;
        n->pro=1;
        return;
    }
    if(n->b<l||n->a>r)return ;
    push(n);
    assign(n->l,l,r,k);
    assign(n->r,l,r,k);
    pull(n);
}
LL f_pow(LL a,LL b){
    LL res=1,temp=a;
    while(b){
        if(b&1)res=res*temp%mod;
        temp=temp*temp%mod;
        b>>=1;
    }
    return res;
}
void solve(){
    int n,q;
    scanf("%d %d",&n,&q);
    LL ans=0;
    root = new node(1,n);
    build(root);
    for(int i = 0;i<q;i++){
        int l,r,x;
        char c;
        l=rand()%n+1;
        r=rand()%n+1;
        if(l>r)swap(l,r);
        x=rand()%10+1;
        c="+-*/"[rand()%4];
        scanf("%d %d %c %d",&l,&r,&c,&x);
        //printf("%d %d %c %d\n",l,r,c,x);
        if(c=='-')c='+',x=(mod-x)%mod;
        if(c=='/')c='*',x=f_pow(x,mod-2);
        if(c=='+'){
            ans+=query(root,l,r);
            assign(root,l,r,x);
        }
        if(c=='*'){
            mul(root,l,r,x);
        }
    }
    printf("%lld\n",(ans+query(root,1,n))%mod);
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
