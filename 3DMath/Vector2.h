#ifndef VECTOR2_H
#define VECTOR2_H

namespace math{
    template<unsigned int L, typename T> struct vec;

    template<typename T>
    struct vec<2, T>
    {
        vec() {}

        vec(T _s)
        {
            x = _s;
            y = _s;
        }

        vec(const vec<3,T>& v)
        {
            x = v.x;
            y = v.y;
        }

        vec(T _x, T _y)
        {
            x = _x;
            y = _y;
        }
        T x, y;
        template<typename U>
        vec<2, T>&  operator+=(const vec<2, U>& v);
        template<typename U>
        vec<2, T>&  operator-=(const vec<2, U>& v);
        template<typename U>
        vec<2, T>&  operator*=(const vec<2, U>& v);

		T operator[](int i) const
		{
			assert(i >= 0 && i < 2);
			switch(i)
			{
				case 0: return x;
				case 1: return y;
			}
		}
		T& operator[](int i)
		{
			assert(i >= 0 && i < 2);
			switch(i)
			{
				case 0: return x;
				case 1: return y;
			}
		}
    };


    template<typename T>
    template<typename U>
    vec<2, T>& vec<2, T>::operator+=(const vec<2, U>& v)
    {
        this->x += static_cast<T>(v.x);
        this->y += static_cast<T>(v.y);
        return *this;
    }

    template<typename T>
    template<typename U>
    vec<2, T>& vec<2, T>::operator-=(const vec<2, U>& v)
    {
        this->x -= static_cast<T>(v.x);
        this->y -= static_cast<T>(v.y);
        return *this;
    }

    template<typename T>
    template<typename U>
    vec<2, T>& vec<2, T>::operator*=(const vec<2, U>& v)
    {
        this->x *= static_cast<T>(v.x);
        this->y *= static_cast<T>(v.y);
        return *this;
    }

    template<typename T>
    vec<2, T> operator+(const vec<2, T>& v1, const vec<2, T>& v2)
    {
        return vec<2, T>(v1.x + v2.x, v1.y + v2.y);
    }

    template<typename T>
    vec<2, T> operator-(const vec<2, T>& v1, const vec<2, T>& v2)
    {
        return vec<2, T>(v1.x - v2.x, v1.y - v2.y);
    }
    template<typename T>
    vec<2, T> operator*(const vec<2, T>& v1, const vec<2, T>& v2)
    {
        return vec<2, T>(v1.x * v2.x, v1.y * v2.y);
    }

    template<typename T>
    vec<2, T> operator/(const vec<2, T>& v1, T scalar)
    {
        return vec<2, T>(v1.x / scalar, v1.y / scalar);
    }

    template<typename T>
    T compMax(const vec<2, T>& v)
    {
        return max(v.x, v.z);
    }
}
#endif
