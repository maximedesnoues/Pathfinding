#include "CVect.h"

// return angle between 2 vectors on X axis
// in radian
float CVect::angleXBetween(const CVect& u , const CVect& v )
{
	
   // calculer c, s : cosinus et sinus de (u,v)
	float c = u*v / (u.length() * v.length());
	float s = u[1]*v[2]- v[1]*u[2];//u^v / (u.length() * v.length());

	if (s>0) {
      return acosf(c);
	}
    else return -acosf(c);
}

// return angle between 2 vectors on Y axis
// in radian
float CVect::angleYBetween(const CVect& u , const CVect& v )
{
	
   // calculer c, s : cosinus et sinus de (u,v)
	float c = u*v / (u.length() * v.length());
	float s = u[0]*v[2]- v[0]*u[2];//u^v / (u.length() * v.length());

	if (s>0) {
      return acosf(c);
	}
    else return -acosf(c);
}

// return angle between 2 vectors on Z axis
// in radian
float CVect::angleZBetween(const CVect& u , const CVect& v )
{
	
   // calculer c, s : cosinus et sinus de (u,v)
	float c = u*v / (u.length() * v.length());
	float s = u[0]*v[1]- v[0]*u[1];//u^v / (u.length() * v.length());

	if (s>0) {
      return acosf(c);
	}
    else return -acosf(c);
}


// A1 : a point on your ray
// A2 : the direction vector your ray
// N : the normal to the plane
// D : the distance of the plane from the origin
CVect CVect::getPointOnPlane(const CVect& A1, const CVect& A2, const CVect& N, const float &D)
{
	float Vd = N*A2;
	if(Vd == 0) return CVect(0,0,0); // plane and ray are parallel
	float V0 = -(N*A1 + D);
	float t = V0/Vd;
	if(t < 0) return CVect(0,0,0); // ray intersects plane behind origin

	return CVect(A1[0]+A2[0]*t,A1[1]+A2[1]*t,A1[2]+A2[2]*t);
}


float CVect::dotProduct( const CVect& _v )  const					
{ 
	return (v[0]*_v[0]) + (v[1]*_v[1]); 
}

float CVect::getLength() 	const								
{ 
	return sqrtf((v[0]*v[0]) + (v[1]*v[1])); 
}

float CVect::getAngleBetween( const CVect& _v1 ) const
{
	return acosf(dotProduct(_v1) / (getLength()*_v1.getLength()));
}

float CVect::getSquaredLength() const
{ 
	return ((v[0]*v[0]) + (v[1]*v[1])); 
}
