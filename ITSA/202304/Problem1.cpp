#include <iostream>
#include <vector>
using namespace std;

int main(){
    int cases,row,column;
    vector<vector<int>> mapper(102,vector<int>(102,0));
    vector<vector<char>> result(102,vector<char>(102,'_'));
    vector<pair<int,int>> adja={make_pair(0,1),make_pair(1,0),make_pair(0,-1),make_pair(-1,0)};

    freopen("./input/Problem1.in", "r", stdin);
    freopen("./output/Problem1.out","w",stdout);

    cin>>cases;
    while (cases--){
        cin >>row>>column;
        for (int i = 0; i < row; i++)
            for (int j = 0; j < column; j++)
                cin>>mapper[i][j];

        for (int i = 0; i < row; i++){
            for (int j = 0; j < column; j++){
                result[i][j]='_';
                for (auto location:adja){
                    if (mapper[i][j]==1&&mapper[i+location.first][j+location.second]==0){
                        result[i][j]='0';
                        break;
                    }
                }
                cout<<result[i][j];
            }
            cout<<endl;    
        }
    }
    return 0;
}