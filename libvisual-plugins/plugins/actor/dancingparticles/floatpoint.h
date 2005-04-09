#ifndef FLOATPOINT_HH
#define FLOATPOINT_HH

#include <iostream>
#include <GL/gl.h>
using namespace std;

#define myfloat  GLfloat

struct Point
{
  int h,v;
};
struct Rect{
  int top,bottom,left,right;
};

typedef int Boolean;
 
class FloatPoint
{
public :
  myfloat v[3];
  FloatPoint operator +=(const FloatPoint &b){
    for(int i=0;i<3;i++)
      v[i]+=b.v[i];
    return *this;
  };

  FloatPoint operator -=(const FloatPoint &b){
    for(int i=0;i<3;i++)
      v[i]-=b.v[i];
    return *this;
  };

  FloatPoint operator *=(const myfloat &b){
    for(int i=0;i<3;i++)
      v[i]*=b;
    return *this;
  };

  FloatPoint operator /=(const myfloat &b){
    for(int i=0;i<3;i++)
      v[i]/=b;
    return *this;
  };

	
  FloatPoint(const Point &P){
    v[0]=P.h;
    v[1]=P.v;
    v[2]=20;
  };
  FloatPoint(const FloatPoint &P){
    visual_mem_copy(v,P.v,3*sizeof(myfloat));
  };

  FloatPoint(){
    v[0]=0;
    v[1]=0;
    v[2]=20;	  
  };
  FloatPoint (myfloat a, myfloat b, myfloat c){
    v[0]=a;
    v[1]=b;
    v[2]=c;	  
  };
  FloatPoint (myfloat *a){
    v[0]=a[0];
    v[1]=a[1];
    v[2]=a[2];
  }
  FloatPoint (myfloat a){
    v[0]=a;
    v[1]=a;
    v[2]=a;
  }
  FloatPoint & operator = (myfloat a)
  {
    v[0]=a;
    v[1]=a;
    v[2]=a;
	return *this;
  }
  FloatPoint & operator = (FloatPoint a)
  {
    v[0]=a[0];
    v[1]=a[1];
    v[2]=a[2];	  
	return *this;
  }
  myfloat &operator [] (int i) { return v[i]; }
  const myfloat &operator [] (int i) const{ return v[i]; }

  inline myfloat length(){return (myfloat)fastsqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);}
  void display()
  {
    cout << v[0] <<" "<< v[1] <<" " << v[2] <<endl;  
  }


};
inline	FloatPoint operator+ (const FloatPoint &a, const FloatPoint &b){
		FloatPoint c(a);
		for(int i=0;i<3;i++)
		  c[i]+=b[i];
		return c;
		};
inline	FloatPoint operator- (const FloatPoint &a, const FloatPoint &b){
  FloatPoint c(a);
  for(int i=0;i<3;i++)
    c[i]-=b[i];
  return c;
}
inline	FloatPoint operator* (const FloatPoint &a, const myfloat &b){
  FloatPoint c(a);
  for(int i=0;i<3;i++)
    c[i]*=b;
  return c;
}
inline	FloatPoint operator/ (const FloatPoint &a, const float &b){
  FloatPoint c(a);
  for(int i=0;i<3;i++)
    c[i]/=b;
  return c;
};
// prodiut scalaire
inline	myfloat operator % (const FloatPoint &a, const FloatPoint &b){
  return a[0]*b[0] + a[1]*b[1] +a[2]*b[2];
}

// produit vecoriel
inline	FloatPoint operator * (const FloatPoint &a, const FloatPoint &b){
  
  return FloatPoint(a[1]*b[2]-a[2]*b[1],
		     a[2]*b[0]-a[0]*b[2],
		     a[0]*b[1]-a[1]*b[0]);

}

#endif
