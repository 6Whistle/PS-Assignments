//P Value
#define P0_5 99.3           //P(0.5)
#define P0_1 117.4          //P(0.1)
#define P0_05 123.2         //P(0.05)
#define P0_001 148.2        //P(0.01)

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

double Gaussian_distribution(double ave, double dev);                               //Make Gaussian Random Value
double Chi_Square_Verification(double *value, int size, double ave, double dev);    //Calculate Chi-Square Value
void P_Value_Check(double chi);                                                     //Compare Chi-Square Value and P Value
void Print_Histogram(double* value, int size);                                      //Print Gaussian Random Value Histogram

int main(void){
    srand((unsigned int)time(NULL));

    int size = 100;                     //Number of random value
    double ave, var, dev;               //Average(mean), variance, Standard deviation
    double *array = new double[size];   //Gaussian random value

    cout << "Input mean and variance(ex. 0 1) : ";      //Insert mean and variance
    cin >> ave >> var;
    dev = sqrt(var);                                    //Calculate standard deviation;

    /*  Store Gaussian random value at array  */
    cout << "\n====== Gaussian Random Value ======" << endl;
    for(int i = 0; i < size; i++){
        array[i] = Gaussian_distribution(ave, dev);     //Make Gaussian random value
        cout << array[i] << endl;                       //Print value
    }
    cout << "====================================\n";

    Print_Histogram(array, size);                       //Sort and print using Histogram
    
    double chi = Chi_Square_Verification(array, size, ave, dev);        //Calculate Chi value
    cout << "\nChi-Square Verification : " <<  chi << endl;             
    P_Value_Check(chi);                                                 //Compare Chi value and P value

    delete[] array;

    return 0;
}


/*  Make Gaussian Random Value using Marsaglia polar method  */
double Gaussian_distribution(double ave, double dev){
    while(true){
        double x1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;      //random value(-1 ~ 1)
        double x2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;      //random value(-1 ~ 1)

        double pw = x1 * x1 + x2 * x2;      //w = x1^2 + x2^2 (Pythagoras theorem)

        if(pw > 1)                          //if length^2(w) is bigger than 1, make new random value
            continue;

        return sqrt(-2 * log(pw) / pw) * x1 * dev + ave;        //(-2ln(w)/w)^(1/2) * x1 * dev + ave (Marsaglia polar method)
    }
}

/*  Calculate Chi_Square value  */
double Chi_Square_Verification(double *value, int size, double ave, double dev){
    double chi = 0;

    for(int i = 0 ; i < size ; i++)
        chi += pow((value[i] - ave) / dev, 2);      //sigma[0 ~ 100]((V - mean) / deviation)^2 = (100 - 1)s^2 / dev^2 = (X99)^2 

    return chi;
}

/*  Compare Chi_Square value and p_value  */
void P_Value_Check(double chi){
    if(chi < P0_5){
        cout << "P_Value > 0.5" << endl;
    }
    else if(chi < P0_1){
        cout << "0.1 < P_Value < 0.5" << endl;
    }
    else if(chi < P0_05){
        cout << "0.01 < P_Value < 0.05" << endl;
    }
    else if(chi < P0_001){
        cout << "0.001 < P_Value < 0.05" << endl;
    }
    else{
        cout << "P_Value < 0.001" << endl;
    }

    return;
}

/*  Print Value using Histogram  */
void Print_Histogram(double* value, int size){
    sort(value, value + size);          //Sort Random Value
    
    double min = value[0];
    double max = value[size - 1];
    
    double temp = min;
    double part = (max - min) / 20;     //Separate 21 partision
    int count[21] = { 0 };              //Count of each partision
    int j = 0;

    cout << "\n====== Histogram ======" << endl;
    for(int i = 0; i < size ; i++){
       if(value[i] < (temp + part)){    //If value is in the range
           (count[j])++;
       }
       else{
            cout << setw(10) << left << temp << "  : ";     //If value is out of range
            for(int k = 0 ; k < count[j] ; k++)             //Print count
                cout << "* ";
            cout << endl;

            j++;
            count[j]++;
            temp += part;
       }
    }
    cout << setw(10) << left << max << "  : ";
    for(int k = 0 ; k < count[20] ; k++)                    //Print Last count
        cout << "* ";
    cout << "\n========================\n";

    return;
}