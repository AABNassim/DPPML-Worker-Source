//
// Created by root on 03/06/19.
//

#include "PPLogisticRegression.h"
#include <iomanip>
#include <ctime>
#include <sstream>


PPLogisticRegression::PPLogisticRegression(void) : secretKey(ring), scheme(secretKey, ring) {


    srand(time(NULL));
    SetNumThreads(numThread);

    //SecretKey secretKey(ring);
    //Scheme scheme(secretKey, ring);
    scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
    scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message

    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));
    dtpkc.keygen(precision, randstate, nb_bits, error);

    dtpkc.getKey(dtpkc_pkey, dtpkc_skey);
    dtpkc.updatePkw(dtpkc_pkey);

    encoded_sigmoid_coeffs = new complex<double>[n];
    for (int i = 0; i < sigmoid_degree; i++) {
        complex<double> c;
        c.imag(0);
        c.real(sigmoid_coeffs_deg3[i]);
        encoded_sigmoid_coeffs[i] = c;
    }
    scheme.encrypt(cipher_sigmoid_coeffs, encoded_sigmoid_coeffs, n, logp, logq);


    // Model initialization
    complex<double> *encoded_model = new complex<double>[n];
    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_model[i] = c;
    }
    scheme.encrypt(cipher_model, encoded_model, n, logp, logq);


    // Toy Matrix creation
    complex<double> *encoded_gadget_matrix = new complex<double>[n];
    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_gadget_matrix[i] = c;
    }
    for (int i = 0; i < nb_rows; i++) {
        complex<double> c;
        c.imag(0);
        c.real(1);
        encoded_gadget_matrix[i * nb_cols] = c;
    }
    scheme.encrypt(cipher_gadget_matrix, encoded_gadget_matrix, n, logp, logq);
}



/*void PPLogisticRegression::encrypt_dataset() {
    DatasetReader *datasetReader = new DatasetReader(datasets_path + dataset_name + "/", class_number, d);
    //cout << "AYAA" << endl;
    complex<double> *vectorized_dataset = new complex<double>[nb_slots];
    vector<Record*> training_data = datasetReader->read_all_train_records();

    for (int i = 0; i < m; i++) {
        training_data[i]->print();
    }

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        vectorized_dataset[i] = c;
    }

    for (int i = 0; i < m; i++) {
        Record* rcd = training_data[i];
        for (int j = 0; j < d; j++) {
            complex<double> c;
            int label;
            if (rcd->label == 0) {
                label = -1;
            } else {
                label = 1;
            }
            c.imag(0);
            c.real(rcd->values[j] * label);
            vectorized_dataset[i * nb_cols + j] = c;
        }
        scheme.encrypt(cipher_training_set, vectorized_dataset, n, logp, logq);
    }
}*/


void PPLogisticRegression::encrypt_dataset() {
    DatasetReader *datasetReader = new DatasetReader(datasets_path + dataset_name + "/", class_number, d);
    complex<double> *vectorized_batch = new complex<double>[nb_slots];
    vector<Record*> training_data = datasetReader->read_all_train_records();

    for (int i = 0; i < m; i++) {
        training_data[i]->print();
    }

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        vectorized_batch[i] = c;
    }

    for (int i = 0; i < nb_training_ciphers; i++) {
        Ciphertext cipher_training_batch;
        vectorized_batch = new complex<double>[nb_slots];
        for (int j = 0; j < nb_rows; j++) {
            Record *rcd = training_data[i * nb_rows + j];
            for (int k = 0; k < d; k++) {
                complex<double> c;
                int label;
                if (rcd->label == 0) {
                    label = -1;
                } else {
                    label = 1;
                }
                c.imag(0);
                c.real(rcd->values[k] * label);
                vectorized_batch[j * nb_cols + k] = c;
            }
        }
        scheme.encrypt(cipher_training_batch, vectorized_batch, n, logp, logq);
        cipher_training_set.push_back(cipher_training_batch);
    }
}

vector<Record*> PPLogisticRegression::decrypt_dataset() {
    vector<Record*> records(m);
    for (int i = 0; i < nb_training_ciphers; i++) {
        complex<double> *decrypted_training_batch = scheme.decrypt(secretKey, cipher_training_set[i]);
        for (int j = 0; j < nb_rows; j++) {
            vector<int> values(d);
            for (int k = 0; k < d; k++) {
                complex<double> val = decrypted_training_batch[j * nb_cols + k];
                values[k] = (int) round(val.real());
            }

            Record* rcd = new Record(i * nb_rows + j, values, 0);
            records[i * nb_rows + j] = rcd;
            rcd->print();
        }
    }
    return records;
}


void PPLogisticRegression::pp_fit() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M");
    auto grads_file_name = "CIPHER_APPROX_GRADS_" + oss.str();
    auto weights_file_name = "CIPHER_WEIGHTS_" + oss.str();

    ofstream grads_log_file;
    ofstream weights_log_file;

    grads_log_file.open(grads_file_name);
    weights_log_file.open(weights_file_name);

    for (int e = 0; e < epochs; e++) {
        Ciphertext cipher_grad;
        complex<double> *encoded_grad = new complex<double>[n];
        for (int i = 0; i < n; i++) {
            complex<double> c;
            c.imag(0);
            c.real(0);
            encoded_grad[i] = c;
        }
        scheme.encrypt(cipher_grad, encoded_grad, n, logp, logq - 7 * logp);

        for (int i = 0; i < nb_training_ciphers; i++) {
            Ciphertext cipher_product;
            //Ciphertext cipher_dot_product;
            /*
            complex<double> *hhh = scheme.decrypt(secretKey, cipher_training_set);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < nb_cols; j++) {
                    cout << hhh[i * nb_cols + j] << ", ";
                }
                cout << " " << endl;
            }
            cout << " " << endl;

            cout << "wa shit" << endl;*/
            scheme.mult(cipher_product, cipher_model, cipher_training_set[i]);   // TODO : modify
            scheme.reScaleByAndEqual(cipher_product, logp);

            /*
            complex<double> * deb = scheme.decrypt(secretKey, cipher_product);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < nb_cols; j++) {
                    cout << deb[i * nb_cols + j] << ", ";
                }
                cout << " " << endl;
            }
            cout << " " << endl;

            */
            Ciphertext cipher_dot_product = sum_slots(cipher_product, 0, log_nb_cols);
            /*
            deb = scheme.decrypt(secretKey, cipher_dot_product);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < nb_cols; j++) {
                    cout << deb[i * nb_cols + j] << ", ";
                }
                cout << " " << endl;
            }
            cout << " " << endl;*/


            scheme.multAndEqual(cipher_dot_product, cipher_gadget_matrix);

            scheme.reScaleByAndEqual(cipher_dot_product, logp);

            Ciphertext cipher_dot_product_duplicated = sum_slots_reversed(cipher_dot_product, 0, log_nb_cols);

            Ciphertext cipher_sig = pp_sigmoid_deg3(cipher_dot_product_duplicated);

            /*complex<double> * eups = scheme.decrypt(secretKey, cipher_sig);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < nb_cols; j++) {
                    cout << eups[i * nb_cols + j] << ", ";
                }
                cout << " " << endl;
            }
            cout << " " << endl;*/


            scheme.multAndEqual(cipher_sig, cipher_training_set[i]); // TODO : modify
            scheme.reScaleByAndEqual(cipher_sig, logp);

            Ciphertext cipher_partial_grad = sum_slots(cipher_sig, log_nb_cols, log_nb_rows + log_nb_cols);
            cout << "CHOUF, EKUUUUTH " << cipher_partial_grad.logq << endl;
            scheme.addAndEqual(cipher_grad, cipher_partial_grad);
            /*complex<double> * lol = scheme.decrypt(secretKey, cipher_grad);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < nb_cols; j++) {
                    cout << lol[i * nb_cols + j] << ", ";
                }
                cout << " " << endl;
            }
            cout << " " << endl;*/
        }

        scheme.multByConstAndEqual(cipher_grad, alpha / m, logp);
        scheme.reScaleByAndEqual(cipher_grad, logp);            //TODO: factor out

        cout << "Gradient n : " << e << endl;

        complex<double> * grad = scheme.decrypt(secretKey, cipher_grad);
        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < d; k++) {
                grads_log_file << grad[j * nb_cols + k].real() << ", ";
            }
            grads_log_file << "" << endl;
        }
        cout << " " << endl;

        //scheme.encrypt(cipher_grad, grad, n, logp, logq);

        Ciphertext refreshed_grad = refresh_cipher(cipher_grad);
        scheme.addAndEqual(cipher_model, refreshed_grad);

        complex<double> * weights = scheme.decrypt(secretKey, cipher_model);

        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < d; k++) {
                weights_log_file << weights[j * nb_cols + k].real() << ", ";
            }
            weights_log_file << "" << endl;
        }
        cout << " " << endl;
        //Ciphertext cipher_model_bis = refresh_cipher(cipher_model);
        //cipher_model = cipher_model_bis;
        //complex<double> * plaintext = scheme.decrypt(secretKey, cipher_model);
        //Ciphertext new_model;
        //cipher_model.free();
        //scheme.encrypt(new_model, plaintext, n, logp, logq);
    }
    grads_log_file.close();
    weights_log_file.close();
}


Ciphertext PPLogisticRegression::refresh_cipher_unsecure(Ciphertext c) {
    complex<double> * plaintext = scheme.decrypt(secretKey, c);
    Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, plaintext, n, logp, logq);
    return refreshed_c;
}

Ciphertext PPLogisticRegression::refresh_cipher(Ciphertext c) {
    complex<double> * randomness = EvaluatorUtils::randomComplexArray(n);
    Ciphertext encrypted_randomness;
    Ciphertext encrypted_randomness_down;
    scheme.encrypt(encrypted_randomness, randomness, n, logp, logq);
    scheme.modDownBy(encrypted_randomness_down, encrypted_randomness, logq - c.logq);

    Ciphertext blinded_cipher;
    scheme.add(blinded_cipher, c, encrypted_randomness_down);

    complex<double> * blinded_plaintext = scheme.decrypt(secretKey, blinded_cipher);
    Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, blinded_plaintext, n, logp, logq);

    scheme.subAndEqual(refreshed_c, encrypted_randomness);
    return refreshed_c;
}


Ciphertext PPLogisticRegression::sum_slots(Ciphertext c, int start_slot, int end_slot) {
    Ciphertext cipher_sum = c;
    for (int i = start_slot; i < end_slot; i++) {
        Ciphertext cipher_rot;
        //cout << "Rotating by : " << (int) pow(2, i) << endl;
        scheme.leftRotateFast(cipher_rot, cipher_sum, (int) pow(2, i));/// TODO: Consider iterative update
        //complex<double> * decrypted_sum = scheme.decrypt(secretKey, cipher_sum);
        scheme.addAndEqual(cipher_sum, cipher_rot);
    }
    return cipher_sum;
}

Ciphertext PPLogisticRegression::sum_slots_reversed(Ciphertext c, int start_slot, int end_slot) {
    Ciphertext cipher_sum = c;
    for (int i = start_slot; i < end_slot; i++) {
        Ciphertext cipher_rot;
        //cout << "Rotating by : " << (int) pow(2, i) << endl;
        scheme.rightRotateFast(cipher_rot, cipher_sum, (int) pow(2, i));   // TODO: Consider iterative update
        //complex<double> * decrypted_sum = scheme.decrypt(secretKey, cipher_sum);
        scheme.addAndEqual(cipher_sum, cipher_rot);
    }
    return cipher_sum;
}


void PPLogisticRegression::test_sum_slots() {
    complex<double> *encoded_x;
    vector<double> x{1, 1, 1, 1};
    Ciphertext cipher_x;
    long nb = x.size();
    encoded_x = new complex<double>[n];

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_x[i] = c;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < nb_cols; j++) {
            complex<double> c;
            c.imag(0);
            c.real(i);
            encoded_x[i * nb_cols + j] = c;
        }
    }

    scheme.encrypt(cipher_x, encoded_x, n, logp, logq);

    Ciphertext cipher_sum = sum_slots(cipher_x, 0, log_nb_cols);
    scheme.multAndEqual(cipher_sum, cipher_gadget_matrix);
    scheme.reScaleByAndEqual(cipher_sum, logp);

    Ciphertext duplicated = sum_slots_reversed(cipher_sum, 0, log_nb_cols);

    complex<double> * decrypted_sum = scheme.decrypt(secretKey, duplicated);
    cout << " " << endl;

    cout << "Decrypted sums:" << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < nb_cols; j++) {
            cout << decrypted_sum[i * nb_cols + j] << ", ";
        }
        cout << " " << endl;
    }
    cout << " " << endl;
}

Ciphertext PPLogisticRegression::pp_sigmoid_deg3(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;

    complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    cout << "Input: ";
    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);

    cout << "First Mul: ";
    scheme.mult(cipher_x_cube, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    eussou = scheme.decrypt(secretKey, cipher_x_cube);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "Second Mul: ";
    scheme.mult(cipher_x_cube, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    eussou = scheme.decrypt(secretKey, cipher_x_cube);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "First Const Mul: ";
    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg3[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    eussou = scheme.decrypt(secretKey, cipher_ax);
    //scheme.encrypt(cipher_ax, eussou, n, logp, logq);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "Second Const Mul: ";


    scheme.multByConst(cipher_sig, cipher_x_cube, sigmoid_coeffs_deg3[2], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    eussou = scheme.decrypt(secretKey, cipher_sig);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "Const add: ";

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg3[0], logp);

    eussou = scheme.decrypt(secretKey, cipher_sig);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;
    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);

    cout << "ADD: ";

    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, 2 * logp);

    scheme.add(cipher_result, cipher_sig, cipher_ax);

    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << endl;

    eussou = scheme.decrypt(secretKey, cipher_result);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    return cipher_result;
}

double PPLogisticRegression::approx_sigmoid_deg3(double x) {
    return sigmoid_coeffs_deg3[0] + sigmoid_coeffs_deg3[1] * x + sigmoid_coeffs_deg3[2] * x * x * x;
}

void PPLogisticRegression::test_pp_sigmoid(vector<double> x) {
    complex<double> *encoded_x;
    Ciphertext cipher_x;
    long nb = x.size();
    encoded_x = new complex<double>[n];

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_x[i] = c;
    }

    for (int i = 0; i < nb; i++) {
        complex<double> c;
        c.imag(0);
        c.real(x[i]);
        encoded_x[i] = c;
    }

    scheme.encrypt(cipher_x, encoded_x, n, logp, logq);
    Ciphertext cipher_sig = pp_sigmoid_deg3(cipher_x);

    complex<double> *decrypted_encoded_sigmoids = scheme.decrypt(secretKey, cipher_sig);
    vector<double> decrypted_sig(nb);
    cout << "Values after decryption: " << endl;
    for (int i = 0; i < nb; i++) {
        decrypted_sig[i] = decrypted_encoded_sigmoids[i].real();
        cout << decrypted_sig[i] << ", ";
    }
    cout << " " << endl;

    cout << "Original Sigmoid Values" << endl;
    for (int i = 0; i < nb; i++) {
        double plain_sig = LogisticRegression::sigmoid(x[i]);
        cout << plain_sig << ", ";
    }
    cout << " " << endl;
    cout << "Plaintext Sigmoid Approx" << endl;
    for (int i = 0; i < nb; i++) {
        double approx_sig = approx_sigmoid_deg3(x[i]);
        cout << approx_sig << ", ";
    }
}

Ciphertext PPLogisticRegression::pp_dot_product(Ciphertext cx, Ciphertext cy) {
    Ciphertext cipher_prod;
    scheme.mult(cipher_prod, cx, cy);
    scheme.reScaleByAndEqual(cipher_prod, logp);

    complex<double> *decrypted_product = scheme.decrypt(secretKey, cipher_prod);
    for (int i = 0; i < n; i++) {
        cout << decrypted_product[i] << ", ";
    }
    cout << " " << endl;

    Ciphertext dot_product = sum_slots(cipher_prod, 0, 10);

    complex<double> *decrypted_dot_product = scheme.decrypt(secretKey, dot_product);
    for (int i = 0; i < n; i++) {
        cout << decrypted_dot_product[i] << ", ";
    }
    cout << " " << endl;
    return dot_product;
}


void PPLogisticRegression::test_pp_dot_product(vector<double> x, vector<double> y) {
    Ciphertext cx, cy;
    complex<double> *encoded_x, *encoded_y;

    long nb = x.size();
    encoded_x = new complex<double>[n];
    encoded_y = new complex<double>[n];
    for (int i = 0; i < nb; i++) {
        complex<double> c;
        c.imag(0);
        c.real(x[i]);
        encoded_x[i] = c;
    }

    for (int i = 0; i < nb; i++) {
        complex<double> c;
        c.imag(0);
        c.real(y[i]);
        encoded_y[i] = c;
    }

    scheme.encrypt(cx, encoded_x, n, logp, logq);
    scheme.encrypt(cy, encoded_y, n, logp, logq);

    Ciphertext cipher_dot_product = pp_dot_product(cx, cy);

    complex<double> *decrypted_dot_product = scheme.decrypt(secretKey, cipher_dot_product);
    for (int i = 0; i < n; i++) {
        cout << decrypted_dot_product[i] << ", ";
    }
    cout << " " << endl;
}

void PPLogisticRegression::test_cryptosystem_switching() {
    DTPKC::Cipher dtpkc_value;

    double val = 1.11111;
    long val_scaled = (long) (val * dtpkc_scale_factor);
    mpz_class mpz_value;
    mpz_value.set_str(std::to_string(val_scaled), 10);
    dtpkc_value = dtpkc.enc(mpz_value, dtpkc_pkey);

    chrono::high_resolution_clock::time_point start_key_exchange = chrono::high_resolution_clock::now();

    Ciphertext fhe_value = cryptosystem_switching_single(dtpkc_value);

    complex<double> *encoded_value = scheme.decrypt(secretKey, fhe_value);


    cout << "Encoded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << encoded_value[i] << ' ';
    }
    cout << " " << endl;

    chrono::high_resolution_clock::time_point end_key_exchange = chrono::high_resolution_clock::now();
    auto duration_exchange = chrono::duration_cast<chrono::milliseconds>(end_key_exchange - start_key_exchange).count();
    cout << "Protocol V2 takes : " << duration_exchange / 1000 << "s." << endl;
}


Ciphertext PPLogisticRegression::cryptosystem_switching_single(DTPKC::Cipher dtpkc_value) {
    DTPKC::Cipher dtpkc_randomness;
    DTPKC::Cipher dtpkc_value_blinded;

    long randomness = rand();

    mpz_class mpz_randomness;
    mpz_randomness.set_str(std::to_string(randomness), 10);
    mpz_class mpz_value;


    mpz_randomness.set_str(std::to_string(randomness), 10);
    dtpkc_randomness = dtpkc.enc(mpz_randomness, dtpkc_pkey);

    EvalAdd add(dtpkc_value, dtpkc_randomness);

    dtpkc_value_blinded = add.EvalAdd_U1();

    mpz_class mpz_value_blinded = dtpkc.Sdec(dtpkc_value_blinded);
    long decrypted_value_blinded = mpz_value_blinded.get_si();
    double rescaled_decrypted_value_blinded = (double) decrypted_value_blinded / dtpkc_scale_factor;
    double rescaled_randomness = (double) randomness / dtpkc_scale_factor;

    cout << "Blinded value after decryption:" << decrypted_value_blinded << endl;

    cout << "When you remove noise : " << decrypted_value_blinded - randomness << endl;

    cout << "Rescaled Blinded Decrypted Value : " << rescaled_decrypted_value_blinded << endl;
    cout << "Rescaled when you remove noise : " << rescaled_decrypted_value_blinded - rescaled_randomness << endl;


    Ciphertext fhe_value, fhe_rescaled_randomness, fhe_rescaled_value_blinded;
    complex<double> *encoded_randomness = new complex<double>[n];
    complex<double> *encoded_value_blinded = new complex<double>[n];

    {
        complex<double> c;
        c.real(rescaled_randomness);
        c.imag(0);
        encoded_randomness[0] = c;
    }
    {
        complex<double> c;
        c.real(rescaled_decrypted_value_blinded);
        c.imag(0);
        encoded_value_blinded[0] = c;
    }
    for (int i = 1; i < n; ++i) {
        complex<double> c;
        c.real(0);
        c.imag(0);
        encoded_randomness[i] = c;
        encoded_value_blinded[i] = c;
    }

    scheme.encrypt(fhe_rescaled_value_blinded, encoded_value_blinded, n, logp, logq);

    scheme.encrypt(fhe_rescaled_randomness, encoded_randomness, n, logp, logq);

    complex<double> *decrypted_blinded = scheme.decrypt(secretKey, fhe_rescaled_value_blinded);

    cout.precision(10);
    cout << "Decrypted Rescaled Encoded blinded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << decrypted_blinded[i] << ' ';
    }
    cout << " " << endl;
    scheme.sub(fhe_value, fhe_rescaled_value_blinded, fhe_rescaled_randomness);

    return fhe_value;
}


Ciphertext PPLogisticRegression::cryptosystem_switching_batch_naive(vector<DTPKC::Cipher> dtpkc_values) {
    long nb_vals = dtpkc_values.size();

    Ciphertext fhe_value, fhe_rescaled_randomness, fhe_rescaled_value_blinded;
    complex<double> *encoded_randomness = new complex<double>[n];
    complex<double> *encoded_value_blinded = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(0);
        c.imag(0);
        encoded_randomness[i] = c;
        encoded_value_blinded[i] = c;
    }

    for (int i = 0; i < nb_vals; i++) {
        DTPKC::Cipher dtpkc_randomness;
        DTPKC::Cipher dtpkc_value = dtpkc_values[i];
        DTPKC::Cipher dtpkc_value_blinded;

        long randomness = rand();
        mpz_class mpz_randomness;
        mpz_randomness.set_str(std::to_string(randomness), 10);
        mpz_class mpz_value;


        mpz_randomness.set_str(std::to_string(randomness), 10);
        dtpkc_randomness = dtpkc.enc(mpz_randomness, dtpkc_pkey);

        EvalAdd add(dtpkc_value, dtpkc_randomness);

        dtpkc_value_blinded = add.EvalAdd_U1();

        mpz_class mpz_value_blinded = dtpkc.Sdec(dtpkc_value_blinded);
        long decrypted_value_blinded = mpz_value_blinded.get_si();
        double rescaled_decrypted_value_blinded = (double) decrypted_value_blinded / dtpkc_scale_factor;
        double rescaled_randomness = (double) randomness / dtpkc_scale_factor;

        cout << "Blinded value after decryption:" << decrypted_value_blinded << endl;

        cout << "When you remove noise : " << decrypted_value_blinded - randomness << endl;

        cout << "Rescaled Blinded Decrypted Value : " << rescaled_decrypted_value_blinded << endl;
        cout << "Rescaled when you remove noise : " << rescaled_decrypted_value_blinded - rescaled_randomness << endl;

        {
            complex<double> c;
            c.real(rescaled_randomness);
            c.imag(0);
            encoded_randomness[i] = c;
        }

        {
            complex<double> c;
            c.real(rescaled_decrypted_value_blinded);
            c.imag(0);
            encoded_value_blinded[i] = c;
        }

    }

    scheme.encrypt(fhe_rescaled_value_blinded, encoded_value_blinded, n, logp, logq);

    scheme.encrypt(fhe_rescaled_randomness, encoded_randomness, n, logp, logq);

    complex<double> *decrypted_blinded = scheme.decrypt(secretKey, fhe_rescaled_value_blinded);

    cout.precision(10);
    cout << "Decrypted Rescaled Encoded blinded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << decrypted_blinded[i] << ' ';
    }
    cout << " " << endl;
    scheme.sub(fhe_value, fhe_rescaled_value_blinded, fhe_rescaled_randomness);

    return fhe_value;
}



Ciphertext PPLogisticRegression::cryptosystem_switching_batch_optimized(vector<DTPKC::Cipher> dtpkc_values) {
    long nb_vals = dtpkc_values.size();
    int dtpkc_nb_slots = dtpkc.nb_slots;

    Ciphertext fhe_value, fhe_rescaled_randomness, fhe_rescaled_value_blinded;
    complex<double> *encoded_randomness = new complex<double>[n];
    complex<double> *encoded_value_blinded = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(0);
        c.imag(0);
        encoded_randomness[i] = c;
        encoded_value_blinded[i] = c;
    }

    for (int i = 0; i < nb_vals; i++) {
        DTPKC::Cipher dtpkc_randomness;
        DTPKC::Cipher dtpkc_value = dtpkc_values[i];
        DTPKC::Cipher dtpkc_value_blinded;

        vector<long> randomness_vec(dtpkc_nb_slots);
        vector<double> rescaled_randomness_vec(dtpkc_nb_slots);
        for (int j = 0; j < dtpkc_nb_slots; j++) {
            randomness_vec[j] = rand();
            rescaled_randomness_vec[j] = ((double) randomness_vec[j]) / (dtpkc_scale_factor * 1.0);
        }

        dtpkc_randomness = dtpkc.enc_batch(randomness_vec, dtpkc_pkey);

        mpz_class mpz_value;

        EvalAdd add(dtpkc_value, dtpkc_randomness);

        dtpkc_value_blinded = add.EvalAdd_U1();

        vector<long> value_blinded_vec = dtpkc.decrypt_batch(dtpkc_value_blinded);
        cout << "gege" << endl;
        for (int i = 0; i < value_blinded_vec.size(); ++i) {
            cout << value_blinded_vec[i] << ' ';
        }
        cout << " " << endl;
        vector<double> rescaled_decrypted_value_blinded_vec(dtpkc_nb_slots);                                 // dsl g paike lol

        for (int j = 0; j < dtpkc_nb_slots; j++) {
            rescaled_decrypted_value_blinded_vec[j] = ((double)value_blinded_vec[j]) / dtpkc_scale_factor;
        }

        for (int j = 0; j < dtpkc_nb_slots; j++) {
            {
                complex<double> c;
                c.real(rescaled_randomness_vec[j]);
                c.imag(0);
                encoded_randomness[i * dtpkc_nb_slots + j] = c;
            }

            {
                complex<double> c;
                c.real(rescaled_decrypted_value_blinded_vec[j]);
                c.imag(0);
                encoded_value_blinded[i * dtpkc_nb_slots + j] = c;
            }
        }
    }

    cout << "Assubido lamareca" << endl;
    for (int j = 0; j < n; j++) {
        cout << encoded_value_blinded[j] << " ";
    }
    cout << "" << endl;


    scheme.encrypt(fhe_rescaled_value_blinded, encoded_value_blinded, n, logp, logq);

    complex<double> *decrypted_blinded = scheme.decrypt(secretKey, fhe_rescaled_value_blinded);

    cout << "wtf:" << endl;
    for (int j = 0; j < n; ++j) {
        cout << decrypted_blinded[j] << ' ';
    }
    cout << " " << endl;

    scheme.encrypt(fhe_rescaled_randomness, encoded_randomness, n, logp, logq);


    scheme.sub(fhe_value, fhe_rescaled_value_blinded, fhe_rescaled_randomness);

    complex<double> *decrypted_fhe_value = scheme.decrypt(secretKey, fhe_value);

    cout << "Decrypted values:" << endl;
    for (int j = 0; j < n; ++j) {
        cout << decrypted_fhe_value[j] << ' ';
    }
    cout << " " << endl;

    return fhe_value;
}



void PPLogisticRegression::test_cryptosystem_switching_batch_naive() {
    vector<double> values{1.5, 1.411, 1.1111, 2.55555, 2.66666};
    int nb_values = values.size();
    vector<long> scaled_values(nb_values);

    for (int i = 0; i < nb_values; i++) {
        scaled_values[i] = (long) (values[i] * dtpkc_scale_factor);
    }

    vector<DTPKC::Cipher> dtpkc_values(nb_values);
    for (int i = 0; i < nb_values; i++) {
        long scaled_value = scaled_values[i];
        mpz_class mpz_value;
        mpz_value.set_str(std::to_string(scaled_value), 10);
        DTPKC::Cipher dtpkc_value = dtpkc.enc(mpz_value, dtpkc_pkey);
        dtpkc_values[i] = dtpkc_value;
    }

    Ciphertext fhe_value = cryptosystem_switching_batch_naive(dtpkc_values);

    complex<double> *encoded_value = scheme.decrypt(secretKey, fhe_value);

    cout << "Encoded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << encoded_value[i] << ' ';
    }
    cout << " " << endl;
}


void PPLogisticRegression::test_cryptosystem_switching_batch_optimized() {
    vector<double> values{1.5, 1.411, 1.1111, 2.55555, 2.6666, 4, 2.3333, 6.666, 8.9999};
    int nb_values = values.size();
    int nb_ciphers = 1;
    vector<long> scaled_values(nb_values);
    int dtpkc_nb_slots = dtpkc.nb_slots;

    for (int i = 0; i < nb_values; i++) {
        scaled_values[i] = (long) (values[i] * dtpkc_scale_factor);
    }

    vector<DTPKC::Cipher> dtpkc_values;
    for (int i = 0; i < nb_ciphers; i++) {
        vector<long> scaled_slot(dtpkc_nb_slots);
        for (int j = 0; j < dtpkc_nb_slots; j++) {
            scaled_slot[j] = scaled_values[i * dtpkc_nb_slots + j];
        }
        DTPKC::Cipher dtpkc_value = dtpkc.enc_batch(scaled_slot, dtpkc_pkey);
        vector<long> lol = dtpkc.decrypt_batch(dtpkc_value);
        cout << "Just checking:" << endl;
        for (int i = 0; i < lol.size(); ++i) {
            cout << lol[i] << ' ';
        }
        dtpkc_values.push_back(dtpkc_value);
    }
    
    Ciphertext fhe_value = cryptosystem_switching_batch_optimized(dtpkc_values);

    complex<double> *encoded_value = scheme.decrypt(secretKey, fhe_value);

    cout << "Encoded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << encoded_value[i] << ' ';
    }
    cout << " " << endl;
}