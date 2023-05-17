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

int minimumDifference(int k, const vector<int>& values) {
    int totalSum = 0;
    for (int i = 0; i < k; i++) {
        totalSum += values[i];
    }

    int maxSum = totalSum / 2;

    vector<vector<int>> dp(k + 1, vector<int>(maxSum + 1, 0));
    for (int i = 1; i <= k; i++) {
        for (int j = 0; j <= maxSum; j++) {
            if (j >= values[i - 1]) {
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - values[i - 1]] + values[i - 1]);
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    int minDifference = totalSum - 2 * dp[k][maxSum];
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

    int result = minimumDifference(k, values);
    cout << result << endl;

    return 0;
}