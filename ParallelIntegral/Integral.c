#include "Integral.h"


double Integralling (double from, double to) 
{
    double res = 0;

    for (double x = from; x < to; x += X_STEP) 
        res += func(x) * X_STEP;

    return res;
}

double func(double x)
{
    return pow(sin(x), 7) + pow(cos(x) * cos(x), 5);
}