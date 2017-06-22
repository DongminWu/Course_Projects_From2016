//Jiapeng Li 387565, Dongmin Wu 387563, Yuan Zhang 387552



#include <immintrin.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define IDCT_SIZE         16
#define ITERATIONS        1000000
#define MAX_NEG_CROP      1024

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

static const short g_aiT16[16][16] =
{
  { 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
  { 90, 87, 80, 70, 57, 43, 25,  9, -9,-25,-43,-57,-70,-80,-87,-90},
  { 89, 75, 50, 18,-18,-50,-75,-89,-89,-75,-50,-18, 18, 50, 75, 89},
  { 87, 57,  9,-43,-80,-90,-70,-25, 25, 70, 90, 80, 43, -9,-57,-87},
  { 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83},
  { 80,  9,-70,-87,-25, 57, 90, 43,-43,-90,-57, 25, 87, 70, -9,-80},
  { 75,-18,-89,-50, 50, 89, 18,-75,-75, 18, 89, 50,-50,-89,-18, 75},
  { 70,-43,-87,  9, 90, 25,-80,-57, 57, 80,-25,-90, -9, 87, 43,-70},
  { 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64},
  { 57,-80,-25, 90, -9,-87, 43, 70,-70,-43, 87,  9,-90, 25, 80,-57},
  { 50,-89, 18, 75,-75,-18, 89,-50,-50, 89,-18,-75, 75, 18,-89, 50},
  { 43,-90, 57, 25,-87, 70,  9,-80, 80, -9,-70, 87,-25,-57, 90,-43},
  { 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36},
  { 25,-70, 90,-80, 43,  9,-57, 87,-87, 57, -9,-43, 80,-90, 70,-25},
  { 18,-50, 75,-89, 89,-75, 50,-18,-18, 50,-75, 89,-89, 75,-50, 18},
  {  9,-25, 43,-57, 70,-80, 87,-90, 90,-87, 80,-70, 57,-43, 25, -9}
};

static int64_t diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    int64_t d;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    d = temp.tv_sec*1000000000+temp.tv_nsec;
    return d;
}

static void compare_results(short *ref, short *res, const char *msg)
{
    int correct =1;

    printf("Comparing %s\n",msg);
    for(int j=0; j<IDCT_SIZE; j++)  {
        for(int i=0; i<IDCT_SIZE; i++){
            if(ref[j*IDCT_SIZE+i] != res[j*IDCT_SIZE+i]){
                correct=0;
                printf("failed at %d,%d\t ref=%d, res=%d\n ", i, j, ref[j*IDCT_SIZE+i],res[j*IDCT_SIZE+i]);
            }
        }
    }
    if (correct){
        printf("correct\n\n");
    }
}

// this function is for timing, do not change anything here
static void benchmark( void (*idct16)(short *, short *), short *input, short *output, const char *version )
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME,&start);

    for(int i=0;i<ITERATIONS;i++)
        idct16(input, output);

    clock_gettime(CLOCK_REALTIME,&end);
    double avg = (double) diff(start,end)/ITERATIONS;
    printf("%10s:\t %.3f ns\n", version, avg);
}

//scalar code for the inverse transform
static void partialButterflyInverse16(short *src, short *dst, int shift)
{
  int E[8],O[8];
  int EE[4],EO[4];
  int EEE[2],EEO[2];
  int add = 1<<(shift-1);

  for (int j=0; j<16; j++)
  {
    /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
    for (int k=0; k<8; k++)
    {
      O[k] = g_aiT16[ 1][k]*src[ 16] + g_aiT16[ 3][k]*src[ 3*16] + g_aiT16[ 5][k]*src[ 5*16] + g_aiT16[ 7][k]*src[ 7*16] +
        g_aiT16[ 9][k]*src[ 9*16] + g_aiT16[11][k]*src[11*16] + g_aiT16[13][k]*src[13*16] + g_aiT16[15][k]*src[15*16];
    }
    for (int k=0; k<4; k++)
    {
      EO[k] = g_aiT16[ 2][k]*src[ 2*16] + g_aiT16[ 6][k]*src[ 6*16] + g_aiT16[10][k]*src[10*16] + g_aiT16[14][k]*src[14*16];
    }
    EEO[0] = g_aiT16[4][0]*src[ 4*16 ] + g_aiT16[12][0]*src[ 12*16 ];
    EEE[0] = g_aiT16[0][0]*src[ 0    ] + g_aiT16[ 8][0]*src[  8*16 ];
    EEO[1] = g_aiT16[4][1]*src[ 4*16 ] + g_aiT16[12][1]*src[ 12*16 ];
    EEE[1] = g_aiT16[0][1]*src[ 0    ] + g_aiT16[ 8][1]*src[  8*16 ];

    /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
    for (int k=0; k<2; k++)
    {
      EE[k] = EEE[k] + EEO[k];
      EE[k+2] = EEE[1-k] - EEO[1-k];
    }
    for (int k=0; k<4; k++)
    {
      E[k] = EE[k] + EO[k];
      E[k+4] = EE[3-k] - EO[3-k];
    }
    for (int k=0; k<8; k++)
    {
      dst[k]   = MAX( -32768, MIN( 32767, (E[k]   + O[k]   + add)>>shift ));
      dst[k+8] = MAX( -32768, MIN( 32767, (E[7-k] - O[7-k] + add)>>shift ));
    }
    src ++;
    dst += 16;
  }
}

static void idct16_scalar(short* pCoeff, short* pDst)
{
  short tmp[ 16*16] __attribute__((aligned(16)));
  partialButterflyInverse16(pCoeff, tmp, 7);
  partialButterflyInverse16(tmp, pDst, 12);
}

/// CURRENTLY SAME CODE AS SCALAR !!
/// REPLACE HERE WITH SSE intrinsics
static void partialButterflyInverse16_simd(short *src, short *dst, int shift)
{
  int E[8] __attribute__ ((aligned (16)));
  int O[8] __attribute__ ((aligned (16)));
  int EE[4] __attribute__ ((aligned (16)));
  int EO[4] __attribute__ ((aligned (16)));
  int EEE[4] __attribute__ ((aligned (16)));
  int EEO[4] __attribute__ ((aligned (16)));
  int add = 1<<(shift-1);

//cast to SIMD vector type

    __m128i *g_aiT16_vec  = (__m128i *)g_aiT16; //short, 16*16
    
    __m128i *O_vec = (__m128i *)O; 
    __m128i *E_vec = (__m128i *)E; 
    __m128i *EE_vec = (__m128i *)EE; 
    __m128i *EO_vec = (__m128i *)EO; 
    __m128i *EEE_vec = (__m128i *)EEE; 
    __m128i *EEO_vec = (__m128i *)EEO;

  __m128i *in_vec = (__m128i *) src;       
  __m128i *out_vec = (__m128i *) dst;

  short gt[8][8] __attribute__ ((aligned (16)));
  __m128i *gt_vec = (__m128i *)gt;

  short r[16][16] __attribute__ ((aligned (16)));
  __m128i *r_vec = (__m128i *)r;  
  

//transpose g_aiT16[1],[3],[5]...

__m128i a = _mm_load_si128(&g_aiT16_vec[2]); //g_aiT16[1,0],[1,1],...,[1,7]
__m128i b = _mm_load_si128(&g_aiT16_vec[6]); //g_aiT16[3,0],[3,1],...,[3,7]
__m128i c = _mm_load_si128(&g_aiT16_vec[10]);
__m128i d = _mm_load_si128(&g_aiT16_vec[14]);
__m128i e = _mm_load_si128(&g_aiT16_vec[18]);
__m128i f = _mm_load_si128(&g_aiT16_vec[22]);
__m128i g = _mm_load_si128(&g_aiT16_vec[26]);
__m128i h = _mm_load_si128(&g_aiT16_vec[30]);

__m128i t1 = _mm_unpacklo_epi16(a, b); //g_aiT16[1,0][3,0],[1,1],[3,1],[1,2],[3,2],[1,3],[3,3]
__m128i t2 = _mm_unpacklo_epi16(c, d); //g_aiT16[5,0][7,0],[5,1],[7,1],[5,2],[7,2],[5,3],[7,3]
__m128i t3 = _mm_unpacklo_epi16(e, f);
__m128i t4 = _mm_unpacklo_epi16(g, h);
__m128i t5 = _mm_unpackhi_epi16(a, b); //g_aiT16[1,4][3,4],[1,5],[3,5],[1,6],[3,6],[1,7],[3,7]
__m128i t6 = _mm_unpackhi_epi16(c, d);
__m128i t7 = _mm_unpackhi_epi16(e, f);
__m128i t8 = _mm_unpackhi_epi16(g, h);

__m128i t9 = _mm_unpacklo_epi32(t1, t2); //g_aiT16[1,0],[3,0],[5,0],[7,0],[1,1],[3,1],[5,1],[7,1]
__m128i t10 = _mm_unpackhi_epi32(t1, t2);
__m128i t11 = _mm_unpacklo_epi32(t3, t4);
__m128i t12 = _mm_unpackhi_epi32(t3, t4);
__m128i t13 = _mm_unpacklo_epi32(t5, t6);
__m128i t14 = _mm_unpackhi_epi32(t5, t6);
__m128i t15 = _mm_unpacklo_epi32(t7, t8);
__m128i t16 = _mm_unpackhi_epi32(t7, t8);
 
a = _mm_unpacklo_epi64(t9, t11);  //g_aiT16[1,0],[3,0],[5,0],...,[15,0]
b = _mm_unpackhi_epi64(t9, t11);
c = _mm_unpacklo_epi64(t10, t12);
d = _mm_unpackhi_epi64(t10, t12);
e = _mm_unpacklo_epi64(t13, t15);
f = _mm_unpackhi_epi64(t13, t15);
g = _mm_unpacklo_epi64(t14, t16);
h = _mm_unpackhi_epi64(t14, t16);

_mm_store_si128(&gt_vec[0], a);   //store 
_mm_store_si128(&gt_vec[1], b);   
_mm_store_si128(&gt_vec[2], c);
_mm_store_si128(&gt_vec[3], d);
_mm_store_si128(&gt_vec[4], e);
_mm_store_si128(&gt_vec[5], f);
_mm_store_si128(&gt_vec[6], g);
_mm_store_si128(&gt_vec[7], h);   //g_aiT16[1,7],[3,7],[5,7],...,[15,7]


//transpose src[1],[3],[5]...

//load src (short,16*16) 
a = _mm_load_si128(&in_vec[2]); //src[1,0],[1,1],...,[1,7]
b = _mm_load_si128(&in_vec[6]);
c = _mm_load_si128(&in_vec[10]);
d = _mm_load_si128(&in_vec[14]);
e = _mm_load_si128(&in_vec[18]);
f = _mm_load_si128(&in_vec[22]);
g = _mm_load_si128(&in_vec[26]);
h = _mm_load_si128(&in_vec[30]);


__m128i temp1 = _mm_unpacklo_epi16(a, b); //src[1,0][3,0],[1,1],[3,1],[1,2],[3,2],[1,3],[3,3]
__m128i temp2 = _mm_unpacklo_epi16(c, d);
__m128i temp3 = _mm_unpacklo_epi16(e, f);
__m128i temp4 = _mm_unpacklo_epi16(g, h);
__m128i temp5 = _mm_unpackhi_epi16(a, b);
__m128i temp6 = _mm_unpackhi_epi16(c, d);
__m128i temp7 = _mm_unpackhi_epi16(e, f);
__m128i temp8 = _mm_unpackhi_epi16(g, h);

__m128i temp9 = _mm_unpacklo_epi32(temp1, temp2); //src[1,0],[3,0],[5,0],[7,0],[1,1],[3,1],[5,1],[7,1]
__m128i temp10 = _mm_unpackhi_epi32(temp1, temp2);
__m128i temp11 = _mm_unpacklo_epi32(temp3, temp4);
__m128i temp12 = _mm_unpackhi_epi32(temp3, temp4);
__m128i temp13 = _mm_unpacklo_epi32(temp5, temp6);
__m128i temp14 = _mm_unpackhi_epi32(temp5, temp6);
__m128i temp15 = _mm_unpacklo_epi32(temp7, temp8);
__m128i temp16 = _mm_unpackhi_epi32(temp7, temp8);
 
__m128i T0 = _mm_unpacklo_epi64(temp9, temp11);  //src[1,0],[3,0],[5,0],...,[15,0]
__m128i T1 = _mm_unpackhi_epi64(temp9, temp11);
__m128i T2 = _mm_unpacklo_epi64(temp10, temp12);
__m128i T3 = _mm_unpackhi_epi64(temp10, temp12);
__m128i T4 = _mm_unpacklo_epi64(temp13, temp15);
__m128i T5 = _mm_unpackhi_epi64(temp13, temp15);
__m128i T6 = _mm_unpacklo_epi64(temp14, temp16);
__m128i T7 = _mm_unpackhi_epi64(temp14, temp16);

_mm_store_si128(&r_vec[0], T0);   //store 
_mm_store_si128(&r_vec[1], T1);
_mm_store_si128(&r_vec[2], T2);
_mm_store_si128(&r_vec[3], T3);
_mm_store_si128(&r_vec[4], T4);
_mm_store_si128(&r_vec[5], T5);
_mm_store_si128(&r_vec[6], T6);
_mm_store_si128(&r_vec[7], T7);



a = _mm_load_si128(&in_vec[3]); //src[1,8],[1,9],...,[1,15]
b = _mm_load_si128(&in_vec[7]);
c = _mm_load_si128(&in_vec[11]);
d = _mm_load_si128(&in_vec[15]);
e = _mm_load_si128(&in_vec[19]);
f = _mm_load_si128(&in_vec[23]);
g = _mm_load_si128(&in_vec[27]);
h = _mm_load_si128(&in_vec[31]);
   
temp1 = _mm_unpacklo_epi16(a, b); //src[1,8],[3,8],...,[1,11],[3,11]
temp2 = _mm_unpacklo_epi16(c, d);
temp3 = _mm_unpacklo_epi16(e, f);
temp4 = _mm_unpacklo_epi16(g, h);
temp5 = _mm_unpackhi_epi16(a, b);
temp6 = _mm_unpackhi_epi16(c, d);
temp7 = _mm_unpackhi_epi16(e, f);
temp8 = _mm_unpackhi_epi16(g, h);

temp9 = _mm_unpacklo_epi32(temp1, temp2); //src[1,8],[3,8],[5,8],[7,8],[1,9],[3,9],[5,9],[7,9]
temp10 = _mm_unpackhi_epi32(temp1, temp2);
temp11 = _mm_unpacklo_epi32(temp3, temp4);
temp12 = _mm_unpackhi_epi32(temp3, temp4);
temp13 = _mm_unpacklo_epi32(temp5, temp6);
temp14 = _mm_unpackhi_epi32(temp5, temp6);
temp15 = _mm_unpacklo_epi32(temp7, temp8);
temp16 = _mm_unpackhi_epi32(temp7, temp8);

T0 = _mm_unpacklo_epi64(temp9, temp11); //src[1,8],[3,8],...[15,8]
T1 = _mm_unpackhi_epi64(temp9, temp11);
T2 = _mm_unpacklo_epi64(temp10, temp12);
T3 = _mm_unpackhi_epi64(temp10, temp12);
T4 = _mm_unpacklo_epi64(temp13, temp15);
T5 = _mm_unpackhi_epi64(temp13, temp15);
T6 = _mm_unpacklo_epi64(temp14, temp16);
T7 = _mm_unpackhi_epi64(temp14, temp16); //src[1,15],[3,15],...,[15,15] 

_mm_store_si128(&r_vec[8], T0);   //store
_mm_store_si128(&r_vec[9], T1);
_mm_store_si128(&r_vec[10], T2);
_mm_store_si128(&r_vec[11], T3);
_mm_store_si128(&r_vec[12], T4);
_mm_store_si128(&r_vec[13], T5);
_mm_store_si128(&r_vec[14], T6);
_mm_store_si128(&r_vec[15], T7);



//transpose g_aiT16_vec[0],[2]...

 a = _mm_load_si128(&g_aiT16_vec[0]); //g_aiT16[0,0],[0,1],...,[0,7]
 b = _mm_load_si128(&g_aiT16_vec[4]); //g_aiT16[2,0],[2,1],...,[2,7]
 c = _mm_load_si128(&g_aiT16_vec[8]);
 d = _mm_load_si128(&g_aiT16_vec[12]);
 e = _mm_load_si128(&g_aiT16_vec[16]);
 f = _mm_load_si128(&g_aiT16_vec[20]);
 g = _mm_load_si128(&g_aiT16_vec[24]);
 h = _mm_load_si128(&g_aiT16_vec[28]);

 temp1 = _mm_unpacklo_epi16(a, b); //g_aiT16[0,0],[2,0],[0,1],[2,1],[0,2],[2,2],[0,3],[2,3]
 temp2 = _mm_unpacklo_epi16(c, d); 
 temp3 = _mm_unpacklo_epi16(e, f);
 temp4 = _mm_unpacklo_epi16(g, h);

 temp9 = _mm_unpacklo_epi32(temp1, temp2); //g_aiT16[0,0],[2,0],[4,0],[6,0],[0,1],[2,1],[4,1],[6,1]
 temp10 = _mm_unpackhi_epi32(temp1, temp2);
 temp11 = _mm_unpacklo_epi32(temp3, temp4);
 temp12 = _mm_unpackhi_epi32(temp3, temp4);

 T0 = _mm_unpacklo_epi64(temp9, temp11); //g_aiT16[0,0],[2,0],[4,0],[6,0],[8,0],[10,0],[12,0],[14,0]
 T1 = _mm_unpackhi_epi64(temp9, temp11); //g_aiT16[0,1],[2,1],[4,1],[6,1],[8,1],[10,1],[12,1],[14,1]
 T2 = _mm_unpacklo_epi64(temp10, temp12); //g_aiT16[0,2],[2,2],[4,2],[6,2],[8,2],[10,2],[12,2],[14,2]
 T3 = _mm_unpackhi_epi64(temp10, temp12); //g_aiT16[0,3],[2,3],[4,3],[6,3],[8,3],[10,3],[12,3],[14,3]


  
_mm_store_si128(&gt_vec[8], T0);  //store
_mm_store_si128(&gt_vec[9], T1);
_mm_store_si128(&gt_vec[10], T2);
_mm_store_si128(&gt_vec[11], T3);


// transpose src[0],[2]
a = _mm_load_si128(&in_vec[0]); //src[0,0],[0,1],...,[0,7]
b = _mm_load_si128(&in_vec[4]); //src[2,0],[2,1],...,[2,7]
c = _mm_load_si128(&in_vec[8]);
d = _mm_load_si128(&in_vec[12]);
e = _mm_load_si128(&in_vec[16]);
f = _mm_load_si128(&in_vec[20]);
g = _mm_load_si128(&in_vec[24]);
h = _mm_load_si128(&in_vec[28]);


 temp1 = _mm_unpacklo_epi16(a, b); //src[0,0],[2,0],[0,1],[2,1],[0,2],[2,2],[0,3],[2,3]
 temp2 = _mm_unpacklo_epi16(c, d);
 temp3 = _mm_unpacklo_epi16(e, f);
 temp4 = _mm_unpacklo_epi16(g, h);
 temp5 = _mm_unpackhi_epi16(a, b);
 temp6 = _mm_unpackhi_epi16(c, d);
 temp7 = _mm_unpackhi_epi16(e, f);
 temp8 = _mm_unpackhi_epi16(g, h);

 temp9 = _mm_unpacklo_epi32(temp1, temp2); //src[0,0],[2,0],[4,0],[6,0],[0,1],[2,1],[4,1],[6,1]
 temp10 = _mm_unpackhi_epi32(temp1, temp2);
 temp11 = _mm_unpacklo_epi32(temp3, temp4);
 temp12 = _mm_unpackhi_epi32(temp3, temp4);
 temp13 = _mm_unpacklo_epi32(temp5, temp6);
 temp14 = _mm_unpackhi_epi32(temp5, temp6);
 temp15 = _mm_unpacklo_epi32(temp7, temp8);
 temp16 = _mm_unpackhi_epi32(temp7, temp8);

 T0 = _mm_unpacklo_epi64(temp9, temp11); //src[0,0],[2,0],[4,0],[6,0],[8,0],[10,0],[12,0],[14,0]
 T1 = _mm_unpackhi_epi64(temp9, temp11); //src[0,1],[2,1],[4,1],[6,1],[8,1],[10,1],[12,1],[14,1]
 T2 = _mm_unpacklo_epi64(temp10, temp12);
 T3 = _mm_unpackhi_epi64(temp10, temp12);
 T4 = _mm_unpacklo_epi64(temp13, temp15);
 T5 = _mm_unpackhi_epi64(temp13, temp15);
 T6 = _mm_unpacklo_epi64(temp14, temp16);
 T7 = _mm_unpackhi_epi64(temp14, temp16);

_mm_store_si128(&r_vec[16], T0);   //store 
_mm_store_si128(&r_vec[17], T1);
_mm_store_si128(&r_vec[18], T2);
_mm_store_si128(&r_vec[19], T3);
_mm_store_si128(&r_vec[20], T4);
_mm_store_si128(&r_vec[21], T5);
_mm_store_si128(&r_vec[22], T6);
_mm_store_si128(&r_vec[23], T7);
      
	  
	  
a = _mm_load_si128(&in_vec[1]); //src[0,8],[0,9],[0,10],[0,11],[0,12],[0,13],[0,14],[0,15]
b = _mm_load_si128(&in_vec[5]); //src[2,8],[2,9],[2,10],[2,11],[2,12],[2,13],[2,14],[2,15]
c = _mm_load_si128(&in_vec[9]);
d = _mm_load_si128(&in_vec[13]);
e = _mm_load_si128(&in_vec[17]);
f = _mm_load_si128(&in_vec[21]);
g = _mm_load_si128(&in_vec[25]);
h = _mm_load_si128(&in_vec[29]);
   
 temp1 = _mm_unpacklo_epi16(a, b); //src[0,8],[2,8],[0,9],[2,9],[0,10],[2,10],[0,11],[2,11]
 temp2 = _mm_unpacklo_epi16(c, d); 
 temp3 = _mm_unpacklo_epi16(e, f);
 temp4 = _mm_unpacklo_epi16(g, h);
 temp5 = _mm_unpackhi_epi16(a, b); //src[0,12],[2,12],[0,13],[2,13],[0,14],[2,14],[0,15],[2,15]
 temp6 = _mm_unpackhi_epi16(c, d);
 temp7 = _mm_unpackhi_epi16(e, f);
 temp8 = _mm_unpackhi_epi16(g, h);

 temp9 = _mm_unpacklo_epi32(temp1, temp2); //src[0,8],[2,8],[4,8],[6,8],[0,9],[2,9],[4,9],[6,9]
 temp10 = _mm_unpackhi_epi32(temp1, temp2);
 temp11 = _mm_unpacklo_epi32(temp3, temp4);
 temp12 = _mm_unpackhi_epi32(temp3, temp4);
 temp13 = _mm_unpacklo_epi32(temp5, temp6);
 temp14 = _mm_unpackhi_epi32(temp5, temp6);
 temp15 = _mm_unpacklo_epi32(temp7, temp8);
 temp16 = _mm_unpackhi_epi32(temp7, temp8);

 T0 = _mm_unpacklo_epi64(temp9, temp11); //src[0,8],[2,8],[4,8],[6,8],[8,8],[10,8],[12,8],[14,8]
 T1 = _mm_unpackhi_epi64(temp9, temp11);
 T2 = _mm_unpacklo_epi64(temp10, temp12);
 T3 = _mm_unpackhi_epi64(temp10, temp12);
 T4 = _mm_unpacklo_epi64(temp13, temp15);
 T5 = _mm_unpackhi_epi64(temp13, temp15);
 T6 = _mm_unpacklo_epi64(temp14, temp16);
 T7 = _mm_unpackhi_epi64(temp14, temp16);  

_mm_store_si128(&r_vec[24], T0);   //store 
_mm_store_si128(&r_vec[25], T1);
_mm_store_si128(&r_vec[26], T2);
_mm_store_si128(&r_vec[27], T3);
_mm_store_si128(&r_vec[28], T4);
_mm_store_si128(&r_vec[29], T5);
_mm_store_si128(&r_vec[30], T6);
_mm_store_si128(&r_vec[31], T7);  
  

  for (int j=0; j<16; j++)
  {

      /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */ 
    __m128i I0 = _mm_load_si128 (&r_vec[j]); //src[1,j],[3,j],...,[15,j]
    __m128i II0 = _mm_load_si128 (&r_vec[j+16]); //src[0,j],[2,j],...[14,j]

  

        __m128i I1 = _mm_load_si128 (&gt_vec[0]);  //g_aiT16[1,0],[3,0],...,[15,0] 
        __m128i I2 = _mm_madd_epi16 (I1, I0);      
         
        __m128i I3 = _mm_load_si128 (&gt_vec[1]);   
        __m128i I4 = _mm_madd_epi16 (I3, I0);
   
        __m128i I5 = _mm_load_si128 (&gt_vec[2]);   
        __m128i I6 = _mm_madd_epi16 (I5, I0);

        __m128i I7 = _mm_load_si128 (&gt_vec[3]);   
        __m128i I8 = _mm_madd_epi16 (I7, I0);

        __m128i I9 = _mm_load_si128 (&gt_vec[4]);   
        __m128i I10 = _mm_madd_epi16 (I9, I0);

        __m128i I11 = _mm_load_si128 (&gt_vec[5]);   
        __m128i I12 = _mm_madd_epi16 (I11, I0);

        __m128i I13 = _mm_load_si128 (&gt_vec[6]);   
        __m128i I14 = _mm_madd_epi16 (I13, I0);

        __m128i I15 = _mm_load_si128 (&gt_vec[7]);   
        __m128i I16 = _mm_madd_epi16 (I15, I0);

       __m128i A1 =_mm_hadd_epi32 (I2, I4); 
       __m128i A2 =_mm_hadd_epi32 (I6, I8);
       __m128i R1 =_mm_hadd_epi32 (A1, A2); // O[0,j],O[1,j],O[2,j],O[3,j]

       __m128i A3 =_mm_hadd_epi32 (I10, I12);
       __m128i A4 =_mm_hadd_epi32 (I14, I16);
       __m128i R2 =_mm_hadd_epi32 (A3, A4); // O[4,j],O[5,j],O[6,j],O[7,j]
 
	  __m128i zeros = _mm_set1_epi32(0);
   
       I1 = _mm_load_si128 (&gt_vec[8]);  //g_aiT16[0,0],[2,0],...,[14,0]     
       I2 = _mm_mullo_epi16 (I1, II0);
       I3 = _mm_mulhi_epi16 (I1, II0);
      __m128i lowI23 = _mm_unpacklo_epi16(I2,I3);//EEE[0](0),EO[0](0),EEO[0](0),EO[0](1)
      __m128i hiI23 = _mm_unpackhi_epi16(I2,I3); //EEE[0](1),EO[0](2),EEO[0](1),EO[0](3)   
	 
	  //start
	  __m128i EEE_EO_0 = _mm_unpacklo_epi32(lowI23,hiI23);
	  __m128i EEO_EO_0 = _mm_unpackhi_epi32(lowI23,hiI23);
	  __m128i EEE_EEO_0 = _mm_unpacklo_epi64(EEE_EO_0,EEO_EO_0);
	  __m128i EO_0 = _mm_unpackhi_epi64(EEE_EO_0,EEO_EO_0);
	  //end

       I4 = _mm_load_si128 (&gt_vec[9]);  //g_aiT16[0,1],[2,1],...,[14,1]     
       I5 = _mm_mullo_epi16 (I4, II0);
       I6 = _mm_mulhi_epi16 (I4, II0);
      __m128i lowI56 = _mm_unpacklo_epi16(I5,I6);
      __m128i hiI56 = _mm_unpackhi_epi16(I5,I6);    
	  //start
	  __m128i EEE_EO_1 = _mm_unpacklo_epi32(lowI56,hiI56);
	  __m128i EEO_EO_1 = _mm_unpackhi_epi32(lowI56,hiI56);
	  __m128i EEE_EEO_1 = _mm_unpacklo_epi64(EEE_EO_1,EEO_EO_1);
	  __m128i EO_1 = _mm_unpackhi_epi64(EEE_EO_1,EEO_EO_1);
	  //end
             
       I7 = _mm_load_si128 (&gt_vec[10]);      
       I8 = _mm_mullo_epi16 (I7, II0);
       I9 = _mm_mulhi_epi16 (I7, II0);
      __m128i lowI89 = _mm_unpacklo_epi16(I8,I9);
      __m128i hiI89 = _mm_unpackhi_epi16(I8,I9);    
	  //start
	  __m128i EEE_EO_2 = _mm_unpacklo_epi32(lowI89,hiI89);
	  __m128i EEO_EO_2 = _mm_unpackhi_epi32(lowI89,hiI89);
	  __m128i EEE_EEO_2 = _mm_unpacklo_epi64(EEE_EO_2,EEO_EO_2);
	  __m128i EO_2 = _mm_unpackhi_epi64(EEE_EO_2,EEO_EO_2);
	  //end

       I10 = _mm_load_si128 (&gt_vec[11]);       
       I11 = _mm_mullo_epi16 (I10, II0);
       I12 = _mm_mulhi_epi16 (I10, II0);
      __m128i lowI1112 = _mm_unpacklo_epi16(I11,I12);
      __m128i hiI1112 = _mm_unpackhi_epi16(I11,I12);    
	  //start
	  __m128i EEE_EO_3 = _mm_unpacklo_epi32(lowI1112,hiI1112);
	  __m128i EEO_EO_3 = _mm_unpackhi_epi32(lowI1112,hiI1112);
	  __m128i EEE_EEO_3 = _mm_unpacklo_epi64(EEE_EO_3,EEO_EO_3);
	  __m128i EO_3 = _mm_unpackhi_epi64(EEE_EO_3,EEO_EO_3);
	  //end

	  __m128i EO_0_1 = _mm_hadd_epi32(EO_0,EO_1);
	  __m128i EO_2_3 = _mm_hadd_epi32(EO_2,EO_3);
	  __m128i EO_0_1_2_3 = _mm_hadd_epi32(EO_0_1,EO_2_3);

	  __m128i EO_1_0 = _mm_hadd_epi32(EO_1,EO_0);
	  __m128i EO_3_2 = _mm_hadd_epi32(EO_3,EO_2);
	  __m128i EO_3_2_1_0 = _mm_hadd_epi32(EO_1_0,EO_3_2);

	  __m128i EEE_EEO_0_EEE_EEO_1 = _mm_hadd_epi32(EEE_EEO_0, EEE_EEO_1); //5
	  __m128i EEE_EEO_1_EEE_EEO_0 = _mm_hadd_epi32(EEE_EEO_1, EEE_EEO_0); //5'

	  __m128i EE_0_1 = _mm_hadd_epi32(EEE_EEO_0_EEE_EEO_1,zeros);
	  __m128i EE_2_3 = _mm_hsub_epi32(zeros,EEE_EEO_1_EEE_EEO_0);
	  __m128i EE_0_1_2_3 = _mm_add_epi32(EE_0_1, EE_2_3);

	  __m128i EE_1_0 = _mm_hadd_epi32(zeros,EEE_EEO_1_EEE_EEO_0);
	  __m128i EE_3_2 = _mm_hsub_epi32(EEE_EEO_0_EEE_EEO_1,zeros);
	  __m128i EE_3_2_1_0 = _mm_add_epi32(EE_3_2, EE_1_0);

 
       __m128i R3 =_mm_add_epi32 (EE_0_1_2_3, EO_0_1_2_3); // E[0] to E[3]
       __m128i R4 =_mm_sub_epi32 (EE_3_2_1_0, EO_3_2_1_0);//E[4] to E[7]

 /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
         __m128i add_reg = _mm_set1_epi32(add);

         __m128i sum_vec0 = _mm_add_epi32(R3,R1);        
         sum_vec0 = _mm_add_epi32(sum_vec0,add_reg); 
         sum_vec0 = _mm_srai_epi32(sum_vec0, shift); // shift right
	 
         
         __m128i sum_vec1 = _mm_add_epi32(R4,R2); 
         sum_vec1 = _mm_add_epi32(sum_vec1,add_reg); 
         sum_vec1 = _mm_srai_epi32(sum_vec1, shift); // shift right

	     __m128i finalres0 = _mm_packs_epi32(sum_vec0, sum_vec1); // shrink packed 32bit to packed 16 bit and saturate
         _mm_store_si128 (&out_vec[2*j], finalres0); //out_vec[0],][2],[4],...,[30]
         
        __m128i  sum_vec2 = _mm_sub_epi32(R4, R2);
         sum_vec2 = _mm_add_epi32(sum_vec2,add_reg);
         sum_vec2 = _mm_srai_epi32(sum_vec2, shift); // shift right  	 

         __m128i sum_vec3 = _mm_sub_epi32(R3, R1);
         sum_vec3 = _mm_add_epi32(sum_vec3,add_reg);
         sum_vec3 = _mm_srai_epi32(sum_vec3, shift); // shift right

         I5 = _mm_unpackhi_epi32(sum_vec2, sum_vec3);
         I6 = _mm_unpacklo_epi32(sum_vec2, sum_vec3);
         I7 = _mm_unpackhi_epi32(I5, I6);
         I8 = _mm_unpacklo_epi32(I5, I6);
         I9 = _mm_unpacklo_epi32(I7, I8);
         I10 = _mm_unpackhi_epi32(I7, I8);
         
	     sum_vec3 = _mm_packs_epi32(I9, I10); // shrink packed 32bit to packed 16 bit and saturate
         _mm_store_si128 (&out_vec[2*j+1], sum_vec3);

  	 src ++;
  	 dst += 16;
  }
}
///end of simd codes!

static void idct16_simd(short* pCoeff, short* pDst)
{
  short tmp[ 16*16] __attribute__((aligned(16)));
  partialButterflyInverse16_simd(pCoeff, tmp, 7);
  partialButterflyInverse16_simd(tmp, pDst, 12);
}

int main(int argc, char **argv)
{
    //allocate memory 16-byte aligned
    short *scalar_input = (short*) memalign(16, IDCT_SIZE*IDCT_SIZE*sizeof(short));
    short *scalar_output = (short *) memalign(16, IDCT_SIZE*IDCT_SIZE*sizeof(short));

    short *simd_input = (short*) memalign(16, IDCT_SIZE*IDCT_SIZE*sizeof(short));
    short *simd_output = (short *) memalign(16, IDCT_SIZE*IDCT_SIZE*sizeof(short));

    //initialize input
    printf("input array:\n");
    for(int j=0;j<IDCT_SIZE;j++){
        for(int i=0;i<IDCT_SIZE;i++){
            short value = rand()%2 ? (rand()%32768) : -(rand()%32768) ;
            scalar_input[j*IDCT_SIZE+i] = value;
            simd_input  [j*IDCT_SIZE+i] = value;
	    printf("%d\t", value);
        }
        printf("\n");
    }

    idct16_scalar(scalar_input, scalar_output);
    idct16_simd  (simd_input  , simd_output);

    //check for correctness
    compare_results (scalar_output, simd_output, "scalar and simd");

    printf("output array:\n");
    for(int j=0;j<IDCT_SIZE;j++){
        for(int i=0;i<IDCT_SIZE;i++){
	    printf("%d\t", scalar_output[j*IDCT_SIZE+i]);
        }
        printf("\n");
    }
    printf("output array simd:\n");
    for(int j=0;j<IDCT_SIZE;j++){
        for(int i=0;i<IDCT_SIZE;i++){
	    printf("%d\t", simd_output[j*IDCT_SIZE+i]);
        }
        printf("\n");
    }

    //Measure the performance of each kernel
    benchmark (idct16_scalar, scalar_input, scalar_output, "scalar");
    benchmark (idct16_simd, simd_input, simd_output, "simd");

    //cleanup
    free(scalar_input);    free(scalar_output);
    free(simd_input); free(simd_output);
}
