#include <iostream>
#include <cstring>

using namespace std;

union t_x86 
{ 
    long double ld;
    struct 
    {
        uint8_t c[5];
    } composite;
};


/**
 * shift1right shift of 1 bit a array of bytes
 * @param sparc: array of input
 * @param x86: array of output
 * @param shifts: number of bytes
 *
 * 2011.01.06 asa Initial version
**/
void shift1right(const uint8_t* sparc, uint8_t* x86, int shifts)
{
    char carry0 = 0;
    char carry1 = 0;
    while ( shifts-- )
    {
        carry1 = (*sparc & 0x01) << 7; // the () are important because & is of lower precedence as <<
        *x86 = (*sparc >> 1) | carry0;
        carry0 = carry1;
        x86--; sparc++;
    }
}


/**
 * sparc2x86 converts a long double of sparc (128 bit) into a long double of x86 (96 bit)
 * @param sparc: input of sparc (binary array of 16 bytes)
 * @return output for x86 (12 bytes)
 *
 * 2011.01.06 asa Initial version
**/
long double sparc2x86(const uint8_t sparc[16]) 
{
    t_x86 x86;
    memset(&x86, 0, sizeof(x86));
    x86.composite.c[9] = sparc[0]; // higher byte of the exponent
    x86.composite.c[8] = sparc[1]; // lower byte of the exponent
    shift1right(&sparc[2], &x86.composite.c[7], 8); // shift the factor of 1 bit to the right
    if ( (sparc[0] & 0x7f) != 0 || sparc[1] != 0 ) x86.composite.c[7] |= 0x80; // leading bit
    return x86.ld;
}


int main(int argc, char* argv[])
{
    cout << "main - start" << endl;

//    uint8_t sparc[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // (0.0)
//    uint8_t sparc[16] = {0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // (+1.0)
//    uint8_t sparc[16] = {0xBF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // (-1.0)
//    uint8_t sparc[16] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // (+2.0)
    uint8_t sparc[16] = {0x40, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // (+3.0)
    long double x86 = sparc2x86(sparc);

    cout << "main - x86=" << x86 << "." << endl;
    
/*    t_x86 tmp;
    tmp.ld = 3;
    cout << "main - tmp.ld=" << tmp.ld << "." << endl;*/
    cout << "main - end" << endl;
    return 0;
}

/*
sparc
-----
128 bit (16 byte)
alignement 8 byte
1 bit Sign (1)
15 bit exponent
112 bit fraction
                |
 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
00000000000000000000000000000000 (0.0)
3fff0000000000000000000000000000 (+1.0)
BFFF0000000000000000000000000000 (-1.0)
40000000000000000000000000000000 (+2.0)
40080000000000000000000000000000 (+3.0)
7FFF0000000000000000000000000000 (+infinity)
FFFF0000000000000000000000000000 (-infinity)
7FFF7FFFFFFFFFFFFFFFFFFFFFFFFFFF (NAN)

40000000000000000000000000000000 (+2.0)
76543210765432107654321076543210765432107654321076543210765432107654321076543210765432107654321076543210765432107654321076543210
SEEEEEEEEEEEEEEEffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
00000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000

01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
s=0
e=16384
f=0
2^(16384-16383) * 1

127 sign
112-126 exponent
0-111 fraction

x86
---
96 bit (12 byte)
alignement 4 byte
3FFF8000000000000000

 1 2 3 4 5 6 7 8 9 0 1 2
    3FFF8000000000000000

80-95 unused
79 sign
64-78 exponent
63 leading bit
0-62 fraction


      12      11      10       9       8       7       6       5       4       3       2       1
765432107654321076543210765432107654321076543210765432107654321076543210765432107654321076543210
                SEEEEEEEEEEEEEEE fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff


*/
