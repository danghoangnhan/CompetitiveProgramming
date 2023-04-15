#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>  

using namespace std;

class Product{
public:
Product(int id,int profit,int cost,int weight,int expired){
    this->id = id;
    this->profit = profit;
    this->cost = cost;
    this->weight = weight;
    this->expired = expired;
}
int id,profit,cost,weight,expired;
string showId(){
    string result = to_string(this->id);
    int remain  =  5 - result.size();
    if(remain>0){
        for (int i = 0; i < remain; i++)
        {
            result = "0"+result;
        }
        
    }
    return result;
}
};
bool Compare(const Product& a, const Product& b) 
{ 
   if (a.profit < b.profit) return false;
   if (b.profit > a.profit) return true;

   if (a.cost > b.cost) return true;
   if (b.cost < a.cost) return false;

   if (a.weight > b.weight) return true;
   if (b.weight < a.weight) return false;

   if (a.expired > b.expired) return true;
   if (b.expired < a.expired) return false;

   if (a.id < b.id) return true;
   if (b.id < a.id) return false;

   return false;
}
vector<int> split(string line,string delimiter){
    vector<int> result;
    string token;
    size_t pos = 0;

    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        result.push_back(stoi(token));
        line.erase(0, pos + delimiter.length());
    }
    result.push_back(stoi(line));
    return result;
}
int main(){
    vector<Product>ProductList;
    string input;
    vector<int> tokens;
    char *ptr;
    freopen("./input/Problem4.in", "r", stdin);
    freopen("./output/Problem4.out","w",stdout);
    
    while (getline(cin, input)) {
        tokens = split(input,",");
        int id      = tokens[0];
        int profit  = tokens[1];
        int cost    = tokens[2];
        int weight  = tokens[3];
        int expired = tokens[4];
        ProductList.push_back(Product(id,profit,cost,weight,expired));
    }

    sort(ProductList.begin(), ProductList.end(), Compare);
    for(Product element:ProductList){
        cout<<element.id<<" "<<element.profit<<" "<<element.cost<<" "<<element.weight<<" "<<element.expired<<endl;
    }
    return 0;
}