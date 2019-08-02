#include "EvalSub.h"
#include "DTPKC.h"



EvalSub :: EvalSub(Cipher a, Cipher b, DTPKC dtpkc)
{
    this->a=a;
    this->b=b;
    this->dtpkc=dtpkc;
}



Cipher
EvalSub :: EvalSub_U1()
{


    if (a>b)
    {
        c.T1=a.T1*mpz_class_powm(b.T1,dtpkc.n-1,dtpkc.n2);
        c.T2=a.T2*mpz_class_powm(b.T2,dtpkc.n-1,dtpkc.n2);
        c.Pub=a.Pub;
    }
    else
        c=dtpkc.enc(0,a.Pub);


    return c;
}