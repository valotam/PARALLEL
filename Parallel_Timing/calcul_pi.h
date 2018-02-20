#ifndef CALCUL_PI_H_INCLUDED
#define CALCUL_PI_H_INCLUDED

# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cstring>
# include <omp.h>

using namespace std;

int computPI ( );
void r8_test ( int r8_logn_max );
double r8_abs ( double r8 );
double r8_pi_est_omp ( int n );
double r8_pi_est_seq ( int n );

#endif // CALCUL_PI_H_INCLUDED
