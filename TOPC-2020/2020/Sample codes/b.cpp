#include<bits/stdc++.h>
#define pii pair<int,int>
#define x first
#define y second
#define pb push_back
#define mp make_pair
using namespace std;
int main(){
    int l1,r1,l2,r2;
    scanf("%d %d %d %d",&l1,&r1,&l2,&r2);
    int q;
    scanf("%d",&q);
    while(q--){
        int x;
        scanf("%d",&x);
        if(x<=l1||x>=r2){
            printf("%d\n",r2-l2+r1-l1);
        }
        else{
            vector<pii> v;
            if(x<=r1){
                v.pb(mp(0,1));
                v.pb(mp(0,1));
                v.pb(mp(r1-x,-1));
                v.pb(mp(x-l1,-1));
                v.pb(mp(l2-x,1));
                v.pb(mp(r2-x,-1));
            }
            else if(x<=l2){
                v.pb(mp(l2-x,1));
                v.pb(mp(r2-x,-1));
                v.pb(mp(x-r1,1));
                v.pb(mp(x-l1,-1));
            }
            else if(x<=r2){
                 v.pb(mp(0,1));
                v.pb(mp(0,1));
                v.pb(mp(r2-x,-1));
                v.pb(mp(x-l2,-1));
                v.pb(mp(x-l1,-1));
                v.pb(mp(x-r1,1));
            }
            sort(v.begin(),v.end());
            int last=0;
            int sum=0;
            int ans=0;
            for(auto it:v){
              //  printf("%d %d\n",it.x,it.y);
                if(sum)ans+=it.x-last;
                last=it.x;
                sum+=it.y;
            }
            printf("%d\n",ans);
        }
    }
}