#include <cstdio>
#include <cmath>

inline double ToRadians(double deg) {
    return deg * M_PI / 180.0;
}

int main() {
    for (int d = 0; d <= 90; d += 1) {
        printf("%02d %0.6f %0.9f\n", d, ToRadians(d), cos(ToRadians(d)));
    }
    return 0;
}