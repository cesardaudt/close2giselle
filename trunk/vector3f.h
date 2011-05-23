//-----------------------------------------------------------------------------
//           Name: vector3f.h
//         Author: Kevin Harris
//  Last Modified: 02/01/05
//    Description: OpenGL compatible utility class for a 3D vector of floats
//                 NOTE: This class has been left unoptimized for readability.
//-----------------------------------------------------------------------------

#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <cmath>

#define DEG_RAD(theta) (theta*M_PI)/180
#define maxabs(x,y) ((abs(x)>abs(y)) ? (x) : (y))

class vector3f
{
	public:

		float x;
		float y;
		float z;

		vector3f();
		vector3f(float x_, float y_, float z_);

		void set(float x_, float y_, float z_);
		float length();
		void normalize();

		// Static utility methods
		static float distance(const vector3f &v1, const vector3f &v2);
		static float dotProduct(const vector3f &v1, const vector3f &v2);
		static vector3f crossProduct(const vector3f &v1, const vector3f &v2);

		// Operators...
		vector3f operator + (const vector3f &other);
		vector3f operator - (const vector3f &other);
		vector3f operator * (const vector3f &other);
		vector3f operator / (const vector3f &other);

		vector3f operator * (const float scalar);
		friend vector3f operator * (const float scalar, const vector3f &other);
		
		vector3f& operator = (const vector3f &other);
		vector3f& operator += (const vector3f &other);
		vector3f& operator -= (const vector3f &other);

		vector3f operator + (void) const;
		vector3f operator - (void) const;
};

//Vector in Homogeneous Coordinates(4D)
typedef struct vec4d {
	vector3f vec;
	float	 w;
} vector4f;

#endif // _VECTOR3F_H_
