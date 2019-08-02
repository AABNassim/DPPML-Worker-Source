//
// Created by root on 03/06/19.
//

#ifndef DPPML_PPLOGISTICREGRESSION_H
#define DPPML_PPLOGISTICREGRESSION_H

#include <vector>
#include <Ciphertext.h>
#include <cstdlib>
#include <chrono>
#include "HEAAN.h"
#include "../ML/DatasetReader.h"
#include "../PPML/MLSP.h"
#include <cmath>
#include "../ML/LogisticRegression.h"
#include <string.h>



#include "../CRYPTO/DTPKC.h"
#include "../CRYPTO/EvalAdd.h"

using namespace std;



class PPLogisticRegression {
public:
    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;
    long numThread = 5;
    double alpha = 1;
    int epochs = 50;
    int nb_slots = n;
    int nb_rows = 64;
    int nb_cols = 16;
    int log_nb_cols = 4;
    int log_nb_rows = 6;
    int d = 10;
    int class_number = 2;
    int sigmoid_degree = 3;
    int nb_training_ciphers = 1;
    int m = nb_rows * nb_training_ciphers;

    Ring ring;
    SecretKey secretKey;
    Scheme scheme;

    MLSP *mlsp;


    gmp_randstate_t randstate;
    mpz_class dtpkc_pkey, dtpkc_skey;
    int precision = 30;
    int nb_bits = 1024, error = 100;
    DTPKC dtpkc;

    long dtpkc_scale_factor = pow(10, 6);


    string dataset_name = "Edin";
    string datasets_path = "../DATA/Datasets/";
    //Ciphertext cipher_training_set;
    vector<Ciphertext> cipher_training_set;
    Ciphertext cipher_model;
    //vector<double> sigmoid_coeffs_deg3 = {0.5, 0.15012, -0.0015930078125};
    vector<double> sigmoid_coeffs_deg3 = {0.5, -1.20096, 0.81562};
    vector<double> sigmoid_coeffs_deg5 = {0.5, 1.53048, -2.3533056, 1.3511295};
    vector<double> sigmoid_coeffs_deg7 = {0.5, 1.73496, -4.19407, 5.43402, -2.50739};

    Ciphertext cipher_gadget_matrix;
    //vector<double> sigmoid_coeffs = {-0.0, 0.15012, 0.0};

    //vector<double> sigmoid_coeffs = {1.0, 1.0, 1.0};
    complex<double> *encoded_sigmoid_coeffs;
    Ciphertext cipher_sigmoid_coeffs;
    Ciphertext pp_sigmoid_deg3(Ciphertext cipher_x);
    void test_pp_sigmoid(vector<double> x);
    double approx_sigmoid_deg3(double x);
    void pp_fit();
    void encrypt_dataset();
    vector<Record*> decrypt_dataset();
    Ciphertext pp_dot_product(Ciphertext cx, Ciphertext cy);
    void test_pp_dot_product(vector<double> x, vector<double> y);
    Ciphertext sum_slots(Ciphertext c, int start_slot, int end_slot);
    Ciphertext sum_slots_reversed(Ciphertext c, int start_slot, int end_slot);
    void test_sum_slots();
    PPLogisticRegression();

    void test_cryptosystem_switching();
    Ciphertext cryptosystem_switching_single(DTPKC::Cipher dtpkc_value);
    Ciphertext cryptosystem_switching_batch_naive(vector<DTPKC::Cipher> dtpkc_values);
    Ciphertext cryptosystem_switching_batch_optimized(vector<DTPKC::Cipher> dtpkc_values);
    void test_cryptosystem_switching_batch_optimized();
    void test_cryptosystem_switching_batch_naive();

    Ciphertext refresh_cipher_unsecure(Ciphertext c);

    Ciphertext refresh_cipher(Ciphertext c);

};


#endif //DPPML_PPLOGISTICREGRESSION_H
