#include "Camera.h"

#include "R3Matrix.h"

#define A	mDir.mX
#define B	mDir.mY
#define C	mDir.mZ



void Camera::CalcTransMatrix( R3Matrix& outT ) const {
	V3	v;
	R3Matrix Rot;
	float	cosa, sina;
	float	BC	= sqrt( B*B + C*C );
	float	ABC	= mDir.magnitude();
	
	if ( BC > 0.000001 ) {
		outT.setRow( 1, BC/ABC, - A * B / (BC*ABC), - A * C / (BC*ABC) );
		outT.setRow( 2, 0, C / BC, - B / BC );
		outT.setRow( 3, A / ABC, B / ABC, C / ABC ); }
	else {
		outT.setRow( 1, 0, -1, -1 );
		outT.setRow( 2, 0, 1, -1 );
		outT.setRow( 3, 1, 0, 0 );
	}
	
	// Make the camera always orient "up" in the mUpDir direction...
	v.transform( outT, mUpDir );		
	v.mZ = 0;
	v.normalize();

	// Catch the case where our up dir is exactly normal to the view plane
	if ( v.magnitude() >= 0.000001 ) { 
		cosa = v.mY / v.magnitude();
		sina = v.mX / v.magnitude();
		Rot.setRow( 1, cosa, - sina, 0 );
		Rot.setRow( 2, sina, cosa, 0 );
		Rot.setRow( 3, 0, 0, 1 );
		outT.transform( Rot );
	}
	
	// Scale all the coeffs that determine the x and y cords
	for ( int i = 0; i < 6; i++ ) 
		outT.mM[ i ] *= mXYScale;
}