/**
** Class :  EvalMult
 * Authors : Talbi Rania
** Creation date : 27/11/2018
** Last Update : 27/11/2018
** Purpose : Compute the product of two ciphers
 *
**/

#include "EvalMult.h"



/**
** Function : EvalMult
 * Author : Talbi Rania
** Creation date : 27/11/2018
** Last Update : 27/11/2018
 *
** Purpose : This is the constructor of the class, it is used to initialise the operands
 *
 * Parameters :
 * A : First operand
 * B : Second operand
 * dtpkc : used to get the DTPKC parameters
 *
 * Unit Test : (TEST/TestBuildingBlocks)
 *
**/

EvalMult::EvalMult(Cipher A, Cipher B , DTPKC dtpkc)
{
    this->A=A;
    this->B=B;
    this->dtpkc=dtpkc;
}

/**
** Function :EvalMult_U1_step1
 * Author : Talbi Rania
** Creation date : 13/11/2018
** Last Update : 13/11/2018
 *
** Purpose : The first step of the secure multiplication protocol : This part of the code is run by U1
 *
 * Parameters :
 * none
 *
 * Unit Test : (TEST/TestBuildingBlocks)
 *
**/

std::vector<Cipher>
EvalMult::EvalMult_U1_step1()
{

    gmp_randstate_t state;
    Cipher S1, S2, S3, S4, S5;
    std::vector <Cipher> res(4);

    // Generate the blinding value
    gmp_randinit_default(state);
    gmp_randseed_ui(state,time(NULL));
    r=mpz_class_get_z_bits(dtpkc.blindVal);
    gmp_randclear(state);

    S1=dtpkc.enc(r,A.Pub);

    // A + r
    S2.T1= S1.T1*A.T1;
    S2.T2= S1.T1*A.T2;
    S2.Pub= A.Pub;

    // B+ r
    S3.T1= S1.T1*B.T1;
    S3.T2= S1.T1*B.T2;
    S3.Pub= B.Pub;

    // Partially decrypt A+r and B+r
    S4=dtpkc.PSdec0(S2);
    S5=dtpkc.PSdec0(S3);

    res[0]=S2;
    res[1]=S4;
    res[2]=S3;
    res[3]=S5;

    return res;

}


/**
** Function : EvalMult_U2_step2
 * Author : Talbi Rania
** Creation date : 27/11/2018
** Last Update : 27/11/2018
 *
** Purpose : The second step of the secure multiplication protocol : This part of the code is run by U2
 *
 * Parameters :
 * results of the previous step sent by U1
 *
 * Unit Test : (TEST/TestBuildingBlocks)
 *
**/


Cipher EvalMult::EvalMult_U2_step2(std::vector <Cipher> res1)
{

    mpz_class x, y;
    Cipher out;

    // x :  A+r
    // y :  B+r
    x=dtpkc.PSdec1(res1[0],res1[1]);
    y=dtpkc.PSdec1(res1[2],res1[3]);

    // return x*y + r(x+y) + r^²

    out =  dtpkc.enc(x*y,A.Pub);


    return  out;

}

/**
** Function : EvalMult_U1_step3
 * Author : Talbi Rania
** Creation date : 27/11/2018
** Last Update : 27/11/2018
 *
** Purpose : The third step of the secure multiplication protocol : This part of the code is run by U1
 *
 * Parameters :
 * results of the previous step sent by U2
 *
 * Unit Test : (TEST/TestBuildingBlocks)
 *
**/



Cipher EvalMult::EvalMult_U1_step3(Cipher res2)
{

    Cipher S6, S7, S8, S9, S10;

    // A+B
    S6.T1 = A.T1 * B.T1;
    S6.T2 = A.T2 * B.T2;

    // -r*(A+B)
    S7.T1=mpz_class_powm(S6.T1,dtpkc.n-r,dtpkc.n2);
    S7.T2=mpz_class_powm(S6.T2,dtpkc.n-r,dtpkc.n2);
    S7.Pub = A.Pub;

    S8 = dtpkc.enc (r, A.Pub);

    // -r*r
    S9.T1=mpz_class_powm(S8.T1,dtpkc.n-r,dtpkc.n2);
    S9.T2=mpz_class_powm(S8.T2,dtpkc.n-r,dtpkc.n2);
    S9.Pub = A.Pub;

    // remove blinding values
    S10.T1=S9.T1*S7.T1*res2.T1;
    S10.T2=S9.T2*S7.T2*res2.T2;
    S10.Pub=A.Pub;


    return S10;

}
