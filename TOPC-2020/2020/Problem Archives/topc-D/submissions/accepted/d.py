from fractions import Fraction

P, Q, R = map(Fraction, input().split(' '))
S = P - Q + R
Area = P + Q + R + S

x = 2 * (P + S) / Area - Fraction(1) / 2
y = 2 * (P + Q) / Area - Fraction(1) / 2

if x <= 0 or x >= 1 or y <= 0 or y >= 1:
    print(-1)
else:
    print(x, y)
