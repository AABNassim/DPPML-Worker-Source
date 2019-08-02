
#ifndef DAPPLE_EVALSUB_H
#define DAPPLE_EVALSUB_H
#include "DTPKC.h" // Header of the DTPKC cryptosystem's class

class EvalSub {

    private :
    Cipher a, b, c;
    DTPKC dtpkc;

    /******* Functions **********/


    public :
    EvalSub(Cipher a, Cipher b, DTPKC dtpkc);


    public :
    Cipher EvalSub_U1();

};


#endif //DAPPLE_EVALSUB_H