//
// Created by root on 28/05/19.
//

#ifndef DPPML_LOGISTICREGRESSION_H
#define DPPML_LOGISTICREGRESSION_H

#include <vector>
#include "DatasetReader.h"
#include <math.h>

using namespace std;

class LogisticRegression {
public:
    double alpha;
    vector<double> w;
    int m = 512;
    int d = 10;
    double th;
    DatasetReader *dt;
    int epochs;
    double static sigmoid(double x);
    vector<double> sigmoid_coeffs_deg3 = {0.5, 0.15012, -0.0015930078125};
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
    LogisticRegression();
};


#endif //DPPML_LOGISTICREGRESSION_H
