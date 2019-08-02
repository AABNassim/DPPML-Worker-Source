/**
** Class :  EvalAdd
 * Authors : Talbi Rania
** Creation date : 05/11/2018
** Last Update : 05/11/2018
** Purpose : Addition of two encrypted integers under the same public key.
 *
 *
**/


#include "EvalAdd.h"





/**
** Function : EvalAdd
 * Author : Talbi Rania
** Creation date : 05/11/2018
** Last Update : 05/11/2018
 *
** Purpose : This is the constructor of the class, it is used to initialise the operands
 *
 * Parameters :
 * a : first operand ( Cipher)
 * b : second operand ( Cipher)
 *
 * Unit Test : (TEST/TestBuildingBlocks)
 *
**/

EvalAdd :: EvalAdd(Cipher a, Cipher b)
{
    this->a=a;
    this->b=b;

}

/**
** Function : EvalAdd_U1
 * Author : Talbi Rania
** Creation date : 05/11/2018
** Last Update : 05/11/2018
 *
** Purpose : The first and only step of the secure addition : This part of the code is run by U1
 *
 * Parameters :
 * none
 *
 * Unit Test : (TEST/TestBuildingBlocks)
 *
**/

Cipher
EvalAdd :: EvalAdd_U1()
{
    // [a]*[b]=[a+b]
    c.T1 = a.T1 * b.T1;
    c.T2 = a.T2 * b.T2;
    c.Pub = a.Pub;

    return c;
}