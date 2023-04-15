#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>  
using namespace std;
class Order{
public:
    string Id;
    int arriredTime;
    int createTime;
    int timeLimit;
    int priority;
    Order(string Id,int arriredTime,int createTime,int timeLimit,int priority){
        this->Id = Id;
        this->arriredTime = arriredTime;
        this->createTime = createTime;
        this->timeLimit = timeLimit;
        this->priority = priority;
    }
};
bool Compare(const Order& a, const Order& b) 
{ 
   if (a.priority > b.priority) return false;
   if (b.priority > a.priority) return true;

   if (a.timeLimit < b.timeLimit) return true;
   if (b.timeLimit < a.timeLimit) return false;
   return false;
}

vector<string> split(string line,string delimiter){
    vector<string> result;
    string token;
    size_t pos = 0;

    while ((pos = line.find(delimiter)) != string::npos) {
        token = line.substr(0, pos);
        result.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    result.push_back(line);
    return result;
}

int main(){
    vector<Order>OrderList;
    int OrderNumber;
    string input;
    vector<string> tokens;
    freopen("./input/Problem6.in", "r", stdin);
    freopen("./output/Problem6.out","w",stdout);
    cin>>OrderNumber;
    getchar();
    while (OrderNumber--){
        getline(cin, input);
        cout<<input<<endl;
        tokens = split(input," ");
        OrderList.push_back(Order(tokens[0],stoi(tokens[1]),stoi(tokens[2]),stoi(tokens[3]),stoi(tokens[4])));
    }
    sort(OrderList.begin(), OrderList.end(), Compare);
    // for(int i =0;i<OrderList.size();i++){
    //     cout<<OrderList[i].Id;
    //     if (i<OrderList.size()-1){
    //         cout<<" ";
    //     }
    //     else{
    //         cout<<endl;
    //     }
    // }
        for(int i =0;i<OrderList.size();i++){
            cout<<OrderList[i].Id<<" "<<OrderList[i].priority<<" "<<OrderList[i].timeLimit<<endl;
    }
    return 0;
}