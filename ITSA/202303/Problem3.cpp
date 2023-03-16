#include <iostream>
#include<stack>
#include<string>
using namespace std;


int main(){
    int time,sample,result,bonus;
    bool extra;
    cin>>time;
    
    while (time--)
    {
        result=70;
        cin>>sample;
        int bonus = sample - 1500;
        if (bonus>0)
        {   
            if(bonus<500)bonus = 500;
            result += (bonus/500)*5;
            result += (bonus%500)*5;
        }
        cout <<result<<endl;
    }
    return 0;
}