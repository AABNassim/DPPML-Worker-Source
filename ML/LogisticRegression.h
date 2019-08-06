//
// Created by root on 28/05/19.
//

#ifndef DPPML_LOGISTICREGRESSION_H
#define DPPML_LOGISTICREGRESSION_H

#include <vector>
#include "DatasetReader.h"
#include <math.h>

#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

class LogisticRegression {
public:
    double alpha;
    vector<double> w;
    int m = 12500;
    int m2 = 1024;
    int d = 16;
    double th;
    int sigmoid_degree = 7;
    int class_number = 2;
    DatasetReader *dt;
    int epochs;
    double static sigmoid(double x);
    vector<double> sigmoid_coeffs_deg3 = {0.5, 1.20096, -0.81562};
    vector<double> sigmoid_coeffs_deg5 = {0.5, 1.53048, -2.3533056, 1.3511295};
    vector<double> sigmoid_coeffs_deg7 = {0.5, 1.73496, -4.19407, 5.43402, -2.50739};
    vector<Record*> train_records;
    vector<Record*> test_records;
    string dataset_name;
    string datasets_path = "../DATA/Datasets/";
    int predict(Record* r);
    void fit();
    void approx_fit();
    vector<int> test();
    int approx_predict(Record* r);
    double dot_product(vector<double> a, vector<double> b);

    double loss();
    double approx_sigmoid_deg3(double x);
    double approx_sigmoid_deg5(double x);
    double approx_sigmoid_deg7(double x);
    double approx_sigmoid(double x, int degree);

    LogisticRegression();
};


#endif //DPPML_LOGISTICREGRESSION_H
