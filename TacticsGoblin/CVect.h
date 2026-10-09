#ifndef _CVECT_H_
#define _CVECT_H_

#include <memory.h>
#include <math.h>

class CVect
{
public:
	union
	{
		float v[3]; // x, y, z
		struct { float x, y, z; };
	};

							CVect()															{ memset(v,0,sizeof(v)); }
							CVect( const CVect& u)											{ memcpy(v, u.v, sizeof(v)); }
							CVect( const float &x, const float &y, const float &z )	{ v[0] = x; v[1] = y; v[2] = z; }
							
							CVect( const CVect &v1, const CVect &v2, const float &t )
							{
							   v[0] = ( v2.v[0] - v1.v[0] ) * t + v1.v[0];
							   v[1] = ( v2.v[1] - v1.v[1] ) * t + v1.v[1];
							   v[2] = ( v2.v[2] - v1.v[2] ) * t + v1.v[2];
							}
							
							CVect( const CVect &v1, const CVect &v2 ) // v1 = starting point; v2 = ending point
							{
							   v[0] = v2.v[0] - v1.v[0];
							   v[1] = v2.v[1] - v1.v[1];
							   v[2] = v2.v[2] - v1.v[2];
							}

	inline	void			zero()												{ memset(v,0,sizeof(v)); }
	inline 	void			normalize()											{ (*this) /= length(); }
	inline 	float			length() const										{ return (float)sqrt(length2()); }
	inline 	float			length2() const										{ return v[0]*v[0] + v[1]*v[1] + v[2]*v[2]; }


	inline 	CVect& 			operator  = ( const CVect &u )						{ v[0] = u.v[0]; v[1] = u.v[1]; v[2] = u.v[2];	return *this; }
	inline 	CVect& 			operator += ( const CVect &u )						{ v[0] += u[0]; v[1] += u[1]; v[2] += u[2];		return *this; }
	inline 	CVect& 			operator -= ( const CVect &u )						{ v[0] -= u[0]; v[1] -= u[1]; v[2] -= u[2];		return *this; }
	inline 	CVect& 			operator *= ( const float &d )					{ v[0] *= d; v[1] *= d; v[2] *= d;				return *this; }
	inline 	CVect& 			operator /= ( const float &d )					{ v[0] /= d; v[1] /= d; v[2] /= d;				return *this; }

	inline 	CVect 			operator  - () const								{ return CVect( -v[0], -v[1], -v[2] ); }

	friend inline CVect 	operator  + ( const CVect& a, const CVect& b )		{ return CVect( a[0] + b[0], a[1] + b[1], a[2] + b[2] ); }
	friend inline CVect 	operator  - ( const CVect& a, const CVect& b )		{ return CVect( a[0] - b[0], a[1] - b[1], a[2] - b[2] ); }
	friend inline CVect 	operator  * ( const CVect& a, const float &d )	{ return CVect(  a[0] * d, a[1] * d, a[2] * d ); }
	friend inline CVect 	operator  * ( const float &d, const CVect& a )	{ return a*d; }
	friend inline float	operator  * ( const CVect& a, const CVect& b )		{ return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }  // produit scalaire
	friend inline CVect 	operator  / ( const CVect& a, const float &d )	{ return a * (1/d); }
	friend inline CVect 	operator  ^ ( const CVect& a, const CVect& b )		{ return CVect ( a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0] ); } // produit vectoriel
		 	
	friend inline bool 	operator == ( const CVect& a, const CVect& b )		{ return ( (a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2])); }
	friend inline bool 	operator != ( const CVect& a, const CVect& b )		{ return !(a == b); }

	inline 	float			operator[]( const int i )  const					{ return v[i];  }
	inline 	float&		operator[]( const int i )							{ return v[i];  }

	static 	float			angleXBetween(const CVect& a , const CVect& b);
	static 	float			angleYBetween(const CVect& a , const CVect& b);
	static 	float			angleZBetween(const CVect& a , const CVect& b);
	static 	CVect			getPointOnPlane(const CVect& A, const CVect& R, const CVect& N, const float &D);

			float			dotProduct( const CVect& _v ) const;
			float			getLength() const ;
	
			float			getAngleBetween( const CVect& _v1 ) const;
			float			getSquaredLength() const ;

	//this function makes no sense, its sole purpose is to allow the use of std::sort on a vector of CVect
	friend inline bool 	operator < (const CVect& a, const CVect& b) { return a.v[0] == b.v[0] ? a.v[1] < b.v[1] : a.v[0] < b.v[0]; }
};



#endif
