#ifndef _GVECTOR3F_H
#define _GVECTOR3F_H
#include "zData.h"
class gVector3f {
	
public :
		float x;
		float y;
		float z;

		gVector3f();
		
		gVector3f(float xx, float yy, float zz);
		
		gVector3f(XYZ& xyz);
		
		gVector3f(XYZ& end, XYZ& start);
		
		gVector3f operator+(const gVector3f& other) const
		{
			return gVector3f( other.x + x, other.y + y, other.z + z );
		}
		
		gVector3f operator+=(const gVector3f& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return gVector3f( other.x + x, other.y + y, other.z + z );
		}
		
		gVector3f operator-(const gVector3f& other) const
		{
			return gVector3f( x - other.x, y - other.y, z - other.z );
		}
		
		
		gVector3f operator*(float num) const
		{
			return gVector3f(x * num, y * num, z * num);
				
		}
				
		gVector3f operator/(float num) const
		{
			return gVector3f(x / num, y / num, z / num);
		}

		float operator*(const gVector3f& other) const
		{
			return (float)(x * other.x + y * other.y + z * other.z);
		}
		
		gVector3f operator^(const gVector3f& other) const
		{
			gVector3f normal;	
			// Calculate the cross product with the non communitive equation
			normal.x = y * other.z - z * other.y;
			normal.y = z * other.x - x * other.z;
			normal.z = x * other.y - y * other.x;

			return normal;
		}
		
		gVector3f operator*(float* mat) const
		{
		
			float bx = x*mat[0] + y*mat[4] + z*mat[8] + mat[12];
			float by = x*mat[1] + y*mat[5] + z*mat[9] + mat[13];
			float bz = x*mat[2] + y*mat[6] + z*mat[10] + mat[14];
					
			return gVector3f(bx, by, bz);
		}
		
		gVector3f transformAsNormal(float* mat);

		float dot(gVector3f other);
		
		gVector3f cross(gVector3f other);
		
		float length();

		gVector3f normal();
		
		void normalize();
		
		gVector3f reverse();
		
		void copyXYZ(XYZ& t) { t.x=x; t.y=y; t.z=z; }
		
		XYZ getXYZ() { XYZ o; o.x=x; o.y=y; o.z=z; return o;}
		
		gVector3f refract(gVector3f& N, float eta);
		
};


#endif
