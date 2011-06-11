#ifndef _R3MATRIX_
#define _R3MATRIX_

#include "V3.h"


class R3Matrix {


		
	public:
		
		void				setRow( int inRow, PFloat in1, PFloat in2, PFloat in3 );
		
		
		void				transform( R3Matrix& inMatrix );
		
		
		void				scale( PFloat inScale );


		PFloat				mM[ 9 ];

};

#endif


