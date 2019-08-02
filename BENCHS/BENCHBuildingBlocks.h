//
// Created by nassimaab on 1/10/19.
//

#ifndef DAPPLE_BENCHBUILDINGBLOCKS_H
#define DAPPLE_BENCHBUILDINGBLOCKS_H

#include "../CRYPTO/DTPKC.h"
#include "../CRYPTO/EvalAdd.h"
#include "../CRYPTO/EvalSub.h"
#include "../CRYPTO/SkeySwitch.h"
#include "../CRYPTO/EvalMult.h"

#include "../UTIL/mpz_class.hh"
#include "../UTIL/num_th_alg.hh"

struct stats {
    long iter;
    long mean;
    long var;
};

class BENCHBuildingBlocks {
    gmp_randstate_t randstate;
public:
    DTPKC dtpkc;
    mpz_class pkey, skey, newPkey, newSkey;
    int a = 10, b = 3;
    mpz_class A, B, C, T;
    mpz_class VA, VB, VC;
    DTPKC::Cipher C_A, C_B, C_C;
    DTPKC::Cipher C_VA, C_VB, C_VC;
    DTPKC::Cipher PC_A;
    int nb_bits = 1024, error = 100;
    int iterations = 1;
    int precision = 7;
    int nb_slots = 10;
    int slot_size = 30;

public:
    BENCHBuildingBlocks();
    void set_operands(long op_a, long op_b);
    void set_keysize(unsigned int keysize);
    void set_iterations(int iter);
    long bench_encryption();
    long bench_keygen();
    long bench_decryption();
    long bench_partial_decryption0();
    long bench_partial_decryption1();
    long bench_partial_decryption_complete();
    long bench_addition();
    long bench_subtraction();
    long bench_multiplication();
    long bench_keyswitch();
    long bench_batch_encryption(std::vector<long> v);
    long bench_batch_decryption();
    void generate_newkeys();
    long bench_batch_addition();
    long bench_batch_encryption_partial_key(std::vector<int> v, mpz_class key);
    long bench_batch_decryption_partial_key(mpz_class skey);

};



#endif //DAPPLE_BENCHBUILDINGBLOCKS_H
