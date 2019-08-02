/*
 * Copyright 2013-2015 Raphael Bost, Raluca Ada Popa
 *
 * This file is part of ciphermed.

 *  ciphermed is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  ciphermed is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with ciphermed.  If not, see <http://www.gnu.org/licenses/>. 2
 *
 */

#pragma once

#include <vector>

#include <gmpxx.h>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>


inline int mpz_class_probab_prime_p(const mpz_class &n, int reps)
{
    return mpz_probab_prime_p(n.get_mpz_t(),reps);
}

inline double  RationalApproximation(double t)
{
    // Abramowitz and Stegun formula 26.2.23.
    // The absolute value of the error should be less than 4.5 e-4.
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) /
               (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}


inline double NormalCDFInverse(double p)
{
    if (p <= 0.0 || p >= 1.0)
    {
        std::stringstream os;
        os << "Invalid input argument (" << p
           << "); must be larger than 0 but less than 1.";
        throw std::invalid_argument( os.str() );
    }

    // See article above for explanation of this section.
    if (p < 0.5)
    {
        // F^-1(p) = - G^-1(p)
        return -RationalApproximation( sqrt(-2.0*log(p)) );
    }
    else
    {
        // F^-1(p) = G^-1(1-p)
        return RationalApproximation( sqrt(-2.0*log(1-p)) );
    }
}

// v: residues, m: moduli, s: number of elements
void mpz_t_crt(mpz_t x, const mpz_ptr *v, const mpz_ptr *m, size_t s);

mpz_class mpz_class_log2_10 (mpz_class op );

mpz_class mpz_class_exp(mpz_class op, int prec);

mpz_class mpz_class_log (mpz_class op, int lambda);

mpz_class mpz_class_crt(const std::vector<mpz_class> &v, const std::vector<mpz_class> &m);

inline mpz_class mpz_class_crt_2(const mpz_class &v1, const mpz_class &v2, const mpz_class &m1, const mpz_class &m2)
{
    return mpz_class_crt({v1,v2},{m1,m2});
}

class FixedPointExp {
public:
    
    FixedPointExp(mpz_t& g, mpz_t& p, int fieldsize);
    ~FixedPointExp();
    
public:
    void powerMod(mpz_t& result, mpz_t& e);


    
private:
    //create table
    void init();
    
private:
    mpz_t m_p;
    mpz_t m_g;
    bool m_isInitialized;
    unsigned m_numberOfElements;
    mpz_t* m_table;
};


