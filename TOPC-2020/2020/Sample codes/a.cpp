#include <bits/stdc++.h>
using namespace std;

int main() {
    int x, y;
    cin >> x >> y;
    while (x > 0) {
        cout << x << ' ' << y << '\n';
        x--;
    }
    while (y > 0) {
        cout << x << ' ' << y << '\n';
        y--;
    }
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            for (int j = 0; j < 10; j++) {
                cout << i << ' ' << j << '\n';
            }
        } else {
            for (int j = 9; j >= 0; j--) {
                cout << i << ' ' << j << '\n';
            }
        }
    }
    return 0;
}
