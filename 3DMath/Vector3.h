#ifndef VECTOR3_H
#define VECTOR3_H

namespace math
{
    template<unsigned int L, typename T> struct vec;

    template<typename T>
    struct vec<3, T>
    {
        vec() {}

        vec(T _s)
        {
            x = _s;
            y = _s;
            z = _s;
        }

        vec(const vec<2,T>& v, T _z)
        {
            x = v.x;
            y = v.y;
            z = _z;
        }

        vec(const vec<4,T>& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        vec(T _x, T _y, T _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }
        union
        {
            struct { T x, y, z; };
            struct { T r, g, b; };
        };
        template<typename U>
        vec<3, T>&  operator+=(const vec<3, U>& v);
        template<typename U>
        vec<3, T>&  operator-=(const vec<3, U>& v);
        template<typename U>
        vec<3, T>&  operator*=(const vec<3, U>& v);
    };

    template<typename T>
    template<typename U>
    vec<3, T>& vec<3, T>::operator+=(const vec<3, U>& v)
    {
        this->x += static_cast<T>(v.x);
        this->y += static_cast<T>(v.y);
        this->z += static_cast<T>(v.z);
        return *this;
    }

    template<typename T>
    template<typename U>
    vec<3, T>& vec<3, T>::operator-=(const vec<3, U>& v)
    {
        this->x -= static_cast<T>(v.x);
        this->y -= static_cast<T>(v.y);
        this->z -= static_cast<T>(v.z);
        return *this;
    }

    template<typename T>
    template<typename U>
    vec<3, T>& vec<3, T>::operator*=(const vec<3, U>& v)
    {
        this->x *= static_cast<T>(v.x);
        this->y *= static_cast<T>(v.y);
        this->z *= static_cast<T>(v.z);
        return *this;
    }

    template<typename T>
    vec<3, T> operator+(const vec<3, T>& v1, const vec<3, T>& v2)
    {
        return vec<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    template<typename T>
    vec<3, T> operator-(const vec<3, T>& v1, const vec<3, T>& v2)
    {
        return vec<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    template<typename T>
    vec<3, T> operator*(const vec<3, T>& v1, const vec<3, T>& v2)
    {
        return vec<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }
    template<typename T>
    vec<3, T> operator/(const vec<3, T>& v1, T scalar)
    {
        return vec<3, T>(v1.x / scalar, v1.y / scalar, v1.z / scalar);
    }
    
    template<typename T>
    T compMax(const vec<3, T>& v)
    {
        return max(v.x, max(v.y, v.z));
    }

}
#endif
