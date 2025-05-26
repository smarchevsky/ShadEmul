#ifndef SHADER_LIB_H
#define SHADER_LIB_H

#include <algorithm> // std::clamp
#include <cassert>
#include <cmath> // floorf
#include <cstdint>

// swizzlers are .xyz .zyyy things
// there are a lot of combinations (swizzlers_44, it contains 256 of them)
// so they've been move in separate headers
// they are included by default, but you can
// #define ENABLE_SWIZZLERS_44 0, if it speeds up a compile time

// clang-format off

#define LIB_UNREAL 0
#define LIB_STD 1

#ifndef LIB_CURRENT_CONTEXT
#define LIB_CURRENT_CONTEXT LIB_STD
#endif

#define LIB_GLSL 0
#define LIB_HLSL 1

#ifndef LIB_CURRENT_LANGUAGE
#define LIB_CURRENT_LANGUAGE LIB_GLSL
#endif

#ifndef ENABLE_SWIZZLERS_33
#define ENABLE_SWIZZLERS_33 1
#endif
#ifndef ENABLE_SWIZZLERS_34
#define ENABLE_SWIZZLERS_34 1
#endif
#ifndef ENABLE_SWIZZLERS_43
#define ENABLE_SWIZZLERS_43 1
#endif
#ifndef ENABLE_SWIZZLERS_44
#define ENABLE_SWIZZLERS_44 1
#endif

// CPP ENVIRONMENT

#if LIB_CURRENT_CONTEXT != LIB_UNREAL
    #if defined(_MSC_VER)
        #define FORCEINLINE __forceinline
    #elif defined(__GNUC__) || defined(__clang__)
        #define FORCEINLINE inline __attribute__((always_inline))
        // #define FORCEINLINE
    #else
        #define FORCEINLINE inline
    #endif
#endif

template <typename T, std::size_t N>
constexpr bool areSwizzlersValid(const T (&arr)[N])
{
    for (std::size_t i = 0; i < N; ++i)
        for (std::size_t j = i + 1; j < N; ++j)
            if (arr[i] == arr[j])
                return false;
    return true;
}

template <typename T>
struct Vector2_base;
template <typename T, uint Size, uint X, uint Y>
struct Swiz2 {
    Swiz2() = delete;
    Swiz2& operator= (const Vector2_base<T>& v);
    Swiz2& operator+=(const Vector2_base<T>& v);
    Swiz2& operator-=(const Vector2_base<T>& v);
    Swiz2& operator*=(const Vector2_base<T>& v);
    Swiz2& operator/=(const Vector2_base<T>& v);

private:
    T m[Size]; // size is templated to fit inside vec union
    friend struct Vector2_base<T>;
};

template <typename T>
struct Vector3_base;
template <typename T, uint Size, uint X, uint Y, uint Z>
struct Swiz3 {
    Swiz3() = delete;
    Swiz3& operator= (const Vector3_base<T>& v);
    Swiz3& operator+=(const Vector3_base<T>& v);
    Swiz3& operator-=(const Vector3_base<T>& v);
    Swiz3& operator*=(const Vector3_base<T>& v);
    Swiz3& operator/=(const Vector3_base<T>& v);

private:
    T m[Size];
    friend struct Vector3_base<T>;
};

template <typename T>
struct Vector4_base;
template <typename T, uint Size, uint X, uint Y, uint Z, uint W>
struct Swiz4 {
    Swiz4() = delete;
    Swiz4& operator= (const Vector4_base<T>& v);
    Swiz4& operator+=(const Vector4_base<T>& v);
    Swiz4& operator-=(const Vector4_base<T>& v);
    Swiz4& operator*=(const Vector4_base<T>& v);
    Swiz4& operator/=(const Vector4_base<T>& v);

private:
    T m[Size];
    friend struct Vector4_base<T>;
};

// VECTOR 2

template <typename T>
struct Vector2_base {
    union {
        struct { T x, y; }; struct { T r, g; }; struct { float s, t; };

        Swiz2<T, 2, 0, 0> xx, rr, ss;
        Swiz2<T, 2, 0, 1> xy, rg, st;
        Swiz2<T, 2, 1, 0> yx, gr, ts;
        Swiz2<T, 2, 1, 1> yy, gg, tt;

        Swiz3<T, 2, 0, 0, 0> xxx, rrr, sss;
        Swiz3<T, 2, 0, 0, 1> xxy, rrg, sst;
        Swiz3<T, 2, 0, 1, 0> xyx, rgr, sts;
        Swiz3<T, 2, 0, 1, 1> xyy, rgg, stt;
        Swiz3<T, 2, 1, 0, 0> yxx, grr, tss;
        Swiz3<T, 2, 1, 0, 1> yxy, grg, tst;
        Swiz3<T, 2, 1, 1, 0> yyx, ggr, tts;
        Swiz3<T, 2, 1, 1, 1> yyy, ggg, ttt;

        Swiz4<T, 2, 0, 0, 0, 0> xxxx, rrrr, ssss;
        Swiz4<T, 2, 0, 0, 0, 1> xxxy, rrrg, ssst;
        Swiz4<T, 2, 0, 0, 1, 0> xxyx, rrgr, ssts;
        Swiz4<T, 2, 0, 0, 1, 1> xxyy, rrgg, sstt;
        Swiz4<T, 2, 0, 1, 0, 0> xyxx, rgrr, stss;
        Swiz4<T, 2, 0, 1, 0, 1> xyxy, rgrg, stst;
        Swiz4<T, 2, 0, 1, 1, 0> xyyx, rggr, stts;
        Swiz4<T, 2, 0, 1, 1, 1> xyyy, rggg, sttt;
        Swiz4<T, 2, 1, 0, 0, 0> yxxx, grrr, tsss;
        Swiz4<T, 2, 1, 0, 0, 1> yxxy, grrg, tsst;
        Swiz4<T, 2, 1, 0, 1, 0> yxyx, grgr, tsts;
        Swiz4<T, 2, 1, 0, 1, 1> yxyy, grgg, tstt;
        Swiz4<T, 2, 1, 1, 0, 0> yyxx, ggrr, ttss;
        Swiz4<T, 2, 1, 1, 0, 1> yyxy, ggrg, ttst;
        Swiz4<T, 2, 1, 1, 1, 0> yyyx, gggr, ttts;
        Swiz4<T, 2, 1, 1, 1, 1> yyyy, gggg, tttt;
    };

    Vector2_base() {}
    constexpr Vector2_base(T f) : x(f), y(f) {}
    constexpr Vector2_base(T x, T y) : x(x), y(y) {}
    template <uint Size, uint X, uint Y> Vector2_base(const Swiz2<T, Size, X, Y>& s) : Vector2_base(s.m[X], s.m[Y]) {}
#if LIB_UNREAL
    Vector2_base(const FVector2f& u) : x(u.X), y(u.Y) {}
#endif

    // operator Swiz2<0, 1>() { return xy; }
    FORCEINLINE bool operator==(const Vector2_base& rhs) const { return x == rhs.x && y == rhs.y; }

    FORCEINLINE T operator[](uint i) const { assert(i < 2); return (&x)[i]; }
    FORCEINLINE T& operator[](uint i) { assert(i < 2); return (&x)[i]; }

    friend FORCEINLINE Vector2_base operator-(const Vector2_base& a) { return Vector2_base(-a.x, -a.y); }

#define SHADER_MATH_DECLARE_OPERATOR_Vector2_base(op) \
    FORCEINLINE Vector2_base operator op(const Vector2_base& rhs) const     \
        { return Vector2_base(x op rhs.x, y op rhs.y); }                    \
    FORCEINLINE Vector2_base operator op(T f) const                         \
        { return Vector2_base(x op f, y op f); }                            \
    FORCEINLINE friend Vector2_base operator op(T f, const Vector2_base& v) \
        { return Vector2_base(f op v.x, f op v.y); }                        \
    FORCEINLINE Vector2_base& operator op##=(const Vector2_base & rhs)      \
        { *this = *this op rhs; return *this; }

SHADER_MATH_DECLARE_OPERATOR_Vector2_base(+)
SHADER_MATH_DECLARE_OPERATOR_Vector2_base(-)
SHADER_MATH_DECLARE_OPERATOR_Vector2_base(*)
SHADER_MATH_DECLARE_OPERATOR_Vector2_base(/)
#undef SHADER_MATH_DECLARE_OPERATOR_Vector2_base
};
static_assert(sizeof(Vector2_base<float>) == 2 * sizeof(float));

// VECTOR 3

template <typename T>
struct Vector3_base {
    union {
        struct { T x, y, z; }; struct { T r, g, b; }; struct { float s, t, p; };

        Swiz2<T, 3, 0, 0> xx, rr, ss;
        Swiz2<T, 3, 0, 1> xy, rg, st;
        Swiz2<T, 3, 0, 2> xz, rb, sp;
        Swiz2<T, 3, 1, 0> yx, gr, ts;
        Swiz2<T, 3, 1, 1> yy, gg, tt;
        Swiz2<T, 3, 1, 2> yz, gb, tp;
        Swiz2<T, 3, 2, 0> zx, br, ps;
        Swiz2<T, 3, 2, 1> zy, bg, pt;
        Swiz2<T, 3, 2, 2> zz, bb, pp;

#if ENABLE_SWIZZLERS_33
#include "swizzlers/swizzlers_33.h"
#endif

#if ENABLE_SWIZZLERS_34
#include "swizzlers/swizzlers_34.h"
#endif
    };

    Vector3_base() {}
    constexpr FORCEINLINE Vector3_base(T f): x(f), y(f), z(f) {}
    constexpr FORCEINLINE Vector3_base(T f, const Vector2_base<T>& v2) : x(f), y(v2.x), z(v2.y) {}
    constexpr FORCEINLINE Vector3_base(const Vector2_base<T>& v2, T f) : x(v2.x), y(v2.y), z(f) {}
    constexpr FORCEINLINE Vector3_base(T x, T y, T z) : x(x), y(y), z(z) {}

    template <uint Size, uint X, uint Y, uint Z>
    FORCEINLINE Vector3_base(const Swiz3<T, Size, X, Y, Z>& s) : Vector3_base(s.m[X], s.m[Y], s.m[Z]) {}

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
    explicit Vector3_base(const FVector3f& u) : x(u.X), y(u.Y), z(u.Z) {}
#endif

    FORCEINLINE bool operator==(const Vector3_base& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

    FORCEINLINE T operator[](uint i) const { assert(i < 3); return (&x)[i]; }
    FORCEINLINE T& operator[](uint i) { assert(i < 3); return (&x)[i]; }

    friend FORCEINLINE Vector3_base operator-(const Vector3_base& a) { return Vector3_base(-a.x, -a.y, -a.z); }

#define SHADER_MATH_DECLARE_OPERATOR_Vector3_base(op) \
    FORCEINLINE Vector3_base operator op(const Vector3_base& rhs) const     \
        { return Vector3_base(x op rhs.x, y op rhs.y, z op rhs.z); }        \
    FORCEINLINE Vector3_base operator op(T f) const                         \
        { return Vector3_base(x op f, y op f, z op f); }                    \
    FORCEINLINE friend Vector3_base operator op(T f, const Vector3_base& v) \
        { return Vector3_base(f op v.x, f op v.y, f op v.z); }              \
    FORCEINLINE Vector3_base& operator op##=(const Vector3_base & rhs)      \
        { *this = *this op rhs; return *this; }

SHADER_MATH_DECLARE_OPERATOR_Vector3_base(+)
SHADER_MATH_DECLARE_OPERATOR_Vector3_base(-)
SHADER_MATH_DECLARE_OPERATOR_Vector3_base(*)
SHADER_MATH_DECLARE_OPERATOR_Vector3_base(/)
#undef SHADER_MATH_DECLARE_OPERATOR_Vector3_base
};
static_assert(sizeof(Vector3_base<float>) == 3 * sizeof(float));

// VECTOR 4

template <typename T>
struct Vector4_base {
    union {
        struct { float x, y, z, w; }; struct { float r, g, b, a; }; struct { float s, t, p, q; };

        Swiz2<T, 4, 0, 0> xx, rr, ss;
        Swiz2<T, 4, 0, 1> xy, rg, st;
        Swiz2<T, 4, 0, 2> xz, rb, sp;
        Swiz2<T, 4, 0, 3> xw, ra, sq;
        Swiz2<T, 4, 1, 0> yx, gr, ts;
        Swiz2<T, 4, 1, 1> yy, gg, tt;
        Swiz2<T, 4, 1, 2> yz, gb, tp;
        Swiz2<T, 4, 1, 3> yw, ga, tq;
        Swiz2<T, 4, 2, 0> zx, br, ps;
        Swiz2<T, 4, 2, 1> zy, bg, pt;
        Swiz2<T, 4, 2, 2> zz, bb, pp;
        Swiz2<T, 4, 2, 3> zw, ba, pq;
        Swiz2<T, 4, 3, 0> wx, ar, qs;
        Swiz2<T, 4, 3, 1> wy, ag, qt;
        Swiz2<T, 4, 3, 2> wz, ab, qp;
        Swiz2<T, 4, 3, 3> ww, aa, qq;

#if ENABLE_SWIZZLERS_43
#include "swizzlers/swizzlers_43.h"
#endif

#if ENABLE_SWIZZLERS_44
#include "swizzlers/swizzlers_44.h"
#endif
    };

    Vector4_base() {}
    constexpr Vector4_base(T f) : x(f) , y(f), z(f), w(f) {}
    constexpr Vector4_base(const Vector3_base<T>& v3, T f) : x(v3.x), y(v3.y), z(v3.z), w(f) {}
    constexpr Vector4_base(T f, const Vector3_base<T>& v3) : x(f), y(v3.x), z(v3.y), w(v3.z) {}
    constexpr Vector4_base(const Vector2_base<T>& v21, const Vector2_base<T>& v22) : x(v21.x), y(v21.y), z(v22.x), w(v22.y) {}
    constexpr Vector4_base(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    template <uint Size, uint X, uint Y, uint Z, uint W>
    FORCEINLINE Vector4_base(const Swiz4<T, Size, X, Y, Z, W>& s) : Vector4_base(s.m[X], s.m[Y], s.m[Z], s.m[W]) {}

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
    Vector4_base(const FVector4f& u) : x(u.X), y(u.Y), z(u.Z), w(u.W) {}
    Vector4_base(const FLinearColor& u) : x(u.R), y(u.G), z(u.B), w(u.A) {}
#endif
    FORCEINLINE bool operator==(const Vector4_base& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }

    FORCEINLINE T operator[](uint i) const { assert(i < 4); return (&x)[i]; }
    FORCEINLINE T& operator[](uint i) { assert(i < 4); return (&x)[i]; }

    friend FORCEINLINE Vector4_base operator-(const Vector4_base& a) { return Vector4_base(-a.x, -a.y, -a.z, -a.w); }

#define SHADER_MATH_DECLARE_OPERATOR_Vector4_base(op) \
    FORCEINLINE Vector4_base operator op(const Vector4_base& rhs) const {      \
        return Vector4_base(x op rhs.x, y op rhs.y, z op rhs.z, w op rhs.w); } \
    FORCEINLINE Vector4_base operator op(T f) const {                          \
        return Vector4_base(x op f, y op f, z op f, w op f); }                 \
    FORCEINLINE friend Vector4_base operator op(T f, const Vector4_base& v) {  \
        return Vector4_base(f op v.x, f op v.y, f op v.z, f op v.w); }         \
    FORCEINLINE Vector4_base& operator op##=(const Vector4_base & rhs) {       \
        *this = *this op rhs; return *this; }

SHADER_MATH_DECLARE_OPERATOR_Vector4_base(+)
SHADER_MATH_DECLARE_OPERATOR_Vector4_base(-)
SHADER_MATH_DECLARE_OPERATOR_Vector4_base(*)
SHADER_MATH_DECLARE_OPERATOR_Vector4_base(/)
#undef SHADER_MATH_DECLARE_OPERATOR_Vector4_base
};
static_assert(sizeof(Vector4_base<float>) == 4 * sizeof(float));

//
//  SWIZZLE IMPL
//


// SWIZZLE 2 FUNCTIONS

template <typename T, uint Size, uint X, uint Y>
FORCEINLINE Swiz2<T, Size, X, Y>& Swiz2<T, Size, X, Y>::operator=(const Vector2_base<T>& v)
{
    static_assert(areSwizzlersValid({ X, Y }));
    m[X] = v.x, m[Y] = v.y;
    return *this;
}

#define SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector2_base(op) \
template <typename T, uint Size, uint X, uint Y>                                                 \
FORCEINLINE Vector2_base<T> operator op(const Swiz2<T, Size, X, Y>& s, const Vector2_base<T>& f) \
{ return Vector2_base<T>(s) op f; }                                                              \
template <typename T, uint Size, uint X, uint Y>                                                 \
FORCEINLINE Vector2_base<T> operator op(const Swiz2<T, Size, X, Y>& s, T f)                      \
{ return Vector2_base<T>(s) op Vector2_base<T>(f); }                                             \
template <typename T, uint Size, uint X, uint Y>                                                 \
FORCEINLINE Vector2_base<T> operator op(T f, const Swiz2<T, Size, X, Y>& s)                      \
{ return Vector2_base<T>(f) op Vector2_base<T>(s); }                                             \
template <typename T, uint Size, uint X, uint Y>                                                 \
Swiz2<T, Size, X, Y>& Swiz2<T, Size, X, Y>::operator op##=(const Vector2_base<T>& v)             \
{ static_assert(areSwizzlersValid({ X, Y })); m[X] op##= v.x, m[Y] op##= v.y; return *this; }

SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector2_base(+)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector2_base(-)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector2_base(*)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector2_base(/)
#undef SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector2_base

// SWIZZLE 3 FUNCTIONS

template <typename T, uint Size, uint X, uint Y, uint Z>
FORCEINLINE Swiz3<T, Size, X, Y, Z>& Swiz3<T, Size, X, Y, Z>::operator=(const Vector3_base<T>& v)
{
    static_assert(areSwizzlersValid({ X, Y, Z }));
    m[X] = v.x, m[Y] = v.y, m[Z] = v.z;
    return *this;
}

#define SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector3_base(op) \
template <typename T, uint Size, uint X, uint Y, uint Z>                                            \
FORCEINLINE Vector3_base<T> operator op(const Swiz3<T, Size, X, Y, Z>& s, const Vector3_base<T>& f) \
{ return Vector3_base<T>(s) op f; }                                                                 \
template <typename T, uint Size, uint X, uint Y, uint Z>                                            \
FORCEINLINE Vector3_base<T> operator op(const Swiz3<T, Size, X, Y, Z>& s, T f)                      \
{ return Vector3_base<T>(s) op Vector3_base<T>(f); }                                                \
template <typename T, uint Size, uint X, uint Y, uint Z>                                            \
FORCEINLINE Vector3_base<T> operator op(T f, const Swiz3<T, Size, X, Y, Z>& s)                      \
{ return Vector3_base<T>(f) op Vector3_base<T>(s); }                                                \
template <typename T, uint Size, uint X, uint Y, uint Z>                                            \
Swiz3<T, Size, X, Y, Z>& Swiz3<T, Size, X, Y, Z>::operator op##=(const Vector3_base<T>& v)          \
{ static_assert(areSwizzlersValid({ X, Y, Z })); m[X] op##= v.x, m[Y] op##= v.y, m[Z] op##= v.z; return *this; }

SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector3_base(+)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector3_base(-)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector3_base(*)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector3_base(/)
#undef SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector3_base

    // SWIZZLE 4 FUNCTIONS

template <typename T, uint Size, uint X, uint Y, uint Z, uint W>
FORCEINLINE Swiz4<T, Size, X, Y, Z, W>& Swiz4<T, Size, X, Y, Z, W>::operator=(const Vector4_base<T>& v)
{
    static_assert(areSwizzlersValid({ X, Y, Z, W }));
    m[X] = v.x, m[Y] = v.y, m[Z] = v.z;
    return *this;
}

#define SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector4_base(op) \
template <typename T, uint Size, uint X, uint Y, uint Z, uint W>                                       \
FORCEINLINE Vector4_base<T> operator op(const Swiz4<T, Size, X, Y, Z, W>& s, const Vector4_base<T>& f) \
{ return Vector4_base<T>(s) op f; }                                                                    \
template <typename T, uint Size, uint X, uint Y, uint Z, uint W>                                       \
FORCEINLINE Vector4_base<T> operator op(const Swiz4<T, Size, X, Y, Z, W>& s, T f)                      \
{ return Vector4_base<T>(s) op Vector4_base<T>(f); }                                                   \
template <typename T, uint Size, uint X, uint Y, uint Z, uint W>                                       \
FORCEINLINE Vector4_base<T> operator op(T f, const Swiz4<T, Size, X, Y, Z, W>& s)                      \
{ return Vector4_base<T>(f) op Vector4_base<T>(s); }                                                   \
template <typename T, uint Size, uint X, uint Y, uint Z, uint W>                                       \
Swiz4<T, Size, X, Y, Z, W>& Swiz4<T, Size, X, Y, Z, W>::operator op##=(const Vector4_base<T>& v)       \
{ static_assert(areSwizzlersValid({ X, Y, Z, W })); m[X] op##= v.x, m[Y] op##= v.y, m[Z] op##= v.z, m[W] op##= v.w; return *this; }

SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector4_base(+)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector4_base(-)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector4_base(*)
SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector4_base(/)
#undef SHADER_EMUL_DECLARE_SWIZZLE_OPERATOR_Vector4_base


// hash functions for unordered map
#include <functional>

namespace std {
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T> {}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hash_combine(seed, rest), ...);
}

template <typename T>
struct hash<Vector2_base<T>> {
    std::size_t operator()(const Vector2_base<T>& v) const { size_t seed = 0; hash_combine(seed, v.x, v.y); return seed; }
};

template <typename T>
struct hash<Vector3_base<T>> {
    std::size_t operator()(const Vector3_base<T>& v) const { size_t seed = 0; hash_combine(seed, v.x, v.y, v.z); return seed; }
};

template <typename T>
struct hash<Vector4_base<T>> {
    std::size_t operator()(const Vector4_base<T>& v) const { size_t seed = 0; hash_combine(seed, v.x, v.y, v.z, v.w); return seed; }
};
}

// LANGUAGE HLSL/GLSL

#if LIB_CURRENT_LANGUAGE == LIB_HLSL
    typedef Vector2_base<float> float2;
    typedef Vector3_base<float> float3;
    typedef Vector4_base<float> float4;
    typedef Vector2_base<int32_t> int2;
    typedef Vector3_base<int32_t> int3;
    typedef Vector4_base<int32_t> int4;
    typedef Vector2_base<int32_t> uint2;
    typedef Vector3_base<int32_t> uint3;
    typedef Vector4_base<int32_t> uint4;
    #define FRAC frac
#elif LIB_CURRENT_LANGUAGE == LIB_GLSL
    typedef Vector2_base<float> vec2;
    typedef Vector3_base<float> vec3;
    typedef Vector4_base<float> vec4;
    typedef Vector2_base<int32_t> ivec2;
    typedef Vector3_base<int32_t> ivec3;
    typedef Vector4_base<int32_t> ivec4;
    typedef Vector2_base<uint32_t> uvec2;
    typedef Vector3_base<uint32_t> uvec3;
    typedef Vector4_base<uint32_t> uvec4;
    #define FRAC fract // glsl: fract(), hlsl: frac()
#endif

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
    #define CLAMP_IMPL(x, a, b) FMath::Clamp(x, a, b)
    #define FLOORF_IMPL(x) FMath::FloorToFloat(x)
    #define FRAC_IMPL(x) FMath::Frac(x)
    #define LERP_IMPL(a, b, x) FMath::Lerp(a, b, x)
    #define ABS_IMPL(x) FMath::Abs(x)
    #define MIN_IMPL(a, b) FMath::Min(a, b)
    #define MAX_IMPL(a, b) FMath::Max(a, b)
#elif LIB_CURRENT_CONTEXT == LIB_STD
    #define CLAMP_IMPL(x, a, b) std::clamp(x, a, b)
    #define FLOORF_IMPL(x) floorf(x)
    FORCEINLINE float FRAC_IMPL(float x) { return x - floorf(x); }
    template <typename T> FORCEINLINE T LERP_IMPL(T a, T b, T x) { return a + (b - a) * x; }
    #define ABS_IMPL(x) std::abs(x)
    #define MIN_IMPL(a, b) std::min(a, b)
    #define MAX_IMPL(a, b) std::max(a, b)
    #define SIN_IMPL(x) std::sin(x)
    #define COS_IMPL(x) std::cos(x)
#endif

// BASIC FUNCTIONS

#if LIB_CURRENT_LANGUAGE == LIB_HLSL
FORCEINLINE float               saturate(const float a) { return CLAMP_IMPL(a, 0.f, 1.f); }
FORCEINLINE Vector2_base<float> saturate(const Vector2_base<float>& a) { return Vector2_base<float>(CLAMP_IMPL(a.x, 0.f, 1.f), CLAMP_IMPL(a.y, 0.f, 1.f)); }
FORCEINLINE Vector3_base<float> saturate(const Vector3_base<float>& a) { return Vector3_base<float>(CLAMP_IMPL(a.x, 0.f, 1.f), CLAMP_IMPL(a.y, 0.f, 1.f), CLAMP_IMPL(a.z, 0.f, 1.f)); }
FORCEINLINE Vector4_base<float> saturate(const Vector4_base<float>& a) { return Vector4_base<float>(CLAMP_IMPL(a.x, 0.f, 1.f), CLAMP_IMPL(a.y, 0.f, 1.f), CLAMP_IMPL(a.z, 0.f, 1.f), CLAMP_IMPL(a.w, 0.f, 1.f)); }
#endif

FORCEINLINE float dot(const Vector2_base<float>& a, const Vector2_base<float>& b) { return a.x * b.x + a.y * b.y; }
FORCEINLINE float dot(const Vector3_base<float>& a, const Vector3_base<float>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
FORCEINLINE float dot(const Vector4_base<float>& a, const Vector4_base<float>& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

FORCEINLINE float length(const Vector2_base<float>& a) { return std::sqrt(dot(a, a)); }
FORCEINLINE float length(const Vector3_base<float>& a) { return std::sqrt(dot(a, a)); }
FORCEINLINE float length(const Vector4_base<float>& a) { return std::sqrt(dot(a, a)); }

FORCEINLINE Vector2_base<float> normalize(const Vector2_base<float>& a) { return a / length(a); }
FORCEINLINE Vector3_base<float> normalize(const Vector3_base<float>& a) { return a / length(a); }
FORCEINLINE Vector4_base<float> normalize(const Vector4_base<float>& a) { return a / length(a); }

FORCEINLINE float               FRAC(const float a) { return FRAC_IMPL(a); }
FORCEINLINE Vector2_base<float> FRAC(const Vector2_base<float>& a) { return Vector2_base<float>(FRAC_IMPL(a.x), FRAC_IMPL(a.y)); }
FORCEINLINE Vector3_base<float> FRAC(const Vector3_base<float>& a) { return Vector3_base<float>(FRAC_IMPL(a.x), FRAC_IMPL(a.y), FRAC_IMPL(a.z)); }
FORCEINLINE Vector4_base<float> FRAC(const Vector4_base<float>& a) { return Vector4_base<float>(FRAC_IMPL(a.x), FRAC_IMPL(a.y), FRAC_IMPL(a.z), FRAC_IMPL(a.w)); }

FORCEINLINE float               floor(const float a) { return FLOORF_IMPL(a); }
FORCEINLINE Vector2_base<float> floor(const Vector2_base<float>& a) { return Vector2_base<float>(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y)); }
FORCEINLINE Vector3_base<float> floor(const Vector3_base<float>& a) { return Vector3_base<float>(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y), FLOORF_IMPL(a.z)); }
FORCEINLINE Vector4_base<float> floor(const Vector4_base<float>& a) { return Vector4_base<float>(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y), FLOORF_IMPL(a.z), FLOORF_IMPL(a.w)); }

FORCEINLINE float               lerp(const float a, const float b, const float x) { return LERP_IMPL(a, b, x); }
FORCEINLINE Vector2_base<float> lerp(const Vector2_base<float>& a, const Vector2_base<float>& b, const Vector2_base<float>& x) { return LERP_IMPL(a, b, x); }
FORCEINLINE Vector3_base<float> lerp(const Vector3_base<float>& a, const Vector3_base<float>& b, const Vector3_base<float>& x) { return LERP_IMPL(a, b, x); }
FORCEINLINE Vector4_base<float> lerp(const Vector4_base<float>& a, const Vector4_base<float>& b, const Vector4_base<float>& x) { return LERP_IMPL(a, b, x); }

// abs can conflict with std-s ones, if parameter type is not float.
// abs(3.0) - ambigious, abs(3.f) - fine
FORCEINLINE float               abs(const float v) { return ABS_IMPL(v); }
FORCEINLINE Vector2_base<float> abs(const Vector2_base<float>& v) { return Vector2_base<float>(ABS_IMPL(v.x), ABS_IMPL(v.y)); }
FORCEINLINE Vector3_base<float> abs(const Vector3_base<float>& v) { return Vector3_base<float>(ABS_IMPL(v.x), ABS_IMPL(v.y), ABS_IMPL(v.z)); }
FORCEINLINE Vector4_base<float> abs(const Vector4_base<float>& v) { return Vector4_base<float>(ABS_IMPL(v.x), ABS_IMPL(v.y), ABS_IMPL(v.z), ABS_IMPL(v.z)); }

FORCEINLINE float               sign(const float v) { return v > 0.f ? 1.f : v < 0.f ? -1.f : 0.f; }
FORCEINLINE Vector2_base<float> sign(const Vector2_base<float>& v) { return Vector2_base<float>(sign(v.x), sign(v.y)); }
FORCEINLINE Vector3_base<float> sign(const Vector3_base<float>& v) { return Vector3_base<float>(sign(v.x), sign(v.y), sign(v.z)); }
FORCEINLINE Vector4_base<float> sign(const Vector4_base<float>& v) { return Vector4_base<float>(sign(v.x), sign(v.y), sign(v.z), sign(v.w)); }

FORCEINLINE float               min(const float a, const float b) { return MIN_IMPL(a, b); }
FORCEINLINE Vector2_base<float> min(const Vector2_base<float>& a, const Vector2_base<float>& b) { return Vector2_base<float>(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y)); }
FORCEINLINE Vector3_base<float> min(const Vector3_base<float>& a, const Vector3_base<float>& b) { return Vector3_base<float>(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y), MIN_IMPL(a.z, b.z)); }
FORCEINLINE Vector4_base<float> min(const Vector4_base<float>& a, const Vector4_base<float>& b) { return Vector4_base<float>(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y), MIN_IMPL(a.z, b.z), MIN_IMPL(a.w, b.w)); }

FORCEINLINE float               max(const float a, const float b) { return MAX_IMPL(a, b); }
FORCEINLINE Vector2_base<float> max(const Vector2_base<float>& a, const Vector2_base<float>& b) { return Vector2_base<float>(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y)); }
FORCEINLINE Vector3_base<float> max(const Vector3_base<float>& a, const Vector3_base<float>& b) { return Vector3_base<float>(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y), MAX_IMPL(a.z, b.z)); }
FORCEINLINE Vector4_base<float> max(const Vector4_base<float>& a, const Vector4_base<float>& b) { return Vector4_base<float>(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y), MAX_IMPL(a.z, b.z), MAX_IMPL(a.w, b.w)); }

FORCEINLINE float               clamp(const float x, const float inMin, const float inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE Vector2_base<float> clamp(const Vector2_base<float>& x, const Vector2_base<float>& inMin, const Vector2_base<float>& inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE Vector3_base<float> clamp(const Vector3_base<float>& x, const Vector3_base<float>& inMin, const Vector3_base<float>& inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE Vector4_base<float> clamp(const Vector4_base<float>& x, const Vector4_base<float>& inMin, const Vector4_base<float>& inMax) { return min(inMax, max(x, inMin)); }

// template?
FORCEINLINE float smoothstep(const float edge0, const float edge1, float t) {
    t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }

FORCEINLINE Vector2_base<float> smoothstep(const Vector2_base<float>& edge0, const Vector2_base<float>& edge1, Vector2_base<float> t) {
    t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }

FORCEINLINE Vector3_base<float> smoothstep(const Vector3_base<float>& edge0, const Vector3_base<float>& edge1, Vector3_base<float> t) {
    t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }

FORCEINLINE Vector4_base<float> smoothstep(const Vector4_base<float>& edge0, const Vector4_base<float>& edge1, Vector4_base<float> t) {
    t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }

// TRIGONOMETRY
// Trigonometry finctions have different implementations in CPU and GPU, and differ between GPU
// Creating noise function fract(5432.1 * sin(x*2345.6)...) can lead to different result

FORCEINLINE float sin(const float v) { return SIN_IMPL(v); }
FORCEINLINE Vector2_base<float> sin(const Vector2_base<float>& v) { return Vector2_base<float>(SIN_IMPL(v.x), SIN_IMPL(v.y)); }
FORCEINLINE Vector3_base<float> sin(const Vector3_base<float>& v) { return Vector3_base<float>(SIN_IMPL(v.x), SIN_IMPL(v.y), SIN_IMPL(v.z)); }
FORCEINLINE Vector4_base<float> sin(const Vector4_base<float>& v) { return Vector4_base<float>(SIN_IMPL(v.x), SIN_IMPL(v.y), SIN_IMPL(v.z), SIN_IMPL(v.z)); }

FORCEINLINE float cos(const float v) { return COS_IMPL(v); }
FORCEINLINE Vector2_base<float> cos(const Vector2_base<float>& v) { return Vector2_base<float>(COS_IMPL(v.x), COS_IMPL(v.y)); }
FORCEINLINE Vector3_base<float> cos(const Vector3_base<float>& v) { return Vector3_base<float>(COS_IMPL(v.x), COS_IMPL(v.y), COS_IMPL(v.z)); }
FORCEINLINE Vector4_base<float> cos(const Vector4_base<float>& v) { return Vector4_base<float>(COS_IMPL(v.x), COS_IMPL(v.y), COS_IMPL(v.z), COS_IMPL(v.z)); }

FORCEINLINE float atan(float x, float y) { return std::atan2(x, y); }

struct mat3 {
    FORCEINLINE mat3(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8) {
        m[0] = Vector3_base<float>(f0, f1, f2), m[1] = Vector3_base<float>(f3, f4, f5), m[2] = Vector3_base<float>(f6, f7, f8);
    }

    FORCEINLINE mat3(const Vector3_base<float>& a, const Vector3_base<float>& b, const Vector3_base<float>& c) {
        m[0] = a, m[1] = b, m[2] = c;
    }

    FORCEINLINE Vector3_base<float>& operator[](uint i) { return m[i]; }
    FORCEINLINE const Vector3_base<float>& operator[](uint i) const { return m[i]; }

private:
    Vector3_base<float> m[3];
};

FORCEINLINE Vector3_base<float> operator*(const mat3& m, const Vector3_base<float>& v)
{
    return Vector3_base<float>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#include <cstdio>
#define PRECISION .3
static void print(float f)               { printf("%" STR(PRECISION) "f\n", f); }
static void print(Vector2_base<float> f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y); }
static void print(Vector3_base<float> f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y, f.z); }
static void print(Vector4_base<float> f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y, f.z, f.w); }

#endif // SHADER_LIB_H
