#include <iostream>
#include<vector>
#include<math.h>
#include <map>

using namespace std;
const int INF = 1e9;
int main(){
    vector<int>result(3,0);
    vector<int>coins={10,5,1};
    int length;
    cin>>length;
    for(int i=0;i<coins.size();i++){
        result[i] =length/coins[i];
        length = length%coins[i];
    };
    cout<<result[0]<<" "<<result[1]<<" "<<result[2]<<endl;
    return 0;
}