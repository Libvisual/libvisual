#include "V3.h"
#include "R3Matrix.h"
#include "Plane.h"

void V3::normalize() {
	PFloat	d;
	
	d = 1 / magnitude();
	mX *= d;
	mY *= d;
	mZ *= d;
}




void V3::transform( const R3Matrix& inMatrix ) {
	PFloat y = mY, x = mX;
	
	mX = inMatrix.mM[0] * x + inMatrix.mM[1] * y + inMatrix.mM[2] * mZ; 
	mY = inMatrix.mM[3] * x + inMatrix.mM[4] * y + inMatrix.mM[5] * mZ; 
	mZ = inMatrix.mM[6] * x + inMatrix.mM[7] * y + inMatrix.mM[8] * mZ; 
}



void V3::transform( const R3Matrix& inMatrix, float inPerspectiveZ ) {
	PFloat y = mY, x = mX;
	PFloat xt, yt;
	
	xt = inMatrix.mM[0] * x + inMatrix.mM[1] * y + inMatrix.mM[2] * mZ; 
	yt = inMatrix.mM[3] * x + inMatrix.mM[4] * y + inMatrix.mM[5] * mZ; 
	mZ = inMatrix.mM[6] * x + inMatrix.mM[7] * y + inMatrix.mM[8] * mZ; 
	
	// Catch float div by zeros...
	x = mZ + inPerspectiveZ;
	mX = xt / x;
	mY = yt / x;
}




void V3::transform( const R3Matrix& inMatrix, const V3& inPt ) {
	PFloat y = inPt.mY, x = inPt.mX, z = inPt.mZ;
	
	mX = inMatrix.mM[0] * x + inMatrix.mM[1] * y + inMatrix.mM[2] * z; 
	mY = inMatrix.mM[3] * x + inMatrix.mM[4] * y + inMatrix.mM[5] * z; 
	mZ = inMatrix.mM[6] * x + inMatrix.mM[7] * y + inMatrix.mM[8] * z; 
}



void V3::cross( const V3& inV ) {
	PFloat x = mX, y = mY;
	
	mX = inV.mZ * y - mZ * inV.mY;
	mY = mZ * inV.mX - inV.mZ * x;
	mZ = x * inV.mY - inV.mX * y;
}




void V3::cross( const V3& inA, const V3& inB ) {
	
	mX = inA.mZ * inB.mY - inB.mZ * inA.mY;
	mY = inB.mZ * inA.mX - inA.mZ * inB.mX;
	mZ = inB.mX * inA.mY - inA.mX * inB.mY;
}


void V3::rotate( const V3& inPt1, const V3& inPt2, PFloat inAng ) {
	PFloat x, s, c;
	V3 line; 
	
	line.set( inPt1 );
	line.subtract( inPt2 );
	subtract( inPt1 );
	toPlane( line );
	s = sin( inAng );
	c = cos( inAng );
	x = mX;
	mX = x * c - mY * s;
	mY = x * s + mY * c;
	fromPlane( line );
	add( inPt1 );
}

#define TOO_BIG	1.0e20


bool V3::intersection( const Plane& inPlane, const V3& inLine, const V3& inPt ) {

	PFloat t = ( inPlane.mD - inPlane.dot( inPt ) ) / inPlane.dot( inLine );
	set( inLine );
	scale( t );
	add( inPt );
	
	return t > - TOO_BIG && t < TOO_BIG;
}





#define A	inNormal.mX
#define B	inNormal.mY
#define C	inNormal.mZ

void V3::toPlane( const V3& inNormal ) {
	PFloat	BC		= sqrt( B*B + C*C );
	PFloat	ABC		= inNormal.magnitude();
	PFloat	x = mX, y = mY;
	
	if ( BC > 0.0001 ) {
		mX = x * BC / ABC - A * ( B*y + C*mZ ) / (ABC * BC);
		mY = (C*y - B*mZ) / BC;
		mZ = (A*x + B*y + C*mZ) / ABC; }
	else { 
		mX = mZ;
		mZ = - x;
	}
		
}




void V3::fromPlane( const V3& inNormal ) {
	PFloat	BC		= sqrt( B*B + C*C );
	PFloat	ABC		= inNormal.magnitude();
	PFloat	x = mX, y = mY;
	
	if ( BC > 0.0001 ) {
		mX = (x * BC + A * mZ ) / ABC;
		mY = C * y / BC - A * B * x / ( BC*ABC ) + B * mZ / ABC; 
		mZ = - B * y / BC - A * C * x / ( BC*ABC ) + C * mZ / ABC; } 
	else {
		mX = - mZ;
		mZ = x; 
	}
}

