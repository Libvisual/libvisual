#ifndef _PLANE_
#define _PLANE_

#include "V3.h"


class Plane : public V3 {

	public:	
		//	Returns 1 if inPt is on same side of this plane as infinity
		//	Returns 0 if inPt is on this plane
		//	Returns -1 if inPt is on the same side as (0,0,0)
		//int					OnPlane( const Vector& inPt );
		
		//	Returns left hand side of equation.  ie, the closer it is to zero, the closer
		//	inPt is to being on this plane.  It's:  (this.Dot( inPt ) - mD)
		PFloat				CheckZero( const V3& inPt ) const;
		
		//	Assigns this plane so that all three pts lie on this plane
		void				set( const V3& inPt1, const V3& inPt2, const V3& inPt3 );
		
		//	Assigns this plane with a normal parellel to inNormal and so that it passes thru the pt: inD * unit(inNormal)
		void				set( const V3& inNormal, PFloat inD );
		void				set( PFloat inX, PFloat inY, PFloat inZ, PFloat inD );

		// Find intersecting line of this plane and another plane
		void				intersect( const Plane& inPl, V3& outDir, V3& outPos );



		// Eq: mX x + mY y + mZ z = mD
		// Convention: mD is *always*  positive
		PFloat	mD;
};

#endif
