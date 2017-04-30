#include "main.h"

#define SWAP(x, y, t) ((t) = (x), (x) = (y), (y) = (t))

const char* triangle_type(int a, int b, int c)
{
    if ( 1 > a || a > 200 )
        return "Invalid";

    if ( 1 > b || b > 200 )
        return "Invalid";

    if ( 1 > c || c > 200 )
        return "Invalid";

    int tmp;

    if ( a > b )
        SWAP(a,b,tmp);
    if ( b > c )
        SWAP(b,c,tmp);
    if ( a > b )
        SWAP(a,b,tmp);

    if ( a + b <= c )
        return "Not a Triangle";

    if( a == b && b == c )
        return "Equilateral";
    if( a == b && a != c )
        return "Isosceles";
    else if( b == c && b != a )
        return "Isosceles";
    else
        return "Scalene";
}
