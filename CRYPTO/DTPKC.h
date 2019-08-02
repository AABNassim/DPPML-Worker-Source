/**
** Class :  DTPKC
 * Authors : Talbi Rania
** Creation date : 31/10/2018
** Last Update : 05/11/2018 : Added PSdec1 function
** Purpose : DT-PKC cryptosystem primitives
 *
**/

#ifndef DAPPLE_DTPKC_H
#define DAPPLE_DTPKC_H

#include<iostream>
#include <gmpxx.h>
#include <gmp.h>
#include "../UTIL/util_gmp_rand.h"// Useful functions for random Big integers generation
#include "../UTIL/num_th_alg.hh" // Useful for random big primes generation
#include "../UTIL/mpz_class.hh" // Extension to the GMP C++ interface
#include <ctime> // Used for random seeds selection
#include <memory> // Used for safe object allocation
#include <vector>
#include <cstring>
#include <fstream>
#include <netinet/in.h>


using namespace std;

class DTPKC {

public :

    /*
     * Cipher :
     * represents the structure of a cipher in DTPKC
     * T1 : First part of the cipher
     * T2 : Second part of  the cipher
     * PUB : The public key used to encrypt the cipher
     *
     */

    struct Cipher {

        mpz_class T1, T2, Pub;
        int sign = 1;

        /**
         * overload the comparison operators for ciphers.
         *
         */

        bool operator<(const DTPKC::Cipher val )
        const {
            return DTPKC::EvalCmp_U2_step2(DTPKC::EvalCmp_U1_step1(*this,val));
        }

        bool operator==( const DTPKC::Cipher val )
        const {

            return DTPKC::EvalEq_U2_step2(DTPKC::EvalCmp_U1_step1(*this,val));

        }

        bool operator!=( const DTPKC::Cipher val )
        const {

            return  ! DTPKC::EvalEq_U2_step2(DTPKC::EvalCmp_U1_step1(*this,val));

        }

        bool operator>(const DTPKC::Cipher val )
        const {
            return ! DTPKC::EvalCmp_U2_step2(DTPKC::EvalCmp_U1_step1(*this,val));
        }

        friend std::ostream& operator << ( ostream& os, const Cipher & rhs)   //Overloaded operator for '<<'{                                                                                    //for struct output
        {
            os << Sdec(rhs).get_str();
            return os;

        }



    };



    /**
     *  ******** Attributes *******
     *  p, q : two strong primes
     *  n=p*q
     *  g : generator of a cyclic group of an order (p-1)(q-1)/2, lookup [1]
     *  n2 : n square.
     *  SK : lcm (p-1,q-1)/2, DT-PKC Strong Key.
     *  SK0 : partial strong key 1.
     *  SK1 : partial strong key 2.
     *  pkw : joint public key in DAPPLE (initial value)
     *  size : bit-size of the systems's strong keys. Its also the size of the generated random numbers.
     *  r : random number used for blinding in comparison.
     *
     */

    public :
            static mpz_class n, g, n2, pkw;
            static int size;
            static int blindVal;
            static  int precision;
            int nb_slots = 9;
            int digits_per_slot = 15;
            int nb_digits = 309;



    private :
            static mpz_class p, q, a, SK, SK0, SK1;

     /******* Functions **********/

    // Used to generate DT-PKC cryptosystem's parameters
    public :
    static void keygen(int precision, gmp_randstate_t state, uint nbits=1024, int error=600, bool useORE=false, int blindingVal=200);

    // Used to encrypt a plaintext using a public key pkey in DT-PKC.
    public :
    static Cipher enc(mpz_class plaintext, mpz_class pkey);
    Cipher enc_batch(vector<long> plaintext_slots, mpz_class pkey);
    vector<long> decrypt_batch(Cipher c);


        // Used to generate a pair of public and secret keys
    public :
    static void getKey(mpz_class &pkey, mpz_class &skey);

    // Used to update the system's joint key according to a given new public key
    public :
    static void updatePkw(mpz_class pkey);

    // Compute function L(x)=x-1/N : used in  full and partial decryption functions in DTPKC
    public :
    static mpz_class L(mpz_class x);

    // Used to decrypt a given cipher using the corresponding secrete key
    public :
    static mpz_class dec(Cipher c, mpz_class skey);

    // Used to decrypt any cipher c using the strong key SK (Not really used
    // in DAPPLE because no party is allowed to have this key, so its just used for tests)
    public :
    static mpz_class Sdec(Cipher c);

    // Used to partially decrypt a cipher c using the partial strong key SK0 held by U1
    public :
    static Cipher PSdec0(Cipher c);

    // Used to decrypt a partial cipher "pc" using the partial strong key SK1 held by U2
    public :
    static mpz_class PSdec1(Cipher c, Cipher pc);

    // Used to serialize the dtpkc system's parameters
    public:
    int serializeDtpkc(string path);

    // Used to deserialize dtpkc
    public:
    int deserializeDtpkc(string path);

    // The first step of the secure comparison protocol : This part of the code is run by U1
    public :
    static std :: vector < Cipher> EvalCmp_U1_step1(Cipher A, Cipher B);

    // The second step of the secure comparison protocol : This part of the code is run by U2
    public :
    static bool EvalCmp_U2_step2(std::vector<Cipher> res1);

    // The second step of the secure equality-test protocol : This part of the code is run by U2
    public :
    static bool EvalEq_U2_step2(std::vector<Cipher> res1);

    public :
    void sendPara(int sockfd, sockaddr_in cliaddr);
};

typedef struct DTPKC::Cipher Cipher;

#endif //DAPPLE_DTPKC_H
