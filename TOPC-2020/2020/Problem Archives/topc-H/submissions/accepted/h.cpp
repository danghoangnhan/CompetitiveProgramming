#include <bits/stdc++.h>
using namespace std;

template <int mod>
struct ModInt {
  int val;
  int trim(int x) const { return x >= mod ? x - mod : x < 0 ? x + mod : x; }
  ModInt(int v = 0) : val(trim(v % mod)) {}
  ModInt(long long v) : val(trim(v % mod)) {}
  ModInt &operator=(int v) { return val = trim(v % mod), *this; }
  ModInt &operator=(const ModInt &oth) { return val = oth.val, *this; }
  ModInt operator+(const ModInt &oth) const { return trim(val + oth.val); }
  ModInt operator-(const ModInt &oth) const { return trim(val - oth.val); }
  ModInt operator*(const ModInt &oth) const { return 1LL * val * oth.val % mod; }
  ModInt operator/(const ModInt &oth) const {
    function<int(int, int, int, int)> modinv = [&](int a, int b, int x, int y) {
      if (b == 0) return trim(x);
      return modinv(b, a - a / b * b, y, x - a / b * y);
    };
    return *this * modinv(oth.val, mod, 1, 0);
  }
  bool operator==(const ModInt &oth) const { return val == oth.val; }
  ModInt operator-() const { return trim(mod - val); }
  template<typename T> ModInt pow(T pw) {
    bool sgn = false;
    if (pw < 0) pw = -pw, sgn = true;
    ModInt ans = 1;
    for (ModInt cur = val; pw; pw >>= 1, cur = cur * cur) {
      if (pw&1) ans = ans * cur;
    }
    return sgn ? ModInt{1} / ans : ans;
  }
};

using mint = ModInt<int(1e9 + 7)>;

int main() {
  ios_base::sync_with_stdio(false); cin.tie(0);
  int N; cin >> N;
  vector<long long> A(N);
  for (int i = 0; i < N; ++i) {
    cin >> A[i];
  }
  sort(A.begin(), A.end());
  auto zero_it = lower_bound(A.begin(), A.end(), 0);
  auto pos_it = upper_bound(A.begin(), A.end(), 0);
  {
    mint ans = 0;
    if (pos_it == A.end()) {
      for (int i = 0; i < N - 1; ++i) {
        ans = ans + A[i] * A.back();
      }
    } else if (zero_it == A.begin()) {
      for (int i = 1; i < N; ++i) {
        ans = ans + A[0] * A[i];
      }
    } else {
      for (auto it = pos_it; it != A.end(); ++it) {
        ans = ans + A[0] * *it;
      }
      for (auto it = A.begin(); it != zero_it; ++it) {
        ans = ans + A.back() * *it;
      }
      ans = ans - A[0] * A.back();
    }
    cout << ans.val << ' ';
  }
  {
    mint ans = 0;
    if (pos_it != A.end()) {
      for (auto it = pos_it; it + 1 != A.end(); ++it) {
        ans = ans + *it * A.back();
      }
    }
    if (zero_it != A.begin()) {
      for (auto it = A.begin() + 1; it != zero_it; ++it) {
        ans = ans + *it * A[0];
      }
    }
    if (zero_it == pos_it and A.begin() != zero_it and pos_it != A.end()) {
      ans = ans + *pos_it * *(pos_it - 1);
    }
    cout << ans.val << '\n';
  }

}
