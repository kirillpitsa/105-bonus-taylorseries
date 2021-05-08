#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ERROR_ARGC (-1);
#define STOP 50

void printer(long int counter, double myfunc, double origfunc)
{
    printf("По модулю последнего члена: шагов %ld\nЗначение: %f  ~   "
           "%f\nМодуль разности с точной реализацией: %e\n",
           counter, myfunc, origfunc, origfunc - myfunc);
}

double randomize(void)
{
    srand(time(0));
    const float RAND_MAX_F = RAND_MAX;
    double random = (double)rand() / RAND_MAX_F;
    return random;
}

double power(double e, uint64_t x)
{
    if (x == 0) {
        return 1;
    }
    if (x == 1) {
        return e;
    }
    if (x % 2 == 1) {
        return e * power(e * e, (x - 1) / 2);
    }
    return power(e * e, x / 2);
}
void ExpOst(double t, double eps)
{
    //Инициализация псевдорандома для остаточного члена
    double random = randomize();

    double origexp = exp(t);
    double x;
    if (t >= 0) {
        x = t;
    } else {
        x = -t;
    }

    uint64_t intx = (int)x;
    double fracx = x - intx;
    double myexpint = power(M_E, intx);
    double myexpfrac = 0;
    double a = 1.0;
    double i = 1.0;
    long int counter = 0;
    do {
        myexpfrac += a;
        a = a * fracx / i;
        i *= (i + 1);
        counter++;
        if (counter > STOP) {
            break;
        }
    } while (fabs(a * exp(random * x) - eps) > DBL_EPSILON);
    double myexp = myexpfrac * myexpint;
    if (t < 0) {
        myexp = 1.0 / myexp;
    }
    printer(counter, myexp, origexp);
}

void ExpLast(double t, double eps)
{
    double origexp = exp(t);
    double myexp;
    double x;
    if (t >= 0) {
        x = t;
    } else {
        x = -t;
    }
    uint64_t intx = (int)x;
    double fracx = x - intx;
    // printf(">>%ld<<\n>>%f<<\n",intx, fracx);
    double myexpint = power(M_E, intx);
    // printf("myexpint = %f - %f\n", myexpint, exp(intx));
    double myexpfrac = 0;
    double a = 1.0;
    double i = 1.0;
    long int counter = 0;
    do {
        myexpfrac += a;
        a = a * fracx / i;
        i *= (i + 1);
        // printf("%f -> ", a);
        counter++;
        if (counter > STOP) {
            break;
        }
    } while (fabs(a - eps) > DBL_EPSILON);
    // printf("\nmyexpfrac = %f - %f\n", myexpfrac, exp(fracx));
    myexp = myexpfrac * myexpint;
    if (t < 0) {
        myexp = 1.0 / myexp;
    }
    printer(counter, myexp, origexp);
}

int flags(double *x)
{
    int flag = 0; // sin = 0, -sin = 1, cos = 2, -cos = 3
    long int counter = 0;
    while (*x - M_PI / 2 > DBL_EPSILON) {
        *x -= M_PI / 2.0;
        counter++;
    }
    // printf("%f\n",x);  X находится от 0 до pi/2
    if (*x - M_PI / 4.0 <= DBL_EPSILON) {
        // x в 0 ... pi/4
        counter %= 4;
        if (counter == 0) {
            flag = 0;
        } else //(sin -> sin)   (+0)
            if (counter == 3) {
            flag = 3;
        } else //(sin -> cos)   (+pi/2)
            if (counter == 2) {
            flag = 1;
        } else //(sin-> -sin)   (+pi)
            if (counter == 1) {
            flag = 2;
        } //(sin -> -cos)   (+3pi/2)
    } else {
        //Доводим до -pi/4 ... 0
        *x -= M_PI / 2;
        *x = -*x;
        counter++;
        counter %= 4;
        //Оборачиваем в 0 ... pi/4
        if (counter == 0) {
            flag = 1;
        } else //(sin -> sin -> -sin)   (+0)
            if (counter == 3) {
            flag = 3;
        } else //(sin -> cos -> cos)   (+pi/2)
            if (counter == 2) {
            flag = 0;
        } else //(sin-> -sin -> sin)   (+pi)
            if (counter == 1) {
            flag = 2;
        } //(sin -> -cos -> -cos)   (+3pi/2)
    }
    return flag;
}

void SinLast(double t, double eps)
{
    double x;
    double origsin = sin(t);
    int minusflag = 0;
    int flag = 0; // sin = 0, -sin = 1, cos = 2, -cos = 3
    if (t < 0) {
        x = -t;
        minusflag = 1;
    } else {
        x = t;
    }
    long int counter = 0;
    flag = flags(&x);
    double myfunc = 0.0;
    double i = 1.0;
    //В зависимости от флага считаю нужную функцию
    if (flag > 1) //косинус
    {
        double a = 1;
        // printf("%f -> ", x);
        do {
            myfunc += a;
            a = (-a * x * x) / ((2 * i) * (2 * i - 1));
            i++;
            counter++;
            if (counter > STOP) {
                break;
            }
        } while (fabs(a - eps) > DBL_EPSILON);
    } else //синус
    {
        double a = x;
        do {
            myfunc += a;
            a = (-a * x * x) / ((2 * i) * (2 * i + 1));
            i++;
            counter++;
            if (counter > STOP) {
                break;
            }
        } while (fabs(a - eps) > DBL_EPSILON);
    }
    if (flag % 2 == 1) {
        myfunc = -myfunc;
    }
    if (minusflag == 1) {
        myfunc = -myfunc;
    }
    printer(counter, myfunc, origsin);
}

void SinOst(double t, double eps)
{
    //Инициализация рандома
    double random = randomize();

    double x;
    double origsin = sin(t);
    int minusflag = 0;
    int flag = 0; // sin = 0, -sin = 1, cos = 2, -cos = 3
    if (t < 0) {
        x = -t;
        minusflag = 1;
    } else {
        x = t;
    }
    long int counter = 0;
    flag = flags(&x);
    double myfunc = 0.0;
    double i = 1.0;
    if (flag > 1) //косинус
    {
        double a = 1;
        do {
            myfunc += a;
            a = (-a * x * x) / ((2 * i) * (2 * i - 1));
            i++;
            counter++;
            if (counter > STOP) {
                break;
            }
        } while (fabs((a * x * sin(random * x) / (i + 1)) - eps) > DBL_EPSILON);
    } else //синус
    {
        double a = x;
        do {
            myfunc += a;
            a = (-a * x * x) / ((2 * i) * (2 * i + 1));
            i++;
            counter++;
            if (counter > STOP) {
                break;
            }
        } while (fabs((a * x * sin(random * x) / (i + 1)) - eps) > DBL_EPSILON);
    }
    if (flag % 2 == 1) {
        myfunc = -myfunc;
    }
    if (minusflag == 1) {
        myfunc = -myfunc;
    }
    printer(counter, myfunc, origsin);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        return ERROR_ARGC;
    }
    double eps = strtod(argv[1], NULL);
    double x = strtod(argv[2], NULL);
    printf("Экспонента:\n");
    //экспонента по последнему члену
    ExpLast(x, eps);
    //экспонента по остаточному члену
    ExpOst(x, eps);
    //Чаще всего погрешность одна и та же

    printf("\nСинус:\n");
    //синус по последнему члену
    SinLast(x, eps);
    //синус по остаточному члену
    SinOst(x, eps);
    //Разные значения например при ./a.out 0.000000000000000001 1.032213213
    return 0;
}
