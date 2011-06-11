#ifndef _V3_
#define _V3_

#define PFloat float
#define PI	 3.141592653589793

#include <math.h>
class R3Matrix;
class Plane;


class XYCord {

	public:
		long mX, mY;
};


class V3 {

	public:
		inline	PFloat		magnitudeSqr() const						{ return mX * mX + mY * mY + mZ * mZ;			}
		inline	PFloat		magnitude()	const							{ return sqrt( mX * mX + mY * mY + mZ * mZ);	}
		
		inline	void		set( const V3& inPt )						{ *this = inPt;	}
		inline	void		set( PFloat inX, PFloat inY, PFloat inZ )	{ mX = inX; mY = inY; mZ = inZ;	}
		
		inline void			scale( PFloat inS, const V3& inPt )			{ mX = inS*inPt.mX; mY = inS*inPt.mY; mZ = inS*inPt.mZ;	}
		inline void			scale( PFloat inS )							{ mX *= inS; mY *= inS; mZ *= inS;				}
		void				normalize();
		
		
		inline PFloat		dot( const V3& inV ) const					{ return mX*inV.mX + mY*inV.mY + mZ*inV.mZ;		}
		
		void				transform( const R3Matrix& inMatrix );
		void				transform( const R3Matrix& inMatrix, const V3& inPt );
		
		// Transforms the vector like the above two, but then applies a perspective transformation on the x and y coords
		void				transform( const R3Matrix& inMatrix, float inPersZ );

		inline void			applyPerspective()							{ mX /= mZ; mY /= mZ;	}
		
		
		inline void			subtract( const V3& inA )					{ mX -= inA.mX; mY -= inA.mY; mZ -= inA.mZ;						} 
		inline void			subtract( const V3& inA, const V3& inB )	{ mX = inA.mX-inB.mX; mY = inA.mY-inB.mY; mZ = inA.mZ-inB.mZ;						} 

		
		inline void			add( PFloat inX, PFloat inY, PFloat inZ )	{ mX += inX; mY += inY; mZ += inZ;				} 
		inline void			add( const V3& inA, const V3& inB )			{ mX = inA.mX+inB.mX; mY = inA.mY+inB.mY; mZ = inA.mZ+inB.mZ;						} 
		inline void			add( const V3& inA )						{ mX += inA.mX; mY += inA.mY; mZ += inA.mZ;						} 


		void				toPlane( const V3& inNormal );
		void				fromPlane( const V3& inNormal );
		
		// Assigns intersection of a plane with a line to this pt
		bool				intersection( const Plane& inPlane, const V3& inLine, const V3& inPt );

		// Rotates this pt around the line defined by inPt1 and inPt2
		void				rotate( const V3& inPt1, const V3& inPt2, PFloat inAng );
		
		void				cross( const V3& inA );
		void				cross( const V3& inA, const V3& inB );
		
		PFloat	mX, mY, mZ;		
		
};

#endif
