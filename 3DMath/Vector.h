#ifndef VECTOR_H
#define VECTOR_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <string>

namespace math{


    template<unsigned int L, typename T>
    vec<L, T> operator*(const vec<L,T>& v, T scalar)
    {
        vec<L, T> temp(scalar);
        return v * temp;
    }
    template<unsigned int L, typename T>
    vec<L, T> operator*(T scalar, const vec<L,T>& v)
    {
        vec<L, T> temp(scalar);
        return v * temp;
    }

    template<unsigned int L, typename T>
    vec<L, T> operator/(const vec<L,T>& v, T scalar)
    {
        return v * 1 / scalar;
    }
    template<unsigned int L, typename T>
    vec<L, T> operator/(T scalar, const vec<L,T>& v)
    {
        vec<L, T> temp(scalar);
        return v * 1 / scalar;
    }

	template<unsigned int L, typename T>
    vec<L, T> operator-(const vec<L,T>& v)
    {
		return static_cast<T>(-1) * v;
    }

	template<typename T>
    T componentSum(const vec<2,T>& v)
    {
        return v.x + v.y;
    }
    template<typename T>
    T componentSum(const vec<3, T>& v)
    {
        return v.x + v.y + v.z;
    }
    template<typename T>
    T componentSum(const vec<4, T>& v)
    {
        return v.x + v.y + v.z + v.w;
    }

     template<unsigned int L, typename T>
    T dot(const vec<L, T>& v1, const vec<L, T>& v2)
    {
        vec<L, T> resultVector = v1 * v2;
        return componentSum(resultVector);
    }

    template<unsigned int L, typename T>
    T lengthSq(const vec<L, T>& v)
    {
        return dot(v, v);
    }

    template<unsigned int L, typename T>
    T length(const vec<L, T>& v)
    {
        return sqrt(dot(v, v));
    }

    template<unsigned int L, typename T>
    T distanceSq(const vec<L, T>& p1, const vec<L, T>& p2)
    {
        return lengthSq(p1 - p2);
    }

    template<unsigned int L, typename T>
    T distance(const vec<L, T>& p1, const vec<L, T>& p2)
    {
        return length(p1 - p2);
    }

    template<typename T>
    vec<3, T> cross(const vec<3, T>& v1, const vec<3, T>& v2)
    {
        vec<3, T> result;
        T x = v1.y * v2.z - v1.z * v2.y;
        T y = v1.z * v2.x - v1.x * v2.z;
        T z = v1.x * v2.y - v1.y * v2.x;
        return vec<3, T>(x, y, z);
    }

    template<unsigned int L,typename T>
    vec<L,T> normalize(const vec<L,T>& v)
    {
        return v / length(v);
    }

    template<typename T>
    std::string ToString(const vec<2, T>& v)
    {
    	std::string result = "Vector2 {" + std::to_string(v.x) + "; " + std::to_string(v.y) + "}";
    	return result;
    }
    template<typename T>
    std::string ToString(const vec<3, T>& v)
    {
    	std::string result = "Vector3 {" + std::to_string(v.x) + "; " + std::to_string(v.y) + "; " + std::to_string(v.z) +"}";
    	return result;
    }
    template<typename T>
    std::string ToString(const vec<4, T>& v)
    {
    	std::string result = "Vector4 {" + std::to_string(v.x) + "; " + std::to_string(v.y) + "; " + std::to_string(v.z) + "; " + std::to_string(v.w) +"}";
    	return result;
    }

    template<unsigned int L, typename T>
    bool operator<(const vec<L, T>& v1, const vec<L, T>& v2)
    {
        return lengthSq(v1) < lengthSq(v2);
    }
    template<unsigned int L, typename T>
    bool operator>(const vec<L, T>& v1, const vec<L, T>& v2)
    {
        return v2 < v1;
    }

    template<typename T>
    bool operator==(const vec<2, T>& v1, const vec<2, T>& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y);
    }
    template<typename T>
    bool operator==(const vec<3, T>& v1, const vec<3, T>& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
    }
    template<typename T>
    bool operator==(const vec<4, T>& v1, const vec<4, T>& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
    }
}
#endif
