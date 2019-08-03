//
// Created by Nassim AAB on 2019-07-27.
//

#ifndef DPPML_CSP_PPLR_H
#define DPPML_CSP_PPLR_H

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "HEAAN.h"

#include <vector>
#include <Ciphertext.h>
#include <cstdlib>
#include <chrono>
#include "../ML/DatasetReader.h"
#include "../PPML/MLSP.h"
#include <cmath>
#include "../ML/LogisticRegression.h"
#include <string.h>

#include <iomanip>
#include <ctime>
#include <sstream>

#include "../CRYPTO/DTPKC.h"
#include "../CRYPTO/EvalAdd.h"


using namespace std;

class Worker_PPLR {
public:

    // Network attributes
    int mlsp_sock = 0;
    struct sockaddr_in mlsp_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    int mlsp_port = 8100;
    char *mlsp_ip = "127.0.0.1";
    int id = 1;


    // Network functions
    Worker_PPLR();
    bool read_file(int sock, char* path);

    bool send_file(int sock, char* path);
    bool read_long(int sock, long *value);
    bool read_data(int sock, void *buf, int buflen);
    bool send_long(int sock, long value);
    bool send_data(int sock, void *buf, int buflen);
    void connect_to_mlsp();

        // Logistic Regression..

    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 450; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;
    long numThread = 5;
    double alpha = 1;
    int epochs = 30;
    int nb_slots = n;
    int nb_rows = 64;
    int nb_cols = 16;
    int log_nb_cols = 4;
    int log_nb_rows = 6;
    int d = 10;
    int class_number = 2;
    int sigmoid_degree = 3;
    int nb_training_ciphers = 8;
    int m = nb_rows * nb_training_ciphers;
    int start_record = 0;

    int refresh_period = 1;

    Ring ring;
    SecretKey secretKey;
    Scheme scheme;


    string dataset_name = "Edin";
    string datasets_path = "../DATA/Datasets/";
    vector<Ciphertext> cipher_training_set;
    Ciphertext cipher_model;
    vector<double> sigmoid_coeffs_deg3 = {0.5, -1.20096, 0.81562};
    vector<double> sigmoid_coeffs_deg5 = {0.5, -1.53048, 2.3533056, -1.3511295};
    vector<double> sigmoid_coeffs_deg7 = {0.5, -1.73496, 4.19407, -5.43402, 2.50739};

    Ciphertext cipher_gadget_matrix;

    complex<double> *encoded_sigmoid_coeffs;
    Ciphertext cipher_sigmoid_coeffs;

    Ciphertext pp_sigmoid_deg3(Ciphertext cipher_x);
    Ciphertext pp_sigmoid_deg5(Ciphertext cipher_x);
    Ciphertext pp_sigmoid_deg7(Ciphertext cipher_x);

    Ciphertext pp_sigmoid(Ciphertext c, int degree);

    void test_pp_sigmoid(vector<double> x);
    double approx_sigmoid_deg3(double x);
    void pp_fit();
    void encrypt_dataset();
    vector<Record*> decrypt_dataset();
    Ciphertext pp_dot_product(Ciphertext cx, Ciphertext cy);
    Ciphertext sum_slots(Ciphertext c, int start_slot, int end_slot);
    Ciphertext sum_slots_reversed(Ciphertext c, int start_slot, int end_slot);
    void test_sum_slots();


    Ciphertext refresh_cipher_local_unsecure(Ciphertext c);

    Ciphertext refresh_cipher_local(Ciphertext c);


    void pp_fit_local();


    };


#endif //DPPML_CSP_PPLR_H
