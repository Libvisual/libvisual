

#include "R3Matrix.h"



void R3Matrix::setRow( int inRow, PFloat in1, PFloat in2, PFloat in3 ) {
	int off = (inRow-1) * 3;
	
	mM[ off ]	= in1;
	mM[ off+1 ]	= in2;
	mM[ off+2 ] = in3;
}
		
		
void R3Matrix::transform( R3Matrix& inM ) {
	PFloat m[9];
	int i;
	
	for ( i = 0; i < 9; i++ )
		m[i] = mM[i];
		
	mM[0] = inM.mM[0] * m[0] + inM.mM[1] * m[3] + inM.mM[2] * m[6];
	mM[1] = inM.mM[0] * m[1] + inM.mM[1] * m[4] + inM.mM[2] * m[7];
	mM[2] = inM.mM[0] * m[2] + inM.mM[1] * m[5] + inM.mM[2] * m[8];
	
	mM[3] = inM.mM[3] * m[0] + inM.mM[4] * m[3] + inM.mM[5] * m[6];
	mM[4] = inM.mM[3] * m[1] + inM.mM[4] * m[4] + inM.mM[5] * m[7];
	mM[5] = inM.mM[3] * m[2] + inM.mM[4] * m[5] + inM.mM[5] * m[8];
	
	mM[6] = inM.mM[6] * m[0] + inM.mM[7] * m[3] + inM.mM[8] * m[6];
	mM[7] = inM.mM[6] * m[1] + inM.mM[7] * m[4] + inM.mM[8] * m[7];
	mM[8] = inM.mM[6] * m[2] + inM.mM[7] * m[5] + inM.mM[8] * m[8];
}


void R3Matrix::scale( PFloat inScale ) {

	mM[0] *= inScale;
	mM[1] *= inScale;
	mM[2] *= inScale;
	mM[3] *= inScale;
	mM[4] *= inScale;
	mM[5] *= inScale;
	mM[6] *= inScale;
	mM[7] *= inScale;
	mM[8] *= inScale;
}
