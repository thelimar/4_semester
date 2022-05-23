#include "Integral.h"
#define e 2.71828182846

/*
double Integralling (double from, double to) 
{
    double res = 0;

    for (double x = from; x < to; x += X_STEP) 
        res += func(x) * X_STEP;

    return res;
}
*/

double func(double x)
{
    return 1.8 * (pow (e, x) + pow (e, -x));
}