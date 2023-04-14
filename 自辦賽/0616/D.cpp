#include <bits/stdc++.h>
#define inf 0x3f3f3f3f
long long int numb;
using namespace std;
long long int arr[(1e5)+1];
long long int current;
long long int ans;
long long int minimum;
long long int nextDestination;
int main(){
	cin>>numb;
	current=1;+
	ans=-1;
	for(long long int i=0;i<numb;i++){
		cin>>arr[i];
	}
	while(arr[current]!=0){
		minimum=inf;
		nextDestination=0;
		for(long long int i=current+1;i<numb;i++){
			if(arr[i]<minimum){
				nextDestination=i;
			}
		}
		

	}
	return 0;
}