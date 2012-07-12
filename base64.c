// fastest encoding variation I've come up with (Not really, see comments after the article!)
// copied from
// http://www.experts-exchange.com/Programming/System/Windows__Programming/3_216-Fast-Base64-Encode-and-Decode.html
//

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <lua.h>
#include <lauxlib.h>


static char Base64Digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



unsigned short Base64Digits8192[ 4096 ];
static void SetupTable8192()
{
    int j,k;
    for (j=0; j<64; j++ ) {
        for (k=0; k<64; k++ ) {
            unsigned short w;
            w  = Base64Digits[k] << 8;
            w |= Base64Digits[j]; 
            Base64Digits8192[(j*64)+k]= w;
        }
    }
}
//--------------------------
// or have a pre-built table, something like:
//BYTE Base64Digits8192[ ((64*64)*2)+1 ]=  // 8192+1 
//"AAABACADAEAFAGAHAIAJAKALAMANAOAPAQARASATA ... A6A7A8A9A+A/"
//"BABBBCBDBEBFBGBHBIBJBKBLBMBNBOBPBQBRBSBTB ... B6B7B8B9B+B/"
//...
//"9A9B9C9D9E9F9G9H9I9J9K9L9M9N9O9P9Q9R9S9T9...  5969798999+9/"
//"+A+B+C+D+E+F+G+H+I+J+K+L+M+N+O+P+Q+R+S+T+...  5+6+7+8+9+++/"
//"/A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/...  5/6/7/8/9/+//"
//;



//  simple version illustrates the basic decode algorithm
//

int ToBase64Fast( const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst )
{
   int nLenOut= ((nLenSrc+2)/3)*4; // 4 out for every 3 in, rounded up
   if ( nLenOut+1 > nLenDst ) {
         return( 0 ); // fail!
   }

   unsigned short* pwDst= (unsigned short*)pDst;
   while( nLenSrc > 2 ) {
      unsigned int n= pSrc[0];  // xxx1
      n <<= 8;           // xx1x
      n |= pSrc[1];      // xx12  
      n <<= 8;           // x12x
      n |= pSrc[2];      // x123  

      pwDst[0]= Base64Digits8192[ n >> 12 ]; 
      pwDst[1]= Base64Digits8192[ n & 0x00000fff ]; 

      nLenSrc -= 3;
      pwDst += 2;
      pSrc += 3;
   }
   // -------------- end of buffer special handling (see text)   
   pDst= (char*)pwDst;
   
   if ( nLenSrc > 0 ) {  // some left after last triple
      int n1= (*pSrc & 0xfc) >> 2;
      int n2= (*pSrc & 0x03) << 4;
      if (nLenSrc > 1 ) {  // corrected.  Thanks to jprichey
            pSrc++;
            n2 |= (*pSrc & 0xf0) >> 4;
      }
      *pDst++ = Base64Digits[n1]; // encode at least 2 outputs
      *pDst++ = Base64Digits[n2];
      if (nLenSrc == 2) {  // 2 src bytes left to encode, output xxx=
         int n3= (*pSrc & 0x0f) << 2;
         pSrc++;
         n3 |= (*pSrc & 0xc0) >> 6;
         *pDst++ = Base64Digits[n3];
      }
      if (nLenSrc == 1) {  // 1 src byte left to encode, output xx==
         *pDst++ = '=';
      }
      *pDst++ = '=';
   }
   // *pDst= 0; nLenOut++ // could terminate with NULL, here
   return( nLenOut );
}


//----------------------------------------------------
// Using two-byte lookup table
// must call here before calling the above
//----------------------------------------------------


static unsigned char LookupDigits[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //gap: ctrl chars
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //gap: ctrl chars
    0,0,0,0,0,0,0,0,0,0,0,           //gap: spc,!"#$%'()*
    62,                   // +
    0, 0, 0,             // gap ,-.
    63,                   // /
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // 0-9
    0, 0, 0,             // gap: :;<
    99,                   //  = (end padding)
    0, 0, 0,             // gap: >?@
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25, // A-Z
    0, 0, 0, 0, 0, 0,    // gap: [\]^_`
    26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,
    43,44,45,46,47,48,49,50,51, // a-z    
    0, 0, 0, 0,          // gap: {|}~ (and the rest...)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

unsigned short gpLookup16[256*256];

void SetupLookup16()
{
    int j,k;
    unsigned short* p= gpLookup16;
    for (j= 0; j<256; j++ ) {
        for ( k= 0; k<256; k++ ) {
            unsigned short w;
            w  = LookupDigits[k] << 8;
            w |= LookupDigits[j] << 2 ; // pre-shifted! See notes
            *p++ = w;
        }
    }
}

int FromBase64Simple( const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst )
{
   int nLenOut= 0;
   int j;
   for(j=0; j<nLenSrc; j+=4 ) {
      if ( nLenOut > nLenDst ) {
         return( 0 ); // error, buffer too small
      }
      unsigned char s1= LookupDigits[ *pSrc++ ];
      unsigned char s2= LookupDigits[ *pSrc++ ];
      unsigned char s3= LookupDigits[ *pSrc++ ];
      unsigned char s4= LookupDigits[ *pSrc++ ];

      unsigned char d1= ((s1 & 0x3f) << 2) | ((s2 & 0x30) >> 4);
      unsigned char d2= ((s2 & 0x0f) << 4) | ((s3 & 0x3c) >> 2);
      unsigned char d3= ((s3 & 0x03) << 6) | ((s4 & 0x3f) >> 0);

      *pDst++ = d1;  nLenOut++;
      if (s3==99) break;      // end padding found
      *pDst++ = d2;  nLenOut++;
      if (s4==99) break;      // end padding found
      *pDst++ = d3;  nLenOut++;
   }
   return( nLenOut );
}


// Utra-Fast base64-decoding, using 128KB lookup table
int FromBase64Fast( const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst )
{
    int j;
    
    int nLenOut= 0;
    if ( nLenDst < ((nLenSrc / 4)-1) * 3 ) {
        return (0); // (buffer too small)
    }
    int nLoopMax= (nLenSrc/4)-1;
    unsigned short* pwSrc= (unsigned short*)pSrc;
    for(j=0; j<nLoopMax; j++ ) {
        unsigned short s1= gpLookup16[ pwSrc[0] ];  // translate two "digits" at once
        unsigned short s2= gpLookup16[ pwSrc[1] ];  // ... and two more

        unsigned int n32;
        n32  = s1;         // xxxxxxxx xxxxxxxx xx111111 222222xx
        n32 <<= 10;        // xxxxxxxx 11111122 2222xxxx xxxxxxxx
        n32 |= s2 >> 2;    // xxxxxxxx 11111122 22223333 33444444

        unsigned char b3= ( n32 & 0x00ff ); n32 >>= 8;  // in reverse (WORD order)
        unsigned char b2= ( n32 & 0x00ff ); n32 >>= 8;
        unsigned char b1= ( n32 & 0x00ff ); 

        // *pDst++ = b1;  *pDst++ = b2;  *pDst++ = b3;  //slighly slower

        pDst[0]= b1;  // slightly faster
        pDst[1]= b2;
        pDst[2]= b3; 

        pwSrc += 2;
        pDst += 3;
    }
    nLenOut= ((nLenSrc/4)-1) * 3;
    //    fprintf(stderr, "ORIGNLENOUT:%d nLenDst:%d\n", (int)nLenOut, (int) nLenDst );

    if( nLenOut >= nLenDst ) return 0; // error

    unsigned char *curSrc = (unsigned char*) pwSrc;
    char inputLeft[3];
    size_t bytesLeft = nLenSrc - (curSrc - pSrc);
    int leftDecodeLen;
    
    //    fprintf(stderr, "bytesleft:%d\n", (int)bytesLeft );

    for(j=0;j<bytesLeft;j++){
        inputLeft[0] = curSrc[j];
    }

    char leftDecodeOut[3];
    
    leftDecodeLen = FromBase64Simple( curSrc, bytesLeft, leftDecodeOut, sizeof(leftDecodeOut) );
    //    fprintf(stderr, "leftDecodeLen:%d\n", (int)leftDecodeLen );

    for(j=0;j<leftDecodeLen;j++){
        pDst[j] = leftDecodeOut[j];
    }
    nLenOut += leftDecodeLen;
    return nLenOut;
    
#if 0    
    //-------------------- special handling outside of loop for end
    unsigned short s1= gpLookup16[ pwSrc[0] ];
    unsigned short s2= gpLookup16[ pwSrc[1] ]; 

    unsigned int n32;
    n32  = s1;      
    n32 <<= 10;
    n32 |= s2 >> 2;

    unsigned char b3= ( n32 & 0x00ff ); n32 >>= 8;
    unsigned char b2= ( n32 & 0x00ff ); n32 >>= 8;
    unsigned char b1= ( n32 & 0x00ff ); 

    fprintf(stderr, "B1:%d B2:%d B3:%d\n",b1,b2,b3 );
    
    if (nLenOut >= nLenDst)       return(0); // error
    *pDst++ = b1;
    nLenOut++;
    if (b2 != 99)  {
        fprintf(stderr,"PADDING2:%d\n",nLenOut);
        if (nLenOut >= nLenDst)   return(0); // error
        *pDst++ = b2;
        nLenOut++;
    }
    if (b3 != 99) {
        fprintf(stderr,"PADDING3:%d\n",nLenOut);        
        if (nLenOut >= nLenDst)   return(0); // error
        *pDst++ = b3;
        nLenOut++;
    }
    return( nLenOut );
#endif    
} 



static int encode(lua_State *L) {

    size_t len;
    unsigned char *srcptr = (unsigned char*) luaL_checklstring(L, 1, &len);
    size_t outmaxlen = len*2 + 32;
    void *outptr = lua_newuserdata( L, outmaxlen );
    int enclen = ToBase64Fast( srcptr, len, outptr, outmaxlen );

    lua_pushlstring(L,outptr,enclen);
    return 1;
}

static int decode(lua_State *L){
    size_t len;
    unsigned char *srcptr = (unsigned char*) luaL_checklstring(L,1,&len);
    size_t outmaxlen = len;
    void *outptr = lua_newuserdata( L, outmaxlen );
    int declen = FromBase64Fast( srcptr, len, outptr, outmaxlen );
    //    fprintf(stderr, "DECLEN:%d\n",declen );
    lua_pushlstring(L,outptr,declen);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////


static const luaL_reg exports[] = {
    {"encode", encode},
    {"decode", decode},
    {NULL, NULL}
};

LUALIB_API int luaopen_base64(lua_State *L) {
    SetupTable8192();
    SetupLookup16();
    
    lua_newtable(L);
    luaL_register(L, NULL, exports);

    // return the new module
    return 1;
}

