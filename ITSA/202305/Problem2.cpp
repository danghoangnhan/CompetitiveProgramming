#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>  
using namespace std;


#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int minimumDifference(const vector<int>& values) {
    int totalSum = 0;
    for (int i = 0; i < values.size(); i++) {
        totalSum += values[i];
    }

    int maxSum = totalSum / 2;
    //dividing totalSum by 2,  for a balanced division

    vector<vector<int>> dp(values.size() + 1, vector<int>(maxSum + 1, 0));
    
    for (int i = 1; i <= values.size(); i++) {
        for (int j = 0; j <= maxSum; j++) {
            if (j >= values[i - 1]) {
                //Check if the current item values[i - 1] can be included in the current total value j:
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - values[i - 1]] + values[i - 1]);
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }
    int minDifference = totalSum - 2 * dp[values.size()][maxSum];
    return minDifference;
}

int main() {
    freopen("./input/Problem2.in", "r", stdin);
    freopen("./output/Problem2.out","w",stdout);
    int k;
    cin >> k;

    vector<int> values(k);
    for (int i = 0; i < k; i++) {
        cin >> values[i];
    }

    int result = minimumDifference(values);
    cout << result << endl;

    return 0;
}