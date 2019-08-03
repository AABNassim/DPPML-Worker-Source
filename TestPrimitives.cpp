#include "HEAAN.h"
#include "BENCHS/BENCHBuildingBlocks.h"
#include <gmp.h>
#include <vector>
#include "ML/DatasetReader.h"
#include "ML/LogisticRegression.h"
#include "PPML/PPLogisticRegression.h"
#include "CRYPTO/DTPKC.h"
#include "CRYPTO/EvalAdd.h"

#include "PPML/MLSP.h"
#include "PPML/CSP.h"
#include "PPML/Worker_PPLR.h"

#include <NTL/fileio.h>



using namespace std;
using namespace NTL;
using namespace std::chrono;

void print(std::vector<int> const &input)
{
    for (int i = 0; i < input.size(); i++) {
        std::cout << input[i] << ' ';
    }
}



int main() {
    //LogisticRegression *model = new LogisticRegression();
    //model->approx_fit();
    //return 0;
    srand(42);
    const std::string& id = "0-1564507236-14833-5318-0x10cdcb5c0";
    SetSeed((const unsigned char *) id.c_str(), id.length());


    Worker_PPLR* worker_pplr = new Worker_PPLR();
    worker_pplr->encrypt_dataset();
    worker_pplr->pp_fit();

    //cout << "Let's test the refresh procedure" << endl;
    //csp_pplr->test_refresh_cipher_unsecure();


    /*srand(42);
    SetNumThreads(numThread);
    TimeUtils timeutils;
    Ring ring;
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
    scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message

    Ciphertext cipher_deserialized(logp, logq, n);

    SerializationUtils serializationUtils;
    serializationUtils.readCiphertextModified(cipher_deserialized, "./cipher.txt");


    complex<double> *decrypted_deserialized = scheme.decrypt(secretKey, cipher_deserialized);


    cout << "Deserialization result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << decrypted_deserialized[i] << ' ';
    }
    cout << " " << endl;*/


    //CSP *csp = new CSP();
    //csp->test_read_file();
    //MLSP *mlsp = new MLSP();
    //pp_model->test_cryptosystem_switching_batch_naive();
    /*string path="/home/st/Bureau/DPPML/DATA/Datasets/Edin/";
    int class_number = 2;
    int dimension = 9;

    ofstream output_file("output.txt");


    DatasetReader *datasetReader = new DatasetReader(path, class_number, dimension);
    cout << datasetReader->test_size << endl;
    cout << datasetReader->train_size << endl;

    //LogisticRegression *model = new LogisticRegression();

    PPLogisticRegression *pp_model = new PPLogisticRegression();
    pp_model->encrypt_dataset();
    cout << "LOL" << endl;
    //pp_model->decrypt_dataset();
    vector<double> x {1.0, 1.0, 1.0, 1.0, 1, 1, 1, 1, 1};
    vector<double> y {2, 2, 1.0, 1.0, 1, 1, 1, 1, 1};
    //pp_model->test_pp_sigmoid(x);
    //pp_model->test_pp_dot_product(x, y);
    //pp_model->test_sum_slots();
    pp_model->pp_fit();

    LogisticRegression *model = new LogisticRegression();
    model->fit();
    //pp_model->pp_sigmoid();
    //model->fit();
    //vector<int> labels = model->test();

    //cout << "BOUH " << labels.size() << endl;

    //for (int i = 0; i < labels.size(); i++) {
    //    cout << labels[i];
    //}
    //cout << "" << endl;

    //if (output_file.is_open()) {
    //    for (int i = 0; i < labels.size(); i++) {
    //        output_file << labels[i] << endl;
    //    }
    //    output_file.close();
    //}


    //print(labels);
    /*cout << "MODEL: " << endl;
    for (int j = 0; j < 14; j++) {
        cout << model->w[j] << " ";
    }*/
    //cout << "" << endl;
    /*for (int k = 0; k < 10; k++) {
        Record* r = datasetReader->getTrainRecord();
        cout << "" << endl;
        print(r->values);
        cout << "" << endl;
        cout << "Class : " << r->label;

        r = datasetReader->getTestRecord();
        cout << "" << endl;
        print(r->values);
        cout << "" << endl;
        cout << "Class : " << r->label;
    }*/



    /*
    * Basic Parameters are in src/Params.h
    * If you want to use another parameter, you need to change src/Params.h file and re-complie this library.
    */

    // Parameters //
    /*long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long n = 1 << logn;
    long numThread = 8;

    cout << "EUSSOU" << endl;

    SerializationUtils *serializationUtils = new SerializationUtils();

    // Construct and Generate Public Keys //
    //srand(time(NULL));
    srand(42);
    SetNumThreads(numThread);
    TimeUtils timeutils;
    Ring ring;
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
    scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message


    gmp_randstate_t randstate;
    mpz_class pkey, skey;
    int precision = 30;
    int nb_bits = 1024, error = 100;
    DTPKC dtpkc;

    DTPKC::Cipher dtpkc_value;
    DTPKC::Cipher dtpkc_randomness;
    DTPKC::Cipher dtpkc_value_blinded;

    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));
    dtpkc.keygen(precision, randstate, nb_bits, error);

    dtpkc.getKey(pkey, skey);
    dtpkc.updatePkw(pkey);


    double val = 1.11111;
    long randomness = 234416831;
    long scale_factor = pow(10, 6);
    mpz_class mpz_randomness;
    mpz_randomness.set_str(std::to_string(randomness), 10);
    long val_scaled = (long) (val * scale_factor);
    double rescale_factor = 1.0 / scale_factor;
    mpz_class mpz_value;

    high_resolution_clock::time_point start_key_exchange = high_resolution_clock::now();

    mpz_value.set_str(std::to_string(val_scaled), 10);
    dtpkc_value = dtpkc.enc(mpz_value, pkey);

    mpz_randomness.set_str(std::to_string(randomness), 10);
    dtpkc_randomness = dtpkc.enc(mpz_randomness, pkey);

    EvalAdd add(dtpkc_value, dtpkc_randomness);

    dtpkc_value_blinded = add.EvalAdd_U1();

    mpz_class mpz_value_blinded = dtpkc.Sdec(dtpkc_value_blinded);
    long decrypted_value_blinded = mpz_value_blinded.get_si();

    cout << "Blinded value after decryption:" << decrypted_value_blinded << endl;

    cout << "When you remove noise : " << decrypted_value_blinded - randomness << endl;

    Ciphertext fhe_value, fhe_randomness, fhe_value_blinded;
    complex<double> *encoded_randomness = new complex<double>[n];
    complex<double> *encoded_value_blinded = new complex<double>[n];

    {
        complex<double> c;
        c.real(randomness);
        c.imag(0);
        encoded_randomness[0] = c;
    }
    {
        complex<double> c;
        c.real(decrypted_value_blinded);
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

    scheme.encrypt(fhe_value_blinded, encoded_value_blinded, n, logp, logq);
    scheme.encrypt(fhe_randomness, encoded_randomness, n, logp, logq);

    complex<double> *decrypted_blinded = scheme.decrypt(secretKey, fhe_value_blinded);

    cout.precision(10);
    cout << "Decrypted Encoded blinded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << decrypted_blinded[i] << ' ';
    }
    cout << " " << endl;
    scheme.sub(fhe_value, fhe_value_blinded, fhe_randomness);

    scheme.multByConstAndEqual(fhe_value, rescale_factor, logp);
    scheme.reScaleByAndEqual(fhe_value, logp);
    //scheme.reScaleByAndEqual(fhe_value_blinded, logp);

    complex<double> *encoded_value = scheme.decrypt(secretKey, fhe_value);

    cout << "Encoded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << encoded_value[i] << ' ';
    }
    cout << " " << endl;

    high_resolution_clock::time_point end_key_exchange = high_resolution_clock::now();
    auto duration_exchange = duration_cast<milliseconds>(end_key_exchange - start_key_exchange).count();
    cout << "Protocol V1 takes : " << duration_exchange / 1000 << "s." << endl; */

    /*gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));
    dtpkc.keygen(precision, randstate, nb_bits, error);

    dtpkc.getKey(pkey, skey);
    dtpkc.updatePkw(pkey);

    high_resolution_clock::time_point start_key_exchange = high_resolution_clock::now();

    mpz_value.set_str(std::to_string(val_scaled), 10);
    dtpkc_value = dtpkc.enc(mpz_value, pkey);

    mpz_randomness.set_str(std::to_string(randomness), 10);
    dtpkc_randomness = dtpkc.enc(mpz_randomness, pkey);

    EvalAdd add(dtpkc_value, dtpkc_randomness);

    dtpkc_value_blinded = add.EvalAdd_U1();

    mpz_class mpz_value_blinded = dtpkc.Sdec(dtpkc_value_blinded);
    long decrypted_value_blinded = mpz_value_blinded.get_si();
    double rescaled_decrypted_value_blinded = (double) decrypted_value_blinded / scale_factor;
    double rescaled_randomness = (double) randomness / scale_factor;

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

    //scheme.multByConstAndEqual(fhe_value, rescale_factor, logp);
    //scheme.reScaleByAndEqual(fhe_value, logp);
    //scheme.reScaleByAndEqual(fhe_value_blinded, logp);

    complex<double> *encoded_value = scheme.decrypt(secretKey, fhe_value);


    cout << "Encoded values:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << encoded_value[i] << ' ';
    }
    cout << " " << endl;
    high_resolution_clock::time_point end_key_exchange = high_resolution_clock::now();
    auto duration_exchange = duration_cast<milliseconds>(end_key_exchange - start_key_exchange).count();
    cout << "Protocol V2 takes : " << duration_exchange / 1000 << "s." << endl;
    //return lround(duration);

    return 0;*/
    //serializationUtils->writeKey(secretKey, "./key.text");

    /*cout << "YOTONGOKLI" << endl;

    complex<double> *mvec1 = new complex<double>[n];
    complex<double> *mvec2 = new complex<double>[n];
    complex<double> *mvec3 = new complex<double>[n];

    double cst = 3;

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(1);
        c.imag(0);
        mvec1[i] = c;
        mvec2[i] = c;
        mvec3[i] = c;
    }

    Ciphertext cipher1;
    Ciphertext cipher2;
    cout << "RISITOS ?" << endl;

    // return 0;

    high_resolution_clock::time_point start = high_resolution_clock::now();
    cout << "KE ?" << endl;
    scheme.encrypt(cipher1, mvec1, n, logp, logq);
    cout << "GNEU GNEU GNEU ?" << endl;
    scheme.encrypt(cipher2, mvec2, n, logp, logq);
    cout << "AKLIKLI ?" << endl;
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();


    complex<double> *dvec = scheme.decrypt(secretKey, cipher1);

    cout << "FHE decrypt result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvec[i] << ' ';
    }
    cout << " " << endl;


    Ciphertext cipherConstAdd;

    scheme.addConst(cipherConstAdd, cipher1, 1.5, logp);

    complex<double> *dvecConstAdd = scheme.decrypt(secretKey, cipherConstAdd);

    cout << "FHE Constant Addition result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecConstAdd[i] << ' ';
    }
    cout << " " << endl;

    Ciphertext cipherConstMul;


    scheme.multByConst(cipherConstMul, cipher1, 1.5, logp);
    scheme.reScaleByAndEqual(cipherConstMul, logp);
    complex<double> *dvecConstMul = scheme.decrypt(secretKey, cipherConstMul);
    cout << "FHE Constant Multiplication result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecConstMul[i] << ' ';
    }
    cout << " " << endl; */



    // Depth should be 10 in this context, i.e logq - logp


    /*long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;
    long numThread = 2;

    // Construct and Generate Public Keys //
    srand(time(NULL));
    SetNumThreads(numThread);
    TimeUtils timeutils;
    Ring ring;
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
    scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message

    complex<double> *mvec1 = new complex<double>[n];
    complex<double> *mvec2 = new complex<double>[n];
    complex<double> *mvec3 = new complex<double>[n];

    double cst = 3;

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(i);
        c.imag(0);
        mvec1[i] = c;
        mvec2[i] = c;
        mvec3[i] = c;
    }

    Ciphertext cipher1;
    Ciphertext cipher2;

    high_resolution_clock::time_point start = high_resolution_clock::now();
    scheme.encrypt(cipher1, mvec1, n, logp, logq);
    scheme.encrypt(cipher2, mvec2, n, logp, logq);
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    cout << "FHE encryption duration: " << duration / (1000 * 2) << "ms." << endl;

    SerializationUtils serializationUtils;
    Ciphertext cipher_deserialized(logp, logq, n);

    start = high_resolution_clock::now();

    serializationUtils.writeCiphertext(cipher1, "./cipher.txt");
    serializationUtils.readCiphertextModified(cipher_deserialized, "./cipher.txt");

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();

    cout << "Ser and Deser took: " << duration / 1000 << "ms." << endl;

    complex<double> *decrypted_deserialized = scheme.decrypt(secretKey, cipher_deserialized);


    cout << "Deserialization result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << decrypted_deserialized[i] << ' ';
    }
    cout << " " << endl;

    return 0;

    Ciphertext cipherAdd;
    scheme.encrypt(cipherAdd, mvec2, n, logp, logq);

    start = high_resolution_clock::now();
    int k = 100;
    int iter = 2;

    for (int i = 0; i < k; i++) {
        scheme.add(cipherAdd, cipher1, cipher2);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE addition duration: " << duration / (1000 * k) << "ms." << endl;

    Ciphertext cipherConstAdd;
    start = high_resolution_clock::now();

    for (int i = 0; i < k; i++) {
        scheme.addConst(cipherConstAdd, cipher1, cst, logp);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Constant addition duration: " << duration / (1000 * k) << "ms." << endl;


    Ciphertext cipherConstMul;
    scheme.encrypt(cipherAdd, mvec2, n, logp, logq);

    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.multByConst(cipherConstMul, cipher1, cst, logp);
        scheme.reScaleByAndEqual(cipherConstMul, logp);
    }

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE const multiplication duration: " << duration / (1000 * iter) << "ms." << endl;


    Ciphertext cipherMul;
    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.mult(cipherMul, cipher1, cipher2);
        scheme.reScaleByAndEqual(cipherMul, logp);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE multiplication duration: " << duration / (1000 *iter) << "ms." << endl;


    Ciphertext cipherLeftRot;

    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.leftRotateFast(cipherLeftRot, cipher1, 4);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Left rotate duration: " << duration / (1000 *iter) << "ms." << endl;

    Ciphertext cipherRightRot;

    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.rightRotateFast(cipherRightRot, cipher1, 4);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Right rotate duration: " << duration / (1000 *iter) << "ms." << endl;


    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        complex<double> *dvec = scheme.decrypt(secretKey, cipher1);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Decrypt duration: " << duration / (1000 *iter) << "ms." << endl;


    complex<double> *dvecAdd = scheme.decrypt(secretKey, cipherAdd);
    complex<double> *dvecMul = scheme.decrypt(secretKey, cipherMul);
    complex<double> *dvecConstAdd = scheme.decrypt(secretKey, cipherConstAdd);
    complex<double> *dvecConstMul = scheme.decrypt(secretKey, cipherConstMul);
    complex<double> *dvecLeftRot = scheme.decrypt(secretKey, cipherLeftRot);
    complex<double> *dvecRightRot = scheme.decrypt(secretKey, cipherRightRot);



    cout << "FHE Addition result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecAdd[i] << ' ';
    }
    cout << " " << endl;

    cout << "FHE Constant Addition result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecConstAdd[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Constant Multiplication result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecConstMul[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Multiplication result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecMul[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Left Rotation result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecLeftRot[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Right Rotation result:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << dvecRightRot[i] << ' ';
    }
    cout << " " << endl;



    /*vector<string> func_names = {"encryption", "decryption", "partial_decryption0",
                                 "partial_decryption1", "addition", "subtraction", "multiplication",
                                 "division", "hbound", "entropy"};
    vector<string> func_names = {"encryption", "decryption", "addition", "subtraction",
                                 "multiplication", "batch_encryption_partial_key", "batch_addition", "batch_decryption_partial_key"};
    long t;

    BENCHBuildingBlocks benchBuildingBlocks;
    int keysizes[7] = {1024};
    int length = 0;
    string separator = ";";
    string file_name = "/home/nassimaab/Desktop/stats.csv";
    //mpz_class pkey, skey;

    /*DTPKC dtpkc;
    mpz_class pkey = dtpkc.pkw;

    dtpkc.deserializeDtpkc();

    PPDatasetReader datasetReader (dtpkc);

    datasetReader.encryptData(path+"DO_1.data", path+"CRYPTO/DO_1.enc", pkey);
    datasetReader.encryptData(passth+"DO_2.data", path+"CRYPTO/DO_2.enc", pkey);
    datasetReader.encryptData(path+"DO_3.data", path+"CRYPTO/DO_3.enc", pkey);
    datasetReader.encryptData(path+"DO_4.data", path+"CRYPTO/DO_4.enc", pkey);
    datasetReader.encryptData(path+"DO_5.data", path+"CRYPTO/DO_5.enc", pkey);
    datasetReader.encryptData(path+"DO_6.data", path+"CRYPTO/DO_6.enc", pkey);
    datasetReader.encryptData(path+"DO_7.data", path+"CRYPTO/DO_7.enc", pkey);
    datasetReader.encryptData(path+"DO_8.data", path+"CRYPTO/DO_8.enc", pkey);

    datasetReader.encryptData(path+"testing.data", path+"CRYPTO/testing.enc", pkey);



    //std::ofstream file;
    //file.open(file_name);
    //file << "Operation" + separator + "KeySize" + separator + "ExecTime" + "\n";
    //file.close();
    std::vector<int> v;
    int nb_slots = 10;
    int slot_size = 30;

    for (int i = 0; i < nb_slots; ++i) {
        v.push_back(i * 111111111);
    }

    benchBuildingBlocks.set_iterations(10);
    benchBuildingBlocks.set_operands(10000000, 10);
    //file.open(file_name, std::ios_base::app);
    for (int i = 0; i < length; i++) {
        int keysize = keysizes[i];
        cout << "Key size : " << keysize << " bits." << endl;
        benchBuildingBlocks.set_keysize(keysize);
        benchBuildingBlocks.dtpkc.getKey(pkey, skey);
        for(auto const& func_name: func_names) {
            if (func_name == "encryption") {
                t = benchBuildingBlocks.bench_encryption();
            } else if (func_name == "decryption") {
                t = benchBuildingBlocks.bench_decryption();
            } else if (func_name == "partial_decryption0") {
                t = benchBuildingBlocks.bench_partial_decryption0();
            } else if (func_name == "partial_decryption1") {
                t = benchBuildingBlocks.bench_partial_decryption1();
            } else if (func_name == "comparison") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_comparison();
            } else if (func_name == "addition") {
                t = benchBuildingBlocks.bench_addition();
            } else if (func_name == "subtraction") {
                t = benchBuildingBlocks.bench_subtraction();
            } else if (func_name == "multiplication") {
                //cout << "issou" << endl;
                t = benchBuildingBlocks.bench_multiplication();
            } else if (func_name == "division") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_division();
            } else if (func_name == "keyswitch") {
                t = benchBuildingBlocks.bench_keyswitch();
            } else if (func_name == "hbound") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_hbound();
            } else if (func_name == "entropy") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_entropy();
            } else if (func_name == "batch_encryption") {
                t = benchBuildingBlocks.bench_batch_encryption(v);
            } else if (func_name == "batch_decryption") {
                t = benchBuildingBlocks.bench_batch_decryption();
            } else if (func_name == "batch_addition") {
                t = benchBuildingBlocks.bench_batch_addition();
            } else if (func_name == "batch_encryption_partial_key") {
                t = benchBuildingBlocks.bench_batch_encryption_partial_key(v, pkey);
            } else if (func_name == "batch_decryption_partial_key") {
                t = benchBuildingBlocks.bench_batch_decryption_partial_key(skey);
            }
            cout << func_name << " took: " << t << "us." << endl;
            //time_size[keysize] = t;
            //file << func_name + separator + to_string(keysize) + separator + to_string(t) + "\n";
        }
        cout << "---------------------------------------------------------------------------------" << endl;
        //benchBuildingBlocks.dtpkc.serializeDtpkc();
    }
    //file.close();*/
}