#include<iostream>
using namespace std;
int population[122][3];
int main(){
    population[0][0]=1;
    population[0][1]=0;
    population[0][2]=0;

    int k;
    for (int i = 1; i <= 122; i++)
    {
        population[i][0]=population[i-1][1]+population[i-1][2];
        population[i][1]=population[i-1][0];
        population[i][2]=population[i-1][1];
    }
    while(cin>>k){
        if (k>=122)
        {
            break;
        }
        cout<<population[k][0]<<endl;
    }
    return 0;
}