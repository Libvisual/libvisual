#include <cmath>
class matrix
{
	public:
	double v[3][3];
	 
	void ApplyMatrix(FloatPoint &p,FloatPoint &C)
	{
	  FloatPoint P(p);
		p[0]=(P[2]-C[2])*v[0][2]+(P[1]-C[1])*v[0][1]+(P[0]-C[0])*v[0][0]+C[0];
		p[1]=(P[2]-C[2])*v[1][2]+(P[1]-C[1])*v[1][1]+(P[0]-C[0])*v[1][0]+C[1];
		p[2]=(P[2]-C[2])*v[2][2]+(P[1]-C[1])*v[2][1]+(P[0]-C[0])*v[2][0]+C[2];

	}
	matrix(double angle, FloatPoint axe)
	{
		#define a axe[0]
		#define b axe[1]
		#define c axe[2]
		double 	aa=a*a,
			bb=b*b,
			cc=c*c,
			cosa=cos(angle),
			sina=sin(angle),
			abc=aa+bb+cc,
			rabc=fastsqrt(abc);
				
		v[0][0]=((bb+cc)*cosa + aa)/abc;
		v[0][1]=-((c*rabc*sina+(b*a*(cosa-1.))))/abc;
		v[0][2]=(b*rabc*sina-(c*a*(cosa-1.)))/abc;

		v[1][0]=((c*rabc*sina+(b*a*(cosa-1.))))/abc;
		v[1][1]=((aa+cc)*cosa + bb)/abc;
		v[1][2]=-(a*rabc*sina-(c*b*(cosa-1.)))/abc;

		v[2][0]=-((b*rabc*sina+(c*a*(cosa-1.))))/abc;
		v[2][1]=(a*rabc*sina-(c*b*(cosa-1.)))/abc; 
		v[2][2]=((bb+aa)*cosa + cc)/abc;
		
		#undef a
		#undef b
		#undef c
	}

	matrix(){};
};

