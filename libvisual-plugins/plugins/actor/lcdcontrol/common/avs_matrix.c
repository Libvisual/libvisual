#include <math.h>
#include <string.h>

void matrixRotate(float matrix[], char m, float Deg) {
  char m1, m2;
  float c,s;
  Deg *= 3.141592653589f / 180.0f;
  memset(matrix,0,sizeof(float)*16);
  matrix[((m-1)<<2)+m-1] = matrix[15] = 1.0f;
  m1 = (m % 3);
  m2 = ((m1+1) % 3);
  c = (float)cos(Deg); s = (float)sin(Deg);
  matrix[(m1<<2)+m1]=c; matrix[(m1<<2)+m2]=s;
  matrix[(m2<<2)+m2]=c; matrix[(m2<<2)+m1]=-s;
}

void matrixTranslate(float m[], float x, float y, float z) {
  memset(m,0,sizeof(float)*16);
  m[0] = m[4+1] = m[8+2] = m[12+3] = 1.0f;
  m[0+3] = x; m[4+3] = y; m[8+3] = z;
}

void matrixMultiply(float *dest, float src[]) {
  float temp[16];
  int i;
  memcpy(temp,dest,sizeof(float)*16);
  for (i = 0; i < 16; i += 4) {
    *dest++ = src[i+0]*temp[(0<<2)+0]+src[i+1]*temp[(1<<2)+0]+
              src[i+2]*temp[(2<<2)+0]+src[i+3]*temp[(3<<2)+0];
    *dest++ = src[i+0]*temp[(0<<2)+1]+src[i+1]*temp[(1<<2)+1]+
              src[i+2]*temp[(2<<2)+1]+src[i+3]*temp[(3<<2)+1];
    *dest++ = src[i+0]*temp[(0<<2)+2]+src[i+1]*temp[(1<<2)+2]+
              src[i+2]*temp[(2<<2)+2]+src[i+3]*temp[(3<<2)+2];
    *dest++ = src[i+0]*temp[(0<<2)+3]+src[i+1]*temp[(1<<2)+3]+
              src[i+2]*temp[(2<<2)+3]+src[i+3]*temp[(3<<2)+3];
  }
}

void matrixApply(float *m, float x, float y, float z,
                 float *outx, float *outy, float *outz) {
  *outx = x*m[0] + y*m[1] + z*m[2] + m[3];
  *outy	= x*m[4] + y*m[5] + z*m[6] + m[7];
  *outz = x*m[8] + y*m[9] + z*m[10] + m[11];
}
