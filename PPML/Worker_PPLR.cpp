//
// Created by Nassim AAB on 2019-07-27.
//

#include "Worker_PPLR.h"

#include <unistd.h>



Worker_PPLR::Worker_PPLR(void) : secretKey(ring), scheme(secretKey, ring, false) {

    // Generate the cryptosystem keys
    SetNumThreads(numThread);

    //SecretKey secretKey(ring);
    //Scheme scheme(secretKey, ring);
    //scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
    //scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message

    //scheme.read_keys();

    complex<double> *mvec1 = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(1);
        c.imag(0);
        mvec1[i] = c;
    }

    Ciphertext cipher1;

    scheme.encrypt(cipher1, mvec1, n, logp, logq);

    complex<double> *dvec = scheme.decrypt(secretKey, cipher1);

    cout << "FHE decrypt result:" << endl;
    for (int i = 0; i < d; ++i) {
        cout << dvec[i] << ' ';
    }
    cout << " " << endl;


    // Next..
    cout << "Tout est bon" << endl;
    //

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
    scheme.encrypt(cipher_gadget_matrix, encoded_gadget_matrix, n, logp, logq);    //TODO : what's happening ?

    connect_to_mlsp();

    /*SerializationUtils::writeCiphertext(cipher1, "test.txt");

    unsigned int microseconds = 3000000;

    usleep(microseconds);

    if (send_file(mlsp_sock, "test.txt")) {
        cout << "gneu" << endl;
    } else {
        cout << "tfou" << endl;
    }*/

}



bool Worker_PPLR::send_data(int sock, void *buf, int buflen)
{
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0)
    {
        int num = send(sock, pbuf, buflen, 0);

        pbuf += num;
        buflen -= num;
    }

    return true;
}


bool Worker_PPLR::send_long(int sock, long value)
{
    value = htonl(value);
    return send_data(sock, &value, sizeof(value));
}



bool Worker_PPLR::send_file(int sock, char* path)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    rewind(f);
    if (filesize == EOF)
        return false;
    if (!send_long(sock, filesize))
        return false;
    if (filesize > 0)
    {
        cout << "Sending a file of size : " << filesize << endl;
        char buffer[1024];
        do
        {
            size_t num = (filesize < sizeof(buffer)) ?  filesize : sizeof(buffer);
            num = fread(buffer, 1, num, f);
            if (num < 1)
                return false;
            if (!send_data(sock, buffer, num))
                return false;
            filesize -= num;
        }
        while (filesize > 0);
    }
    fclose(f);
    return true;
}



bool Worker_PPLR::read_data(int sock, void *buf, int buflen)
{
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0)
    {
        int num = recv(sock, pbuf, buflen, 0);
        if (num == 0)
            return false;

        pbuf += num;
        buflen -= num;
    }

    return true;
}

bool Worker_PPLR::read_long(int sock, long *value)
{
    if (!read_data(sock, value, sizeof(value)))
        return false;
    *value = ntohl(*value);
    return true;
}


bool Worker_PPLR::read_file(int sock, char* path)
{
    FILE *f = fopen(path, "wb");
    long filesize;
    if (!read_long(sock, &filesize)) {
        cout << "lol" << endl;
        return false;
    }
    if (filesize > 0)
    {
        cout << "Getting a file of size : " << filesize << endl;
        char buffer[1024];
        do
        {
            int num = (filesize < sizeof(buffer)) ?  filesize : sizeof(buffer);
            if (!read_data(sock, buffer, num))
                return false;
            int offset = 0;
            do
            {
                size_t written = fwrite(&buffer[offset], 1, num - offset, f);
                if (written < 1)
                    return false;
                offset += written;
            }
            while (offset < num);
            filesize -= num;
        }
        while (filesize > 0);
    }
    fclose(f);
    return true;
}


void Worker_PPLR::connect_to_mlsp() {
    if ((mlsp_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    mlsp_addr.sin_family = AF_INET;
    mlsp_addr.sin_port = htons(mlsp_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, mlsp_ip, &mlsp_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(mlsp_sock, (struct sockaddr *)&mlsp_addr, sizeof(mlsp_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
    //send_long(mlsp_sock, 53);
}


// ------------------------------------ LOGISTIC REGRESSION -------------------------------------------

void Worker_PPLR::encrypt_dataset() {
    DatasetReader *datasetReader = new DatasetReader(datasets_path + dataset_name + "/", class_number, d);
    complex<double> *vectorized_batch = new complex<double>[nb_slots];
    vector<Record*> training_data = datasetReader->read_all_train_records();

    for (int i = start_record; i < m; i++) {
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
            Record *rcd = training_data[start_record + i * nb_rows + j];
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

vector<Record*> Worker_PPLR::decrypt_dataset() {
    vector<Record*> records(m);
    for (int i = 0; i < nb_training_ciphers; i++) {
        complex<double> *decrypted_training_batch = scheme.decrypt(secretKey, cipher_training_set[i]);
        for (int j = 0; j < nb_rows; j++) {
            vector<double> values(d);
            for (int k = 0; k < d; k++) {
                complex<double> val = decrypted_training_batch[j * nb_cols + k];
                values[k] = val.real();
            }

            Record* rcd = new Record(i * nb_rows + j, values, 0);
            records[i * nb_rows + j] = rcd;
            rcd->print();
        }
    }
    return records;
}

Ciphertext Worker_PPLR::refresh_cipher_local_unsecure(Ciphertext c) {
    complex<double> * plaintext = scheme.decrypt(secretKey, c);
    Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, plaintext, n, logp, logq);
    return refreshed_c;
}



/*void Worker_PPLR::refresh_cipher() {
    read_file(new_socket, "cipher_to_refresh.txt");
    Ciphertext* cipher_to_refresh = SerializationUtils::readCiphertext("cipher_to_refresh.txt");

    complex<double> * plaintext = scheme.decrypt(secretKey, *cipher_to_refresh);

    cout << "Plaintext value of the cipher to refresh:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext[i] << ' ';
    }
    cout << " " << endl;

    /*Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, plaintext, n, logp, logq);
    SerializationUtils::writeCiphertext(*cipher_to_refresh, "refreshed_cipher.txt");

    if (send_file(new_socket, "refreshed_cipher.txt")) {
        cout << "Sent the refreshed cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }
}*/



/*void Worker_PPLR::debug() {
    read_file(new_socket, "cipher1.txt");
    Ciphertext* cipher1 = SerializationUtils::readCiphertext("cipher1.txt");

    read_file(new_socket, "cipher2.txt");
    Ciphertext* cipher2 = SerializationUtils::readCiphertext("cipher2.txt");

    read_file(new_socket, "product.txt");
    Ciphertext* product = SerializationUtils::readCiphertext("product.txt");

    read_file(new_socket, "cipher_sig.txt");
    Ciphertext* cipher_sig = SerializationUtils::readCiphertext("cipher_sig.txt");

    Ciphertext cipher_product, cipher_sum, cipher_rot, cipher_cst_product;
    scheme.mult(cipher_product, *cipher1, *cipher2);
    scheme.reScaleByAndEqual(cipher_product, logp);

    scheme.add(cipher_sum, *cipher1, *cipher2);

    scheme.leftRotateFast(cipher_rot, *cipher1, 2);
    scheme.multByConst(cipher_cst_product, *cipher1, 3.0, logp);
    scheme.reScaleByAndEqual(cipher_cst_product, logp);

    complex<double> * plaintext_cipher1 = scheme.decrypt(secretKey, *cipher1);

    cout << "Plaintext value of cipher1:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_cipher1[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * plaintext_cipher2 = scheme.decrypt(secretKey, *cipher2);

    cout << "Plaintext value of cipher2:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_cipher2[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * prod = scheme.decrypt(secretKey, *product);

    cout << "Plaintext value of prod:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << prod[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * sig = scheme.decrypt(secretKey, *cipher_sig);

    cout << "Plaintext value of sig:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << sig[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * plaintext_sum = scheme.decrypt(secretKey, cipher_sum);

    cout << "Plaintext value of add:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_sum[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * plaintext_product = scheme.decrypt(secretKey, cipher_product);

    cout << "Plaintext value of mult:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_product[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * plaintext_cst_product = scheme.decrypt(secretKey, cipher_cst_product);

    cout << "Plaintext value of cst mult:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_cst_product[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * plaintext_rot = scheme.decrypt(secretKey, cipher_rot);

    cout << "Plaintext value of rot:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_rot[i] << ' ';
    }
    cout << " " << endl;

    Ciphertext recrypted_cipher1, recrypted_cipher2, recrypted_prod;
    scheme.encrypt(recrypted_cipher1, plaintext_cipher1, n, logp, logq);
    scheme.encrypt(recrypted_cipher2, plaintext_cipher2, n, logp, logq);
    scheme.encrypt(recrypted_prod, prod, n, logp, logq);

    SerializationUtils::writeCiphertext(recrypted_cipher1, "recrypted_cipher1.txt");
    if (send_file(new_socket, "recrypted_cipher1.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    SerializationUtils::writeCiphertext(recrypted_cipher2, "recrypted_cipher2.txt");
    if (send_file(new_socket, "recrypted_cipher2.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    SerializationUtils::writeCiphertext(recrypted_prod, "recrypted_prod.txt");
    if (send_file(new_socket, "recrypted_prod.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    SerializationUtils::writeCiphertext(cipher_rot, "cipher_rot.txt");
    if (send_file(new_socket, "cipher_rot.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

}

void Worker_PPLR::pp_fit() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M");
    auto weights_file_name = "CIPHER_WEIGHTS_" + oss.str();

    ofstream weights_log_file;

    weights_log_file.open(weights_file_name);

    for (int e = 0; e < epochs; e++) {
        refresh_cipher();
        read_file(new_socket, "cipher_weights.txt");
        Ciphertext *cipher_model = SerializationUtils::readCiphertext("cipher_weights.txt");

        complex<double> * weights = scheme.decrypt(secretKey, *cipher_model);

        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < d; k++) {
                weights_log_file << weights[j * nb_cols + k].real() << ", ";
            }
            weights_log_file << "" << endl;
        }

    }
    weights_log_file.close();
}*/

Ciphertext Worker_PPLR::pp_sigmoid_deg5(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_square;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;
    Ciphertext cipher_x_four;
    Ciphertext cipher_x_five;

    //complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);


    scheme.mult(cipher_square, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_square, logp);


    scheme.mult(cipher_x_cube, cipher_square, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    /*cout << " CUBE " << endl;
    complex<double> *decrypted_x_cube = scheme.decrypt(secretKey, cipher_x_cube);
    for (int i = 0; i < n; i++) {
        cout << decrypted_x_cube[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.mult(cipher_x_four, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_four, logp);

    /*cout << " FOUR " << endl;
    complex<double> *decrypted_x_four = scheme.decrypt(secretKey, cipher_x_four);
    for (int i = 0; i < n; i++) {
        cout << decrypted_x_four[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.mult(cipher_x_five, cipher_x_four, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_five, logp);

    /*cout << " FIVE " << endl;
    complex<double> *decrypted_x_five = scheme.decrypt(secretKey, cipher_x_five);
    for (int i = 0; i < n; i++) {
        cout << decrypted_x_five[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg5[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    scheme.multByConst(cipher_ax_cube, cipher_x_cube, sigmoid_coeffs_deg5[2], logp);
    scheme.reScaleByAndEqual(cipher_ax_cube, logp);

    scheme.multByConst(cipher_sig, cipher_x_five, sigmoid_coeffs_deg5[3], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg5[0], logp);

    //eussou = scheme.decrypt(secretKey, cipher_sig);

    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);

    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, cipher_ax.logq - cipher_sig.logq);
    scheme.modDownByAndEqual(cipher_ax_cube, cipher_ax_cube.logq - cipher_sig.logq);
    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << " Cipher AX CUBE : " << cipher_ax_cube.logq << endl;

    scheme.add(cipher_result, cipher_sig, cipher_ax_cube);
    scheme.addAndEqual(cipher_result, cipher_ax);


    return cipher_result;
}

Ciphertext Worker_PPLR::pp_sigmoid_deg7(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_square;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_x_five;
    Ciphertext cipher_x_seven;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;
    Ciphertext cipher_ax_five;

    //complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);

    scheme.mult(cipher_square, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_square, logp);


    scheme.mult(cipher_x_cube, cipher_square, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    scheme.mult(cipher_x_five, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_five, logp);

    scheme.multAndEqual(cipher_x_five, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_five, logp);

    scheme.mult(cipher_x_seven, cipher_x_five, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_seven, logp);

    scheme.multAndEqual(cipher_x_seven, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_seven, logp);

    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg7[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    scheme.multByConst(cipher_ax_cube, cipher_x_cube, sigmoid_coeffs_deg7[2], logp);
    scheme.reScaleByAndEqual(cipher_ax_cube, logp);

    scheme.multByConst(cipher_ax_five, cipher_x_five, sigmoid_coeffs_deg7[3], logp);
    scheme.reScaleByAndEqual(cipher_ax_five, logp);

    scheme.multByConst(cipher_sig, cipher_x_seven, sigmoid_coeffs_deg7[4], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg7[0], logp);

    //eussou = scheme.decrypt(secretKey, cipher_sig);

    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);

    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, cipher_ax.logq - cipher_sig.logq);
    scheme.modDownByAndEqual(cipher_ax_cube, cipher_ax_cube.logq - cipher_sig.logq);
    scheme.modDownByAndEqual(cipher_ax_five, cipher_ax_five.logq - cipher_sig.logq);

    scheme.add(cipher_result, cipher_sig, cipher_ax_five);
    scheme.addAndEqual(cipher_result, cipher_ax_cube);
    scheme.addAndEqual(cipher_result, cipher_ax);

    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << endl;

    return cipher_result;
}

Ciphertext Worker_PPLR::pp_sigmoid(Ciphertext c, int degree) {
    if (degree == 3) {
        Ciphertext cipher_sig = pp_sigmoid_deg3(c);
        return cipher_sig;
    } else if (degree == 5) {
        Ciphertext cipher_sig = pp_sigmoid_deg5(c);
        return cipher_sig;
    }
    Ciphertext cipher_sig = pp_sigmoid_deg7(c);
    return cipher_sig;
}


void Worker_PPLR::pp_fit_local() {
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
        scheme.encrypt(cipher_grad, encoded_grad, n, logp, logq - 4 * logp - sigmoid_degree * logp);

        for (int i = 0; i < nb_training_ciphers; i++) {
            Ciphertext cipher_product;
            scheme.mult(cipher_product, cipher_model, cipher_training_set[i]);   // TODO : modify
            scheme.reScaleByAndEqual(cipher_product, logp);

            Ciphertext cipher_dot_product = sum_slots(cipher_product, 0, log_nb_cols);

            scheme.multAndEqual(cipher_dot_product, cipher_gadget_matrix);

            scheme.reScaleByAndEqual(cipher_dot_product, logp);

            Ciphertext cipher_dot_product_duplicated = sum_slots_reversed(cipher_dot_product, 0, log_nb_cols);

            Ciphertext cipher_sig = pp_sigmoid_deg3(cipher_dot_product_duplicated);

            scheme.multAndEqual(cipher_sig, cipher_training_set[i]); // TODO : modify
            scheme.reScaleByAndEqual(cipher_sig, logp);

            Ciphertext cipher_partial_grad = sum_slots(cipher_sig, log_nb_cols, log_nb_rows + log_nb_cols);
            scheme.addAndEqual(cipher_grad, cipher_partial_grad);

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

        Ciphertext refreshed_grad = refresh_cipher_local(cipher_grad);
        scheme.addAndEqual(cipher_model, refreshed_grad);

        complex<double> * weights = scheme.decrypt(secretKey, cipher_model);

        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < d; k++) {
                weights_log_file << weights[j * nb_cols + k].real() << ", ";
            }
            weights_log_file << "" << endl;
        }
        cout << " " << endl;
    }
    grads_log_file.close();
    weights_log_file.close();
}

/*void Worker_PPLR::test_refresh_cipher() {
    refresh_cipher();
    read_file(new_socket, "check_cipher.txt");
    Ciphertext* check_cipher = SerializationUtils::readCiphertext("check_cipher.txt");

    complex<double> * plaintext = scheme.decrypt(secretKey, *check_cipher);

    cout << "Plaintext value of the check cipher:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext[i] << ' ';
    }
    cout << " " << endl;
}*/

/*void Worker_PPLR::test_refresh_cipher_unsecure() {
    refresh_cipher_unsecure();
}*/



void Worker_PPLR::pp_fit() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M");
    auto grads_file_name = "CIPHER_APPROX_GRADS_" + oss.str();
    auto weights_file_name = "CIPHER_WEIGHTS_" + oss.str();

    string grad_filename = "worker" + to_string(id) + "_grad.txt";
    string updated_model_filename =  "updated_model.txt";

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
        scheme.encrypt(cipher_grad, encoded_grad, n, logp, logq - 4 * logp - sigmoid_degree * logp);

        for (int i = 0; i < nb_training_ciphers; i++) {
            Ciphertext cipher_product;

            scheme.mult(cipher_product, cipher_model, cipher_training_set[i]);   // TODO : modify
            scheme.reScaleByAndEqual(cipher_product, logp);

            Ciphertext cipher_dot_product = sum_slots(cipher_product, 0, log_nb_cols);


            scheme.multAndEqual(cipher_dot_product, cipher_gadget_matrix);

            scheme.reScaleByAndEqual(cipher_dot_product, logp);

            Ciphertext cipher_dot_product_duplicated = sum_slots_reversed(cipher_dot_product, 0, log_nb_cols);

            Ciphertext cipher_sig = pp_sigmoid(cipher_dot_product_duplicated, sigmoid_degree);


            scheme.multAndEqual(cipher_sig, cipher_training_set[i]); // TODO : modify
            scheme.reScaleByAndEqual(cipher_sig, logp);

            Ciphertext cipher_partial_grad = sum_slots(cipher_sig, log_nb_cols, log_nb_rows + log_nb_cols);

            /*complex<double> * grad = scheme.decrypt(secretKey, cipher_partial_grad);
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < d; k++) {
                    cout << grad[j * nb_cols + k].real() << ", ";
                }
            }
            cout << " " << endl;*/

            scheme.addAndEqual(cipher_grad, cipher_partial_grad);
        }

        // So ..


        /*complex<double> * grad = scheme.decrypt(secretKey, cipher_grad);
        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < d; k++) {
                cout << grad[j * nb_cols + k].real() << ", ";
            }
        }
        cout << " " << endl;*/

        //Ciphertext hihi;
        //scheme.encrypt(hihi, grad, n, logp, logq);

        SerializationUtils::writeCiphertext(cipher_grad, grad_filename.c_str());
        if (send_file(mlsp_sock, (char *) grad_filename.c_str())) {
            cout << "Sent the gradient" << endl;
        } else {
            cout << "ERROR while sending the cipher" << endl;
        }
        read_file(mlsp_sock, (char *) updated_model_filename.c_str());
        Ciphertext* updated_model = SerializationUtils::readCiphertext((char *) updated_model_filename.c_str());

        complex<double> * model = scheme.decrypt(secretKey, *updated_model);
        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < d; k++) {
                weights_log_file << model[j * nb_cols + k].real() << ", ";
            }
            weights_log_file << "" << endl;
        }
        cout << " " << endl;
        scheme.encrypt(cipher_model, model, n, logp, logq);
    }

    if (remove(grad_filename.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    if (remove(updated_model_filename.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    grads_log_file.close();
    weights_log_file.close();
}

Ciphertext Worker_PPLR::refresh_cipher_local(Ciphertext c) {
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

Ciphertext Worker_PPLR::sum_slots(Ciphertext c, int start_slot, int end_slot) {
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

Ciphertext Worker_PPLR::sum_slots_reversed(Ciphertext c, int start_slot, int end_slot) {
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

void Worker_PPLR::test_sum_slots() {
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

Ciphertext Worker_PPLR::pp_sigmoid_deg3(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;

    complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    /*cout << "Input: ";
    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);

    //cout << "First Mul: ";
    scheme.mult(cipher_x_cube, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    /*eussou = scheme.decrypt(secretKey, cipher_x_cube);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "Second Mul: ";*/
    scheme.mult(cipher_x_cube, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    /*eussou = scheme.decrypt(secretKey, cipher_x_cube);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "First Const Mul: ";*/
    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg3[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    /*eussou = scheme.decrypt(secretKey, cipher_ax);
    //scheme.encrypt(cipher_ax, eussou, n, logp, logq);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "Second Const Mul: ";*/


    scheme.multByConst(cipher_sig, cipher_x_cube, sigmoid_coeffs_deg3[2], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    /*eussou = scheme.decrypt(secretKey, cipher_sig);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;

    cout << "Const add: ";*/

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg3[0], logp);

    /*eussou = scheme.decrypt(secretKey, cipher_sig);

    for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;
    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);

    cout << "ADD: ";*/

    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, 2 * logp);

    scheme.add(cipher_result, cipher_sig, cipher_ax);

    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << endl;

    eussou = scheme.decrypt(secretKey, cipher_result);

    /*for (int i = 0; i < n; i++) {
        cout << eussou[i] << ", ";
    }
    cout << " " << endl;*/

    return cipher_result;
}

double Worker_PPLR::approx_sigmoid_deg3(double x) {
    return sigmoid_coeffs_deg3[0] + sigmoid_coeffs_deg3[1] * x + sigmoid_coeffs_deg3[2] * x * x * x;
}

void Worker_PPLR::test_pp_sigmoid(vector<double> x) {
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

Ciphertext Worker_PPLR::pp_dot_product(Ciphertext cx, Ciphertext cy) {
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









