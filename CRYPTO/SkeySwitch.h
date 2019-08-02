/**
** Class :  SkeySwitch
 * Authors : Talbi Rania
** Creation date : 26/11/2018
** Last Update : 26/11/2018
** Purpose : Change the cryptographic key of a cipher
 *
**/

#ifndef DAPPLE_SKEYSWITCH_H
#define DAPPLE_SKEYSWITCH_H
#include "DTPKC.h" // Header of the DTPKC cryptosystem's class
#include "../UTIL/math_util.hh"
#include <vector>

class SkeySwitch{


    private :
    Cipher  A;
    mpz_class pkey;
    DTPKC dtpkc;
    mpz_class r;

    public :
    int bd;

    /******* Functions **********/

    public :
    SkeySwitch(Cipher A, mpz_class pkey , DTPKC dtpkc);

    public :
    std :: vector < Cipher> SkeySwitch_U1_step1();

    public :
    Cipher SkeySwitch_U2_step2(std::vector<Cipher> res1);

    public :
    Cipher SkeySwitch_U1_step3(Cipher res2);

};


#endif
