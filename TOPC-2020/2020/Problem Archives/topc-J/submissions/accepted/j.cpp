#include <bits/stdc++.h>
using namespace std;

const int probSZ = 200;
const int printDigits = 100;
const int inputDigits2 = 4; /* equals 2 times of 2 digits */

struct prob{
	int v[probSZ] = {0};
	
	prob(){
		for (int i=0; i<probSZ; i++) v[i] = 0;
	}
	
	prob (string s){
		int dpt, sz;
		sz = s.size();
		for (int i=0; i<probSZ; i++) v[i] = 0;
		for (dpt=0; dpt<sz; dpt++){
			if (s[dpt]=='.') break;
			v[0] = v[0]*10+(s[dpt]-'0');
		}
		for (int i=1; dpt+i<sz; i++){
			v[i] = s[dpt+i]-'0';
		}
	}
	
	void Print(int digits=6){
		printf("%d", v[0]);
		if (digits) printf(".");
		for (int i=1; i<=digits; i++) printf("%d", v[i]);
	}
	
	void carry(){
		for (int i=probSZ-1; i>0; i--){
			if (v[i]>=0 && v[i]<10) continue;
			int r = v[i]%10;
			if (r<0) r+=10;
			v[i-1] += ((v[i]-r)/10);
			v[i] = r;
		}
	}
	
	prob operator+(const prob &b)const{
		prob res = *this;
		for (int i=0; i<probSZ; i++) res.v[i] += b.v[i];
		res.carry();
		return res;
	}
	
	prob operator-(const prob &b)const{
		prob res = *this;
		for (int i=0; i<probSZ; i++) res.v[i] -= b.v[i];
		res.carry();
		return res;
	}
	
	prob operator*(const prob &b)const{
		prob res;
		for (int ia=0; ia<probSZ; ia++) for (int ib=0; ia+ib<probSZ; ib++) res.v[ia+ib] += v[ia]*b.v[ib];
		res.carry();
		return res;
	}
	
	prob operator/(const int &b)const{
		prob res = *this;
		int r = 0;
		for (int i=0; i<probSZ; i++){
			res.v[i] += r*10;
			r = res.v[i]%b;
			res.v[i] /= b;
		}
		return res;
	}
};

int TCs;
string input;
prob pA[2]; /* AS, AR */
prob winPCTA[2]; /* AS, AR */
prob DPSet[2][20][20]; /* AS|AR, scoreA, scoreB */
prob DPMatch[10][10]; /* SetsA, SetsB */
prob ans;
int i, x, y, a, b;

int denominatorP(prob t){
	int res = 0;
	for (int i=0; i<=inputDigits2; i++) res = res*10+t.v[i];
	return res;
}

int main(){
	prob Zero("0");
	prob One("1");
	
	scanf("%d", &TCs);
	while(TCs--){
		for (i=0; i<2; i++){
			cin >> input;
			pA[i] = prob(input);
		}
		
		pA[1] = prob("1")-pA[1];
		
		for (i=0; i<2; i++){
			for (a=0; a<12; a++) for (b=0; b<12; b++) DPSet[i][a][b] = Zero;
			for (a=0; a<5; a++) for (b=0; b<5; b++) DPMatch[a][b] = Zero;
		}
		
		for (i=0; i<2; i++) DPSet[i][0][0] = One;
		DPMatch[0][0] = One;
		
		for (i=0; i<2; i++) for (a=0; a<11; a++) for (b=0; b<11; b++){
			int server = (((a+b)&2)>>1)^i;
			DPSet[i][a+1][b] = DPSet[i][a+1][b]+DPSet[i][a][b]*pA[server];
			DPSet[i][a][b+1] = DPSet[i][a][b+1]+DPSet[i][a][b]*(One-pA[server]);
		}
		
		for (i=0; i<2; i++){
			winPCTA[i] = Zero;
			int deno = denominatorP((pA[0]*pA[1]+(One-pA[0])*(One-pA[1])));
			prob winD = (pA[0]*pA[1])/deno*prob("10000");
			
			for (x=0; x<10; x++) winPCTA[i] = winPCTA[i]+DPSet[i][11][x];
			
			winPCTA[i] = winPCTA[i]+DPSet[i][10][10]*winD;
			
		}
		
		for (a=0; a<4; a++) for (b=0; b<4; b++){
			int server = (a+b)&1;
			DPMatch[a+1][b] = DPMatch[a+1][b]+DPMatch[a][b]*winPCTA[server];
			DPMatch[a][b+1] = DPMatch[a][b+1]+DPMatch[a][b]*(One-winPCTA[server]);
		}
		
		ans = Zero;
		for (x=0; x<4; x++) ans = ans + DPMatch[4][x];
		
		ans.Print(printDigits); puts("");
		
	}
	
	
	
	return 0;
}

