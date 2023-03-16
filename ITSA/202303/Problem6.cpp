#include<iostream>
using namespace std;
int land[200][200];
int main(){
    int times,M,N,P;
    cin>>times;
    while (times--)
    {
        cin>>M>>N>>P;
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                cin>>land[i][j];
            }
            
        }
        
    }
    
    return 0;
}