#ifndef _MATRIX4X4F_H_
#define _MATRIX4X4F_H_

#include "vector3f.h"

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))
#define getIndex(i,j)	 ((i) + (4*(j)))

class matrix4x4f {

	public:

		float m[16];

		matrix4x4f();

		matrix4x4f( float m0, float m4, float  m8, float m12,
		            float m1, float m5, float  m9, float m13,
		            float m2, float m6, float m10, float m14,
		            float m3, float m7, float m11, float m15 );

		void identity();

		void rotate(const float angle, vector3f axis);

		void transformVector(vector3f *vec);
		void transform(vector4f *v);
		
		//operators

};

#endif
