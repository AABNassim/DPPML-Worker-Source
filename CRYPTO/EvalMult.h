/**
** Class :  EvalMult
 * Authors : Talbi Rania
** Creation date : 27/11/2018
** Last Update : 27/11/2018
** Purpose : Compute the product of two ciphers
 *
**/
#ifndef DAPPLE_EVALMULT_H
#define DAPPLE_EVALMULT_H
#include "DTPKC.h" // Header of the DTPKC cryptosystem's class
#include "../UTIL/math_util.hh"
#include <vector>


class EvalMult{


    /**
     *  ***************Attributes **************
     *  A : First operand
     *  B : Second operand
     *  dtpkc : used to get the DTPKC parameters
     *  r : random value used for cryptographic blinding
     */

    private :
    Cipher  A, B;
    DTPKC dtpkc;
    mpz_class r;



    /******* Functions **********/

    // Constructor : used to initialise the operands
    public :
    EvalMult(Cipher A, Cipher B , DTPKC dtpkc);

    // The first step of the secure product protocol : This part of the code is run by U1
    public :
    std :: vector < Cipher> EvalMult_U1_step1();

    // The second step of the secure product protocol : This part of the code is run by U2
    public :
    Cipher EvalMult_U2_step2(std::vector<Cipher> res1);

    // The third step of the secure product protocol : This part of the code is run by U2
    public :
    Cipher EvalMult_U1_step3(Cipher res2);

};


#endif //DAPPLE_EVALMULT_H
