#ifndef _CAMERA_
#define _CAMERA_

#include "V3.h"


class Camera {


	public:
		V3				mDir;			// The direction we're looking
		V3				mUpDir;			// What direction is up
		V3				mPos;			// Where we are in space
		float			mXYScale;		// Scales all the XY cords in he cam cord system
		float			mPersZ;			// The dist added to the cam's z 


		void			CalcTransMatrix( R3Matrix& outT ) const;
};

#endif



