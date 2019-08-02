/**
** Class :  SkeySwitch
 * Authors : Talbi Rania
** Creation date : 26/11/2018
** Last Update : 26/11/2018
** Purpose : Change the cryptographic key of a cipher
 *
**/

#include "SkeySwitch.h"


SkeySwitch::SkeySwitch(Cipher A, mpz_class pkey, DTPKC dtpkc)
{
    this->A=A;
    this->pkey=pkey;
    this->dtpkc=dtpkc;
}

std::vector<Cipher>
SkeySwitch ::SkeySwitch_U1_step1()
{

    gmp_randstate_t state;
    Cipher S1, S2, S3, S4;
    std::vector <Cipher> res(4);

    // Generate two random numbers : used for multiplicative cryptographic blinding
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    r = mpz_class_get_z_bits(dtpkc.blindVal);
    gmp_randclear(state);

    S1 = dtpkc.enc(r,A.Pub);

    //A+r
    S2.T1= A.T1*S1.T1;
    S2.T2= A.T2*S1.T2;
    S2.Pub= A.Pub;

    // Partially decrypt A+r
    S3=dtpkc.PSdec0(S2);


    res[0]=S2;
    res[1]=S3;


    return res;

}

Cipher SkeySwitch::SkeySwitch_U2_step2(std::vector<Cipher> res1)
{

    mpz_class x;
    Cipher out;

    // Partially decrypt  A+r
    x=dtpkc.PSdec1(res1[0],res1[1]);

    out = dtpkc.enc(x,pkey);

    bd += sizeof(out);

    return  out;

}

Cipher SkeySwitch::SkeySwitch_U1_step3(Cipher res2)
{
    Cipher S4, S5, res;

    S4 = dtpkc.enc(r,pkey);


    S5.T1=mpz_class_powm(S4.T1,dtpkc.n-1,dtpkc.n2*dtpkc.n2);
    S5.T2=mpz_class_powm(S4.T2,dtpkc.n-1,dtpkc.n2*dtpkc.n2);
    S5.Pub=pkey;


    res.T1 = S5.T1 * res2.T1;
    res.T2 = S5.T2 * res2.T2;
    res.Pub = pkey;



    return  res;

}

