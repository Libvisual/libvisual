
#include "Plane.h"




void Plane::set( PFloat inX, PFloat inY, PFloat inZ, PFloat inD ) {
	PFloat m = 1 / ( inX*inX + inY*inY + inZ*inZ );
	
	//if ( inD < 0 )
	//	m = -m;
		
	mX = inX * m;
	mY = inY * m;
	mZ = inZ * m;
	mD = inD * m;
}



void Plane::set( const V3& inPt1, const V3& inPt2, const V3& inPt3 ) {
	PFloat m, v1x, v1y, v1z, v2x, v2y, v2z, x, y, z;

	// v1.subtract( inPt1, inPt2 );
	// v1.subtract( inPt3, inPt2 );
	v1x = inPt1.mX-inPt2.mX; v1y = inPt1.mY-inPt2.mY; v1z = inPt1.mZ-inPt2.mZ; 
	v2x = inPt3.mX-inPt2.mX; v2y = inPt3.mY-inPt2.mY; v2z = inPt3.mZ-inPt2.mZ; 
	
	// r.cross( v1, v2 );
	x = v1z * v2y - v2z * v1y;
	y = v2z * v1x - v1z * v2x;
	z = v2x * v1y - v1x * v2y;

	// r.normalize();
	m = 1 / sqrt( x * x + y * y + z * z );
	x *= m;
	y *= m;
	z *= m;
	
	// this <- r
	mX = x;
	mY = y;
	mZ = z;
	
	// r.dot( inPt1 )
	mD = inPt1.mX * x + inPt1.mY * y + inPt1.mZ * z;
}


void Plane::set( const V3& inNormal, PFloat inD ) {
	PFloat m = 1 / inNormal.magnitude();
	
	//if ( inD < 0 )
	//	m = -m;
		
	mX = m * inNormal.mX;
	mY = m * inNormal.mY;
	mZ = m * inNormal.mZ;
	mD = inD;
}



void Plane::intersect( const Plane& inPlane, V3& outLine, V3& outPt ) {
	V3		l1, l2;
	PFloat	A1, A2, B1, B2, C1, C2, det;
	
	// The line is the cross product of the two normals to each plane
	outLine.set( inPlane );
	outLine.cross( *this );
	
	// Conv plane normals to local plane cords
	l1.set( *this );
	l2.set( inPlane );
	
	l1.toPlane( outLine );
	l2.toPlane( outLine );
	
	
	// Get eqns of lines formed by local plane and each plane
	A1 = l1.mX;
	B1 = l1.mY;
	C1 = mD * sqrt( A1*A1 + B1*B1 );
	A2 = l2.mX;
	B2 = l2.mY;
	C2 = inPlane.mD * sqrt( A2*A2 + B2*B2 );
	
	// Compute local intersection of two planes
	det = B1*A2 - B2*A1;
	outPt.mX = ( C2*B1 - C1*B2 ) / det;
	outPt.mY = ( C1*A2 - C2*A1 ) / det;
	outPt.mZ = outPt.mX * A1 + outPt.mY * B1 - C1;
	outPt.mZ = outPt.mX * A2 + outPt.mY * B2 - C2;
	outPt.mZ = 0;
	
	// Local to global
	outPt.fromPlane( outLine );
}




