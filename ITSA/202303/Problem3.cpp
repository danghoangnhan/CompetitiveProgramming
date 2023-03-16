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
            while (bonus>0)
            {
                result +=5;
                bonus-=500;
            }
            
        }
        cout <<result<<endl;
    }
    return 0;
}