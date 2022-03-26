#ifndef MATH_H
#define MATH_H

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

#define MATH_PI 3.1415926535897932384626433832795

namespace math
{

    typedef vec<2, float> Vec2;
    typedef vec<3, float> Vec3;
    typedef vec<4, float> Vec4;
    typedef quat<float>   Quaternion;
    typedef mat<float>    Matrix4x4;

    typedef vec<2, double> Vec2d;
    typedef vec<3, double> Vec3d;
    typedef vec<4, double> Vec4d;
    typedef quat<double>   Quaterniond;
    typedef mat<double>    Matrix4x4d;

    template<unsigned int L, typename T>
    T const* value_ptr(const vec<L, T>& v)
    {
        return &(v.x);
    }

	template<typename T>
    inline T ToRadians(T x)
    {
        return x * MATH_PI / 180.0;
    }
	template<typename T>
    inline T ToDegrees(T x)
    {
        return x * 180.0 / MATH_PI;
    }

    template<typename  T>
    vec<3,T> rotate(const vec<3,T>& v, const quat<T>& q)
    {
        quat<T> result = q * v * inverse(q);
        return vec<3, T>(result.x, result.y, result.z);
    }

    template<typename T>
    T max(T v1, T v2)
    {
        if (v1 > v2)
            return v1;
        else
            return v2;
    }

    template<typename T>
    T min(T v1, T v2)
    {
        if (v1 < v2)
            return v1;
        else
            return v2;
    }

	template<typename T>
	T clamp(T v, T min, T max)
	{
		if (v < min)
			return min;
		if (v > max)
			return max;
		return v;
	}

}
#endif
