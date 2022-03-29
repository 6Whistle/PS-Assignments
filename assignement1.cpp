#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <windows.h>


using namespace std;

double Gaussian_distribution(double ave, double dev);

int main(void){
    srand((unsigned int)time(NULL));

    double ave, dev;
    double array[10];

    cin >> ave >> dev;
    for(int i = 0; i < 10; i++){
        array[i] = Gaussian_distribution(ave, dev);
        cout << array[i] << '\n';
    }
    system("pause");

}

double Gaussian_distribution(double ave, double dev){
    double x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    cout << x << " ";

    return exp(pow((x - ave), 2) / (-2 * dev)) / sqrt(2 * M_PI * dev);

}