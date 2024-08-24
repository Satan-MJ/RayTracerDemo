//
// Implemented independently
//

#ifndef RAYTRACING_BERNSTEIN_HPP
#define RAYTRACING_BERNSTEIN_HPP

#include <cmath>

int binomialCoefficient(int n, int k) {
    if (k > n) {
        return 0;
    }
    if (k * 2 > n) {
        k = n - k;
    }
    if (k == 0) {
        return 1;
    }

    int result = n;
    for (int i = 2; i <= k; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}

float bernstein(int n, int i, float u) {
    return binomialCoefficient(n, i) * pow(u, i) * pow(1 - u, n - i);
}

float bernsteinDerivative(int n, int i, float u) {
    if (i == 0) {
        return -n * bernstein(n - 1, i, u);
    } else if (i == n) {
        return n * bernstein(n - 1, i - 1, u);
    } else {
        return n * (bernstein(n - 1, i - 1, u) - bernstein(n - 1, i, u));
    }
}



#endif //RAYTRACING_BERNSTEIN_HPP
