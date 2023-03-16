#include <iostream>
#include<map>
#include<vector>
#include<string>
using namespace std;
class Product{
    private:
            int id;
            string name;
            int storeid;
    public:
        Product(int id,string name,int storeid){
            this->id = id;
            this->name = name;
            this->storeid = storeid;
        }
        int getId(){
            return this->id;
        }
        string getName(){
            return this->name;
        }
        string getInfor(){
            return to_string(this->id)+" "+this->name+" "+to_string(this->storeid);
        }
};
int main(){
    vector<Product> database = {Product(123,"iPhone_13",12),
                                Product(456,"iPad",5),
                                Product(789,"AirPods_Pro",14),
                                Product(321,"iPhone_14",0),
                                Product(654,"Apple_Watch",6)
                            };
    map<int,Product> byId;
    map<string,Product> byName;
    int query,queryId;
    string queryName;
    string result;

    for(auto product:database){
        byId.insert({product.getId(),product});
        byName.insert({product.getName(),product});
    }
    while (cin>>query)
    {
        if (!query)
        {
            break;
        }
        result = "404";
        switch (query)
        {
        case 1:
            cin>>queryId;
            if (byId.count(queryId))
            {
                Product pattern = byId.find(queryId)->second;
                result = pattern.getInfor();
            }
            break;
        
        case 2:
            cin>>queryName;
            if (byName.count(queryName))
            {
                Product pattern = byName.find(queryName)->second;
                result = pattern.getInfor();
            }
            break;
        }
        cout<<result<<endl;
    }
    return 0;
}