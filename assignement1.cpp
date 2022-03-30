#define _USE_MATH_DEFINES
#define P0_05 123.2
#define P0_001 148.2 

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

double Gaussian_distribution(double ave, double dev);
double Chi_Square_Verification(double *value, int size, double ave, double dev);
void P_Value_Check(double chi);
void Print_Histogram(double* value, int size);

int main(void){
    srand((unsigned int)time(NULL));

    int size = 100;
    double ave, var, dev;
    double array[size];

    cout << "평균과 분산을 입력하세요(ex. 0 1) : ";
    cin >> ave >> var;
    dev = sqrt(var);

    cout << "\n====== Gaussian Random Value ======" << endl;
    for(int i = 0; i < size; i++){
        array[i] = Gaussian_distribution(ave, dev);
        cout << array[i] << endl;
    }
    cout << "====================================\n";

    Print_Histogram(array, size);
    
    double chi = Chi_Square_Verification(array, size, ave, dev);
    cout << "\nChi-Square Verification : " <<  chi << endl;
    P_Value_Check(chi); 
}


//Marsaglia polar method
double Gaussian_distribution(double ave, double dev){
    while(true){
        double x1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
        double x2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;

        double pw = x1 * x1 + x2 * x2;

        if(pw > 1)
            continue;

        return sqrt(-2 * log(pw) / pw) * x1 * dev + ave;
    }
}

double Chi_Square_Verification(double *value, int size, double ave, double dev){
    double chi = 0;

    double prob;

    for(int i = 0 ; i < size ; i++){
        prob = exp(pow(value[i] - ave, 2) / (-2 * dev * dev)) / (sqrt(2 * M_PI) * dev);
        chi += pow(value[i] - prob * size, 2) / (prob * size);
    }

    return chi;
}

void P_Value_Check(double chi){
    if(chi < P0_05){
        cout << "P_Value > 0.05" << endl;
    }
    else if(chi < P0_001){
        cout << "0.05 > P_Value > 0.001" << endl;
    }
    else{
        cout << "0.001 > P_Value" << endl;
    }

    return;
}

void Print_Histogram(double* value, int size){
    sort(value, value + size);
    
    double min = value[0];
    double max = value[size - 1];
    
    double temp = min;
    double part = (max - min) / 20;
    int count[21] = { 0 };
    int j = 0;

    cout << "\n====== Histogram ======" << endl;
    for(int i = 0; i < size ; i++){
       if(value[i] < (temp + part)){
           (count[j])++;
       }
       else{
            cout << setw(10) << left << temp << "  : ";
            for(int k = 0 ; k < count[j] ; k++)
                cout << "* ";
            cout << endl;

            j++;
            count[j]++;
            temp += part;
       }
    }
    cout << setw(10) << left << max << "  : ";
    for(int k = 0 ; k < count[20] ; k++)
        cout << "* ";
    cout << "\n========================\n";

    return;
}