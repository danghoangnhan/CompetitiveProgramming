#include <iostream>
#include<vector>
#include<math.h>
#include <map>

using namespace std;

int main(){
    vector<vector<int>>Map(101,vector<int>(101,0));
    int N,M,startX, startY, endX, endY,result;
    freopen("./input/Problem7.in", "r", stdin);
    freopen("./output/Problem7.out","w",stdout);

    cin>>N>>M;

    for(int i=0;i<N;i++){
        for (int j = 0; j < N; j++){
            cin >>Map[i][j];
        }
    };
    for (int i = 0; i < M; i++){
        cin>>startX>>startY>>endX>>endY;
        result=0;
        for (int j = startX-1; j <= endX-1; j++){
            for (int l = startY-1; l <= endY-1; l++){
                result+=Map[j][l];
            }
        }
        cout<<result<<endl;
        
    }
    
    return 0;
}