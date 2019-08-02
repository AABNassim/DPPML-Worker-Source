/**
** Class :  EvalAdd
 * Authors : Talbi Rania
** Creation date : 05/11/2018
** Last Update : 05/11/2018
** Purpose : Addition of two encrypted integers under the same public key.
 *
 *
**/

#ifndef DAPPLE_EVALADD_H
#define DAPPLE_EVALADD_H

#include "DTPKC.h"

class EvalAdd {


    /**
     *  ***************Attributes **************
     *  a : Encrypted value of the first operand.
     *  b : Encrypted value of the second operand.
     *  c : Encryption of the addition output
     */

private :
    Cipher a, b, c;


    /******* Functions **********/

    // Constructor : used to initialise the operands
    public :
    EvalAdd(Cipher a, Cipher b);

    // The first and only step of the secure addition : This part of the code is run by U1
    public :
    Cipher EvalAdd_U1();

};


#endif //DAPPLE_EVALADD_H
