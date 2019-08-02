// Created by nassimaab on 1/10/19.
//

#include "BENCHBuildingBlocks.h"
#include <chrono>
#include <gmp.h>
#include <string.h>


using namespace std;
using namespace std::chrono;



BENCHBuildingBlocks::BENCHBuildingBlocks() {
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));
    dtpkc.keygen(precision, randstate, nb_bits, error);

    dtpkc.getKey(pkey, skey);
    dtpkc.updatePkw(pkey);

    A.set_str(std::to_string(a), 10);
    B.set_str(std::to_string(b), 10);

    C_A = dtpkc.enc(A, pkey);
    C_B = dtpkc.enc(B, pkey);
    PC_A = dtpkc.PSdec0(C_A);

}

void BENCHBuildingBlocks::set_keysize(unsigned int keysize){
    nb_bits = keysize;
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate, time(NULL));
    dtpkc.keygen(precision, randstate, nb_bits, error);

    dtpkc.getKey(pkey, skey);
    dtpkc.updatePkw(pkey);

    a = 10, b = 3;
    A.set_str(std::to_string(a), 10);
    B.set_str(std::to_string(b), 10);

    C_A = dtpkc.enc(A, pkey);
    C_B = dtpkc.enc(B, pkey);
    PC_A = dtpkc.PSdec0(C_A);
}

void BENCHBuildingBlocks::set_iterations(int iter) {
    iterations = iter;
}

long BENCHBuildingBlocks::bench_keygen() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        gmp_randinit_default(randstate);
        gmp_randseed_ui(randstate, time(NULL));
        dtpkc.keygen(precision, randstate, nb_bits, error);

        dtpkc.getKey(pkey, skey);
        dtpkc.updatePkw(pkey);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}


long BENCHBuildingBlocks::bench_encryption() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        C_A = dtpkc.enc(A, pkey);
        C_B = dtpkc.enc(B, pkey);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / (2 * iterations));
}

long BENCHBuildingBlocks::bench_decryption() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    mpz_class clear;
    for (int i = 0; i < iterations; i++) {
        clear = dtpkc.Sdec(C_A);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    //cout << "Decryption result : " << clear.get_str() << endl;
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}

long BENCHBuildingBlocks::bench_partial_decryption0() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        Cipher PC = dtpkc.PSdec0(C_A);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}

long BENCHBuildingBlocks::bench_partial_decryption1() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        mpz_class clear = dtpkc.PSdec1(C_A, PC_A);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}

long BENCHBuildingBlocks::bench_partial_decryption_complete() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        PC_A = dtpkc.PSdec0(C_A);
        mpz_class clear = dtpkc.PSdec1(C_A, PC_A);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}

void BENCHBuildingBlocks::set_operands(long op_a, long op_b) {
    A.set_str(std::to_string(op_a), 10);
    B.set_str(std::to_string(op_b), 10);
}


long BENCHBuildingBlocks::bench_addition() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        EvalAdd add(C_A, C_B);
        C_C = add.EvalAdd_U1();
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    mpz_class clear = dtpkc.Sdec(C_C);
    //cout << "Addition result : " + clear.get_str() << endl;
    auto duration = duration_cast<microseconds>(end - start).count();
    //cout << "ADDITION TAKES : " << duration << "us." << endl;
    return lround(duration / iterations);
}

long BENCHBuildingBlocks::bench_batch_encryption(std::vector<long> v) {
    string str_v(nb_slots * slot_size, '0');
    mpz_class vec;
    high_resolution_clock::time_point start = high_resolution_clock::now();

    for (int i = 0; i < v.size(); i++) {
        string val = to_string(v[i]);
        for (int j = 0; j < val.length(); ++j) {
            str_v[(nb_slots - i - 1) * slot_size + slot_size - j - 1] = val[val.length() - j - 1];
        }
    }
    vec.set_str(str_v, 10);
    //cout << str_v << endl;
    for (int i = 0; i < iterations; i++) {
        C_VA = dtpkc.enc(vec, pkey);
        C_VB = dtpkc.enc(vec, pkey);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / (2 * iterations));
}


long BENCHBuildingBlocks::bench_batch_encryption_partial_key(std::vector<int> v, mpz_class key) {
    string str_v(nb_slots * slot_size, '0');
    mpz_class vec;
    high_resolution_clock::time_point start = high_resolution_clock::now();

    for (int i = 0; i < v.size(); i++) {
        string val = to_string(v[i]);
        for (int j = 0; j < val.length(); ++j) {
            str_v[(nb_slots - i - 1) * slot_size + slot_size - j - 1] = val[val.length() - j - 1];
        }
    }
    vec.set_str(str_v, 10);
    //cout << str_v << endl;
    for (int i = 0; i < iterations; i++) {
        C_VA = dtpkc.enc(vec, key);
        C_VB = dtpkc.enc(vec, key);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / (2 * iterations));
}


long BENCHBuildingBlocks::bench_batch_decryption() {
    string str_v;
    string dstr_v;
    vector<int> vec;
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int k = 0; k < iterations; k++) {
        VA = dtpkc.Sdec(C_VC);
        str_v = VA.get_str();
        //cout << str_v << endl;
        string padd(nb_slots * slot_size - str_v.length(), '0');
        str_v = padd + str_v;
        //cout << str_v << endl;
        for (int i = nb_slots - 1; i >= 0; i--) {
            //cout << str_v.substr(i * slot_size, slot_size) << endl;
            vec.push_back(stoi(str_v.substr(i * slot_size, slot_size)));
        }
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    /*cout << "BATCH DECRYPT RESULT : " << endl;
    for (int i = 0; i < nb_slots; i++) {
        dstr_v += to_string(vec[i]) + " ";
    }*/
    cout << dstr_v << endl;
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}


long BENCHBuildingBlocks::bench_batch_decryption_partial_key(mpz_class key) {
    string str_v;
    string dstr_v;
    vector<int> vec;
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int k = 0; k < iterations; k++) {
        VA = dtpkc.dec(C_VC, key);
        str_v = VA.get_str();
        //cout << str_v << endl;
        string padd(nb_slots * slot_size - str_v.length(), '0');
        str_v = padd + str_v;
        //cout << str_v << endl;
        for (int i = nb_slots - 1; i >= 0; i--) {
            //cout << str_v.substr(i * slot_size, slot_size) << endl;
            vec.push_back(stoi(str_v.substr(i * slot_size, slot_size)));
        }
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    /*cout << "BATCH DECRYPT PARTIAL KEY RESULT : " << endl;
    for (int i = 0; i < nb_slots; i++) {
        dstr_v += to_string(vec[i]) + " ";
    }
    cout << dstr_v << endl;*/
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}

long BENCHBuildingBlocks::bench_batch_addition() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        EvalAdd add(C_VA, C_VB);
        C_VC = add.EvalAdd_U1();
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    mpz_class clear = dtpkc.Sdec(C_VC);
    //cout << "BATCH Addition result : " + clear.get_str() << endl;
    auto duration = duration_cast<microseconds>(end - start).count();
    //cout << "BATCH ADDITION TAKES : " << duration << "us." << endl;
    return lround(duration / iterations);
}


long BENCHBuildingBlocks::bench_subtraction() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        EvalSub sub(C_A, C_B, dtpkc);
        C_C = sub.EvalSub_U1();
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    mpz_class clear_A = dtpkc.Sdec(C_A);
    mpz_class clear_B = dtpkc.Sdec(C_B);
    mpz_class clear = dtpkc.Sdec(C_C);
    //cout << "Subtraction result : " + clear.get_str() << endl;
    //cout << "A = " + clear_A.get_str() << endl;
    //cout << "B = " + clear_B.get_str() << endl;

    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}

long BENCHBuildingBlocks::bench_multiplication() {
    high_resolution_clock::time_point start = high_resolution_clock::now();

    for (int i = 0; i < iterations; i++) {
        EvalMult e1(C_A, C_B, dtpkc);
        C_C = e1.EvalMult_U1_step3(e1.EvalMult_U2_step2(e1.EvalMult_U1_step1()));
    }

    mpz_class clear = dtpkc.Sdec(C_C);
    //cout << "Multiplication result : " + clear.get_str() << endl;

    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}


long BENCHBuildingBlocks::bench_keyswitch() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        dtpkc.getKey(newPkey, newSkey);
        dtpkc.updatePkw(newPkey);

        SkeySwitch skeySwitch(C_A, newPkey, dtpkc);
        C_C = skeySwitch.SkeySwitch_U1_step3(skeySwitch.SkeySwitch_U2_step2(skeySwitch.SkeySwitch_U1_step1()));
        C = dtpkc.Sdec(C_C);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return lround(duration / iterations);
}




void BENCHBuildingBlocks::generate_newkeys() {
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));
    dtpkc.keygen(precision, randstate, nb_bits, error);

    dtpkc.getKey(newPkey, newSkey);
    dtpkc.updatePkw(newPkey);
}