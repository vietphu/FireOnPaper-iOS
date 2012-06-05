
#ifndef HelloArrow_Vector_hpp
#define HelloArrow_Vector_hpp
#include <cmath>

const float Pi = 4 * std::atan(1.0f);
const float TwoPi = 2 * Pi;

template <typename T>
struct Vector2 {
    Vector2() {}
    Vector2(T x, T y) : x(x), y(y) {}
    T Dot(const Vector2& v) const
    {
        return x * v.x + y * v.y;
    }
    Vector2 operator+(const Vector2& v) const
    {
        return Vector2(x + v.x, y + v.y);
    }
    Vector2 operator-(const Vector2& v) const
    {
        return Vector2(x - v.x, y - v.y);
    }
    Vector2 operator/(float s) const
    {
        return Vector2(x / s, y / s);
    }
    Vector2 operator*(float s) const
    {
        return Vector2(x * s, y * s);
    }
    void Normalize()
    {
        float s = 1.0f / Length();
        x *= s;
        y *= s;
    }
    Vector2 Normalized() const 
    {
        Vector2 v = *this;
        v.Normalize();
        return v;
    }
    T LengthSquared() const
    {
        return x * x + y * y;
    }
    T Length() const
    {
        return sqrt(LengthSquared());
    }
    operator Vector2<float>() const
    {
        return Vector2<float>(x, y);
    }
    bool operator==(const Vector2& v) const
    {
        return x == v.x && y == v.y;
    }
    Vector2 Lerp(float t, const Vector2& v) const
    {
        return Vector2(x * (1 - t) + v.x * t,
                       y * (1 - t) + v.y * t);
    }
    template <typename P>
    P* Write(P* pData)
    {
        Vector2* pVector = (Vector2*) pData;
        *pVector++ = *this;
        return (P*) pVector;
    }
    T x;
    T y;
};

template <typename T>
struct LCVector3 {
    LCVector3() {}
    LCVector3(T x, T y, T z) : x(x), y(y), z(z) {}
    void Normalize()
    {
        float s = 1.0f / std::sqrt(x * x + y * y + z * z);
        x *= s;
        y *= s;
        z *= s;
    }
    LCVector3 Normalized() const 
    {
        LCVector3 v = *this;
        v.Normalize();
        return v;
    }
    LCVector3 Cross(const LCVector3& v) const
    {
        return LCVector3(y * v.z - z * v.y,
                       z * v.x - x * v.z,
                       x * v.y - y * v.x);
    }
    T Dot(const LCVector3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }
    LCVector3 operator+(const LCVector3& v) const
    {
        return LCVector3(x + v.x, y + v.y,  z + v.z);
    }
    void operator+=(const LCVector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    void operator-=(const LCVector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
    void operator/=(T s)
    {
        x /= s;
        y /= s;
        z /= s;
    }
    LCVector3 operator-(const LCVector3& v) const
    {
        return LCVector3(x - v.x, y - v.y,  z - v.z);
    }
    LCVector3 operator-() const
    {
        return LCVector3(-x, -y, -z);
    }
    LCVector3 operator*(T s) const
    {
        return LCVector3(x * s, y * s, z * s);
    }
    LCVector3 operator/(T s) const
    {
        return LCVector3(x / s, y / s, z / s);
    }
    bool operator==(const LCVector3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }
    LCVector3 Lerp(float t, const LCVector3& v) const
    {
        return LCVector3(x * (1 - t) + v.x * t,
                       y * (1 - t) + v.y * t,
                       z * (1 - t) + v.z * t);
    }
    const T* Pointer() const
    {
        return &x;
    }
    template <typename P>
    P* Write(P* pData)
    {
        LCVector3<T>* pVector = (LCVector3<T>*) pData;
        *pVector++ = *this;
        return (P*) pVector;
    }
    T x;
    T y;
    T z;
};

template <typename T>
struct LCVector4 {
    LCVector4() {}
    LCVector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    T Dot(const LCVector4& v) const
    {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }
    LCVector4 Lerp(float t, const LCVector4& v) const
    {
        return LCVector4(x * (1 - t) + v.x * t,
                       y * (1 - t) + v.y * t,
                       z * (1 - t) + v.z * t,
                       w * (1 - t) + v.w * t);
    }
    const T* Pointer() const
    {
        return &x;
    }
    T x;
    T y;
    T z;
    T w;
};

typedef Vector2<bool> bvec2;

typedef Vector2<int> ivec2;
typedef LCVector3<int> ivec3;
typedef LCVector4<int> ivec4;

typedef Vector2<float> vec2;
typedef LCVector3<float> vec3;
typedef LCVector4<float> vec4;

#endif
