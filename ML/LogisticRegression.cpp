//
// Created by root on 28/05/19.
//

#include "LogisticRegression.h"
#include "DatasetReader.h"
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

LogisticRegression::LogisticRegression(void) {
    alpha = 1;
    th = 0.5;
    d = 10;
    m = 256;
    dataset_name = "Edin";
    epochs = 40;
    dt = new DatasetReader(datasets_path + dataset_name + "/", 2, d);
    for (int j = 0; j < d; j++) {
        w.push_back(0);
    }
    train_records = dt->read_all_train_records();
    test_records = dt->read_all_test_records();
}

double LogisticRegression::sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double LogisticRegression::approx_sigmoid_deg3(double x) {
    return sigmoid_coeffs_deg3[0] + sigmoid_coeffs_deg3[1] * x + sigmoid_coeffs_deg3[2] * x * x * x;
}

double LogisticRegression::approx_sigmoid_deg5(double x) {
    x = x / 8.0;
    return sigmoid_coeffs_deg5[0] + sigmoid_coeffs_deg5[1] * x + sigmoid_coeffs_deg5[2] * x * x * x + sigmoid_coeffs_deg5[3] * x * x * x * x * x;
}

double LogisticRegression::approx_sigmoid_deg7(double x) {
    //if (x > 8)
    //    cout << "Hum .. " << x << endl;
    x = x / 8.0;
    return sigmoid_coeffs_deg7[0] + sigmoid_coeffs_deg7[1] * x + sigmoid_coeffs_deg7[2] * x * x * x +
    sigmoid_coeffs_deg7[3] * x * x * x * x * x + sigmoid_coeffs_deg7[4] * x * x * x * x * x * x * x;
}

double LogisticRegression::dot_product(vector<double> a, vector<double> b) {
    double sum = 0.0;
    for (int i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

int LogisticRegression::predict(Record* r) {
    double wx = 0.0, prob = 0.0;
    int label = 0;

    r->print();
    for (int i = 0; i < d; i++) {
        wx += w[i] * r->values[i];
    }
    prob = sigmoid(wx);
    cout << "PROB : " << prob << endl;
    if (prob > th) {
        label = 1;
    } else {
        label = 0;
    }
    return label;
}

int LogisticRegression::approx_predict(Record* r) {
    double wx = 0.0, prob = 0.0;
    int label = 0;

    for (int i = 0; i < d; i++) {
        wx += w[i] * r->values[i];
    }
    prob = approx_sigmoid_deg3(wx);
    if (prob > th) {
        label = 1;
    } else {
        label = 0;
    }
    return label;
}

void LogisticRegression::approx_fit() {
    // Read the hole data set
    //int m = train_records.size();

    for (int j = 0; j < d; j++) {
        w[j] = 0.0;
    }


    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M");
    auto grads_file_name = "PLAIN_APPROX_GRADS_" + oss.str();
    auto weights_file_name = "PLAIN_APPROX_WEIGHTS_" + oss.str();

    ofstream grads_log_file;
    ofstream weights_log_file;

    grads_log_file.open(grads_file_name);
    weights_log_file.open(weights_file_name);

    for (int e = 0; e < epochs; e++) {
        double grad[d];
        for (int j = 0; j < d; j++) {
            grad[j] = 0.0;
        }
        for (int i = 0; i < m; i++) {
            Record* rcd = train_records[i];
            double wx = 0.0;
            for (int j = 0; j < d; j++) {
                wx += w[j] * rcd->values[j];
            }
            double sig = approx_sigmoid_deg3(wx);

            for (int j = 0; j < d; j++) {
                grad[j] += (sig - rcd->label) * rcd->values[j];
            }
        }
        cout << "Gradient : " << e << endl;
        for (int j = 0; j < d; j++) {
            grad[j] = grad[j] * alpha / (m);
            w[j] -= grad[j];
            grads_log_file << -grad[j] << ", ";
        }
        grads_log_file << " " << endl;


        for (int j = 0; j < d; j++) {
            weights_log_file << w[j] << ", ";
        }

        weights_log_file << " " << endl;
        cout << "Loss : " << loss() << endl;
    }
    grads_log_file.close();
    weights_log_file.close();
}

void LogisticRegression::fit() {
    // Read the hole data set
    //train_records = dt->read_all_train_records();
    //int m = train_records.size();

    for (int j = 0; j < d; j++) {
        w[j] = 0.0;
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M");
    auto grads_file_name = "PLAIN_GRADS_" + oss.str();
    auto weights_file_name = "PLAIN_WEIGHTS_" + oss.str();

    ofstream grads_log_file;
    ofstream weights_log_file;

    grads_log_file.open(grads_file_name);
    weights_log_file.open(weights_file_name);


    for (int i = 0; i < m; i++) {
        Record* rcd = train_records[i];
        //cout << "Record num " << rcd->id << ". Values: ";
        //for (int j = 0; j < d; j++) {
        //    cout << rcd->values[j] << " ";
        //}
        //cout << " Label: " << rcd->label << endl;
    }

    for (int e = 0; e < epochs; e++) {
        double grad[d];
        for (int j = 0; j < d; j++) {
            grad[j] = 0.0;
        }
        for (int i = 0; i < m; i++) {
            Record* rcd = train_records[i];
            double wx = 0.0;
            for (int j = 0; j < d; j++) {
                wx += w[j] * rcd->values[j];
            }
            double sig = sigmoid(wx);

            for (int j = 0; j < d; j++) {
                grad[j] += (sig - rcd->label) * rcd->values[j];
            }
        }
        cout << "Gradient : " << e << endl;
        for (int j = 0; j < d; j++) {
            grad[j] = grad[j] * alpha / (m);
            w[j] -= grad[j];
            grads_log_file << -grad[j] << ", ";
        }
        grads_log_file << " " << endl;


        for (int j = 0; j < d; j++) {
            weights_log_file << w[j] << ", ";
        }

        weights_log_file << " " << endl;
        cout << "Loss : " << loss() << endl;
        ///cout << "" << endl;
    }
    grads_log_file.close();
    weights_log_file.close();
}


double LogisticRegression::loss() {
    double loss = 0.0;
    for (int i = 0; i < m; i++) {
        Record* rcd = train_records[i];
        double wx = 0.0;
        for (int j = 0; j < d; j++) {
            wx += w[j] * rcd->values[j];
        }
        double h = sigmoid(wx);
        int y = rcd->label;
        loss += -y * log(h) - (1 - y) * (1 - h);
    }
    return loss;
}

vector<int> LogisticRegression::test() {
    long size = test_records.size();
    vector <int> labels(size);

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H-%M");
    auto file_name = "PLAIN_APPROX_PREDICTIONS_" + oss.str();
    ofstream log_file;
    log_file.open(file_name);

    // Read the hole data set
    for (int i = 0; i < size; i++) {
        labels[i] = approx_predict(test_records[i]);
        cout << labels[i];
        log_file << labels[i] << endl;
    }
    return labels;
}