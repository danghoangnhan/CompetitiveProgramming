#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Product{
public:
Product(int id,int profit,int cost,int weigth,int expired){
    this->id = id;
    this->profit = profit;
    this->cost = cost;
    this->weight = weight;
    this->expired = expired;
}
bool Compare(const Product& a, const Product& b) 
{ 
   if (a.profit < b.profit) return true;
   if (b.profit < a.profit) return false;

   if (a.cost < b.cost) return true;
   if (b.cost < a.cost) return false;

   if (a.weight < b.weight) return true;
   if (b.weight < a.weight) return false;

   if (a.expired < b.expired) return false;
   if (b.expired < a.expired) return true;

   if (a.id < b.id) return true;
   if (b.id < a.id) return false;
   return false;
}

private:
int id,profit,cost,weight,expired;
}
int main(){
    string input;
    string token;
    while (getline(cin, input)) {
        Product *newProduct = new Product(int(getline(input,token,";")),int(getline(input,token,";")),int(getline(input,token,";")),int(getline(input,token,";")),int(getline(input,token,";")));
        

    }
    
    return 0;
}