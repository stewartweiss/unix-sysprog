#include <limits.h>

long fact(int n)  /*  compute  n! */
{
    long k = 1;
    long old_k;
    while ( (n > 0) && ( k < LONG_MAX) ) {
        old_k = k;
        k *= n--;
    }
    if ( n > 0 )
        return -1;
    else
        return(k);
}

