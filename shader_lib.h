#ifndef SHADER_LIB_H
#define SHADER_LIB_H

#include <algorithm> // std::clamp
#include <cassert>
#include <cmath> // floorf
#include <cstdint>

void WriteBMP(const char* filename, int width, int height, const uint8_t* pixelData);
void makeSwizzlers(uint thisVecSize, uint outVecSize);

#define LIB_UNREAL 0
#define LIB_STD 1
#define LIB_CURRENT_CONTEXT LIB_STD

#define LIB_GLSL 0
#define LIB_HLSL 1
#define LIB_CURRENT_LANGUAGE LIB_GLSL

// CPP ENVIRONMENT

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
#define CLAMP_IMPL(x, a, b) FMath::Clamp(x, a, b)
#define FLOORF_IMPL(x) FMath::FloorToFloat(x)
#define FRAC_IMPL(x) FMath::Frac(x)
#define LERP_IMPL(a, b, x) FMath::Lerp(a, b, x)
#define ABS_IMPL(x) FMath::Abs(x)
#define MIN_IMPL(a, b) FMath::Min(a, b)
#define MAX_IMPL(a, b) FMath::Max(a, b)

#elif LIB_CURRENT_CONTEXT == LIB_STD

#if defined(_MSC_VER)
#define FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCEINLINE inline __attribute__((always_inline))
// #define FORCEINLINE
#else
#define FORCEINLINE inline
#endif

#define CLAMP_IMPL(x, a, b) std::clamp(x, a, b)
#define FLOORF_IMPL(x) floorf(x)
FORCEINLINE float FRAC_IMPL(float x) { return x - floorf(x); }
template <typename T>
FORCEINLINE T LERP_IMPL(T a, T b, T x) { return a + (b - a) * x; }
#define ABS_IMPL(x) std::abs(x)
#define MIN_IMPL(a, b) std::min(a, b)
#define MAX_IMPL(a, b) std::max(a, b)
#endif

// LANGUAGE HLSL/GLSL

#if LIB_CURRENT_LANGUAGE == LIB_HLSL
#define VECTOR2 float2
#define VECTOR3 float3
#define VECTOR4 float4
#define FRAC frac
#elif LIB_CURRENT_LANGUAGE == LIB_GLSL
#define VECTOR2 vec2
#define VECTOR3 vec3
#define VECTOR4 vec4
#define FRAC fract // glsl: fract(), hlsl: frac()
#endif

template <typename T, std::size_t N>
constexpr bool areSwizzlersUnique(const T (&arr)[N])
{
    for (std::size_t i = 0; i < N; ++i)
        for (std::size_t j = i + 1; j < N; ++j)
            if (arr[i] == arr[j])
                return false;
    return true;
}

// clang-format off
struct VECTOR2;

template<int Size, int X, int Y>
struct Swiz2 {
    Swiz2() = delete;
    Swiz2& operator=(const VECTOR2& v);
    Swiz2& operator+=(const VECTOR2& v);
    Swiz2& operator-=(const VECTOR2& v);
    Swiz2& operator*=(const VECTOR2& v);
    Swiz2& operator/=(const VECTOR2& v);

private:
    float m[Size]; // size is templated to fit inside vec union
    friend struct VECTOR2;
};

struct VECTOR3;
template<int Size, int X, int Y, int Z>
struct Swiz3 {
    Swiz3() = delete;
    Swiz3& operator=(const VECTOR3& v);
    Swiz3& operator+=(const VECTOR3& v);
    Swiz3& operator-=(const VECTOR3& v);
    Swiz3& operator*=(const VECTOR3& v);
    Swiz3& operator/=(const VECTOR3& v);
private:
    float m[Size];
    friend struct VECTOR3;
};

struct VECTOR2 {
    union {
        struct { float x, y; }; struct { float r, g; };
        Swiz2<2, 0, 0> xx, rr;
        Swiz2<2, 0, 1> xy, rg;
        Swiz2<2, 1, 0> yx, gr;
        Swiz2<2, 1, 1> yy, gg;

        Swiz3<2, 0, 0, 0> xxx, rrr, sss;
        Swiz3<2, 0, 0, 1> xxy, rrg, sst;
        Swiz3<2, 0, 1, 0> xyx, rgr, sts;
        Swiz3<2, 0, 1, 1> xyy, rgg, stt;
        Swiz3<2, 1, 0, 0> yxx, grr, tss;
        Swiz3<2, 1, 0, 1> yxy, grg, tst;
        Swiz3<2, 1, 1, 0> yyx, ggr, tts;
        Swiz3<2, 1, 1, 1> yyy, ggg, ttt;
    };

    constexpr VECTOR2(float f) : x(f), y(f) {}
    constexpr VECTOR2(float x, float y) : x(x), y(y) {}
    template<int Size, int X, int Y> VECTOR2(const Swiz2<Size, X, Y>& s) : VECTOR2(s.m[X], s.m[Y]) {}
#if LIB_UNREAL
    VECTOR2(const FVector2f& u) : x(u.X), y(u.Y) {}
#endif

    // operator Swiz2<0, 1>() { return xy; }
    FORCEINLINE bool operator==(const VECTOR2& rhs) const { return x == rhs.x && y == rhs.y; }
    FORCEINLINE float operator[](uint i) const { assert(i < 2); return (&x)[i]; }
    friend FORCEINLINE VECTOR2 operator-(const VECTOR2& a) { return VECTOR2(-a.x, -a.y); }

#define SHADER_MATH_DECLARE_OPERATOR_VECTOR2(op) \
    FORCEINLINE VECTOR2 operator op(const VECTOR2& rhs) const { return VECTOR2(x op rhs.x, y op rhs.y); }     \
    FORCEINLINE VECTOR2 operator op(float f) const { return VECTOR2(x op f, y op f); }                        \
    FORCEINLINE friend VECTOR2 operator op(float f, const VECTOR2& v) { return VECTOR2(f op v.x, f op v.y); } \
    FORCEINLINE VECTOR2& operator op##=(const VECTOR2& rhs) { *this = *this op rhs; return *this; }

    SHADER_MATH_DECLARE_OPERATOR_VECTOR2(+)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR2(-)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR2(*)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR2(/)

#undef SHADER_MATH_DECLARE_OPERATOR_VECTOR2
};

static_assert(sizeof(VECTOR2) == 2 * sizeof(float));


// SWIZZLE 2 FUNCTIONS

template<int Size, int X, int Y>
FORCEINLINE Swiz2<Size, X, Y>& Swiz2<Size, X, Y>::operator=(const VECTOR2& v) {
    static_assert(areSwizzlersUnique({ X, Y })); m[X] = v.x, m[Y] = v.y; return *this;
}

//template<int Size, int X, int Y> Swiz2<Size, X, Y>& Swiz2<Size, X, Y>::operator+=(const vec2 &v) { return *this; }

#define SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(op) \
template<int Size, int X, int Y> FORCEINLINE VECTOR2 operator op(const Swiz2<Size, X, Y>& s, const VECTOR2& f) { return VECTOR2(s) op f; } \
template<int Size, int X, int Y> FORCEINLINE VECTOR2 operator op(float f, const Swiz2<Size, X, Y>& s) { return VECTOR2(f) op VECTOR2(s); } \
template<int Size, int X, int Y> Swiz2<Size, X, Y>& Swiz2<Size, X, Y>::operator op##=(const VECTOR2& v) \
{ static_assert(areSwizzlersUnique({ X, Y })); m[X] op##= v.x; m[Y] op##= v.y; return *this; }

SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(+)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(-)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(*)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(/)
#undef SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2

//
//
//

struct VECTOR3 {
    union {
        struct { float x, y, z; }; struct { float r, g, b; };

        Swiz2<3, 0, 0> xx, rr;
        Swiz2<3, 1, 1> yy, gg;
        Swiz2<3, 0, 1> xy, rg;
        Swiz2<3, 1, 0> yx, gr;
        Swiz2<3, 1, 2> yz;
        Swiz2<3, 2, 1> zy;

        Swiz3<3, 0, 0, 0> xxx, rrr;
        Swiz3<3, 0, 1, 2> xyz, rgb;
    };

    FORCEINLINE VECTOR3() {} // uninitialized
    constexpr FORCEINLINE VECTOR3(float f) : x(f), y(f), z(f) {}
    constexpr FORCEINLINE VECTOR3(float f, const VECTOR2& v2) : x(f), y(v2.x), z(v2.y) {}
    constexpr FORCEINLINE VECTOR3(const VECTOR2& v2, float f) : x(v2.x), y(v2.y), z(f) {}
    constexpr FORCEINLINE VECTOR3(float x, float y, float z) : x(x), y(y), z(z) {}
    template<int Size, int X, int Y, int Z>
    FORCEINLINE VECTOR3(const Swiz3<Size, X, Y, Z>& s) : VECTOR3(s.m[X], s.m[Y], s.m[Z]) {}

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
    explicit VECTOR3(const FVector3f& u) : x(u.X), y(u.Y), z(u.Z) {}
#endif

    FORCEINLINE bool operator==(const VECTOR3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    FORCEINLINE float operator[](uint i) const { assert(i < 3); return (&x)[i]; }
    friend FORCEINLINE VECTOR3 operator-(const VECTOR3& a) { return VECTOR3(-a.x, -a.y, -a.z); }

#define SHADER_MATH_DECLARE_OPERATOR_VECTOR3(op) \
    FORCEINLINE VECTOR3 operator op(const VECTOR3& rhs) const { return VECTOR3(x op rhs.x, y op rhs.y, z op rhs.z); }   \
    FORCEINLINE VECTOR3 operator op(float f) const { return VECTOR3(x op f, y op f, z op f); }                         \
    FORCEINLINE friend VECTOR3 operator op(float f, const VECTOR3& v) { return VECTOR3(f op v.x, f op v.y, f op v.z); } \
    FORCEINLINE VECTOR3& operator op##=(const VECTOR3& rhs) { *this = *this op rhs; return *this; }
    SHADER_MATH_DECLARE_OPERATOR_VECTOR3(+)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR3(-)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR3(*)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR3(/)
#undef SHADER_MATH_DECLARE_OPERATOR_VECTOR3
};

static_assert(sizeof(VECTOR3) == 3 * sizeof(float));

template <int Size, int X, int Y, int Z>
FORCEINLINE Swiz3<Size, X, Y, Z>& Swiz3<Size, X, Y, Z>::operator=(const VECTOR3& v) {
    static_assert(areSwizzlersUnique({ X, Y, Z })); m[X] = v.x, m[Y] = v.y, m[Z] = v.z; return *this;
}

#define SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR3(op) \
template<int Size, int X, int Y, int Z> FORCEINLINE VECTOR3 operator op(const Swiz3<Size, X, Y, Z>& s, const VECTOR3& f) { return VECTOR3(s) op f; } \
template<int Size, int X, int Y, int Z> FORCEINLINE VECTOR3 operator op(float f, const Swiz3<Size, X, Y, Z>& s) { return VECTOR3(f) op VECTOR3(s); } \
template<int Size, int X, int Y, int Z> Swiz3<Size, X, Y, Z>& Swiz3<Size, X, Y, Z>::operator op##=(const VECTOR3& v) \
{ static_assert(areSwizzlersUnique({ X, Y, Z })); m[X] op##= v.x; m[Y] op##= v.y; m[Z] op##= v.z; return *this; }

SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR3(+)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR3(-)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR3(*)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR3(/)
#undef SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR3

// FLOAT 4

struct VECTOR4 {
    union {
        struct { float x, y, z, w; }; struct { float r, g, b, a; };
        //Swiz3<0, 1, 2> xyz, rgb;
    };

    constexpr VECTOR4(float f) : x(f), y(f), z(f), w(f) {}
    constexpr VECTOR4(const VECTOR3& v3, float f) : x(v3.x), y(v3.y), z(v3.z), w(f) {}
    constexpr VECTOR4(float f, const VECTOR3& v3) : x(f), y(v3.x), z(v3.y), w(v3.z) {}
    constexpr VECTOR4(const VECTOR2& v21, const VECTOR2& v22) : x(v21.x), y(v21.y), z(v22.x), w(v22.y) {}
    constexpr VECTOR4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
    VECTOR4(const FVector4f& u) : x(u.X), y(u.Y), z(u.Z), w(u.W) {}
    VECTOR4(const FLinearColor& u) : x(u.R), y(u.G), z(u.B), w(u.A) {}
#endif
    FORCEINLINE bool operator==(const VECTOR4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    FORCEINLINE float operator[](uint i) const { assert(i < 4); return (&x)[i]; }
    friend FORCEINLINE VECTOR4 operator-(const VECTOR4& a) { return VECTOR4(-a.x, -a.y, -a.z, -a.w); }

#define SHADER_MATH_DECLARE_OPERATOR_VECTOR4(op) \
    FORCEINLINE VECTOR4 operator op(const VECTOR4& rhs) const { return VECTOR4(x op rhs.x, y op rhs.y, z op rhs.z, w op rhs.w); } \
    FORCEINLINE VECTOR4 operator op(float f) const { return VECTOR4(x op f, y op f, z op f, w op f); }                           \
    FORCEINLINE friend VECTOR4 operator op(float f, const VECTOR4& v) { return VECTOR4(f op v.x, f op v.y, f op v.z, f op v.w); } \
    FORCEINLINE VECTOR4& operator op##=(const VECTOR4& rhs) { *this = *this op rhs; return *this; }
    SHADER_MATH_DECLARE_OPERATOR_VECTOR4(+)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR4(-)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR4(*)
    SHADER_MATH_DECLARE_OPERATOR_VECTOR4(/)
#undef SHADER_MATH_DECLARE_OPERATOR_VECTOR4
};

static_assert(sizeof(VECTOR4) == 4 * sizeof(float));

// BASIC FUNCTIONS

#if LIB_CURRENT_LANGUAGE == LIB_HLSL
FORCEINLINE float saturate(const float a) { return CLAMP_IMPL(a, 0.f, 1.f); }
FORCEINLINE VECTOR2 saturate(const VECTOR2& a) { return VECTOR2(CLAMP_IMPL(a.x, 0.f, 1.f), CLAMP_IMPL(a.y, 0.f, 1.f)); }
FORCEINLINE VECTOR3 saturate(const VECTOR3& a) { return VECTOR3(CLAMP_IMPL(a.x, 0.f, 1.f), CLAMP_IMPL(a.y, 0.f, 1.f), CLAMP_IMPL(a.z, 0.f, 1.f)); }
FORCEINLINE VECTOR4 saturate(const VECTOR4& a) { return VECTOR4(CLAMP_IMPL(a.x, 0.f, 1.f), CLAMP_IMPL(a.y, 0.f, 1.f), CLAMP_IMPL(a.z, 0.f, 1.f), CLAMP_IMPL(a.w, 0.f, 1.f)); }
#endif

FORCEINLINE float dot(const VECTOR2& a, const VECTOR2& b) { return a.x * b.x + a.y * b.y; }
FORCEINLINE float dot(const VECTOR3& a, const VECTOR3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
FORCEINLINE float dot(const VECTOR4& a, const VECTOR4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

FORCEINLINE float length(const VECTOR2& a) { return std::sqrt(dot(a, a)); }
FORCEINLINE float length(const VECTOR3& a) { return std::sqrt(dot(a, a)); }
FORCEINLINE float length(const VECTOR4& a) { return std::sqrt(dot(a, a)); }

FORCEINLINE VECTOR2 normalize(const VECTOR2& a) { return a / length(a); }
FORCEINLINE VECTOR3 normalize(const VECTOR3& a) { return a / length(a); }
FORCEINLINE VECTOR4 normalize(const VECTOR4& a) { return a / length(a); }

FORCEINLINE float   FRAC(const float a) { return FRAC_IMPL(a); }
FORCEINLINE VECTOR2 FRAC(const VECTOR2& a) { return VECTOR2(FRAC_IMPL(a.x), FRAC_IMPL(a.y)); }
FORCEINLINE VECTOR3 FRAC(const VECTOR3& a) { return VECTOR3(FRAC_IMPL(a.x), FRAC_IMPL(a.y), FRAC_IMPL(a.z)); }
FORCEINLINE VECTOR4 FRAC(const VECTOR4& a) { return VECTOR4(FRAC_IMPL(a.x), FRAC_IMPL(a.y), FRAC_IMPL(a.z), FRAC_IMPL(a.w)); }

FORCEINLINE float   floor(const float a) { return FLOORF_IMPL(a); }
FORCEINLINE VECTOR2 floor(const VECTOR2& a) { return VECTOR2(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y)); }
FORCEINLINE VECTOR3 floor(const VECTOR3& a) { return VECTOR3(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y), FLOORF_IMPL(a.z)); }
FORCEINLINE VECTOR4 floor(const VECTOR4& a) { return VECTOR4(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y), FLOORF_IMPL(a.z), FLOORF_IMPL(a.w)); }

FORCEINLINE float   lerp(const float a, const float b, const float x) { return LERP_IMPL(a, b, x); }
FORCEINLINE VECTOR2 lerp(const VECTOR2& a, const VECTOR2& b, const VECTOR2& x) { return LERP_IMPL(a, b, x); }
FORCEINLINE VECTOR3 lerp(const VECTOR3& a, const VECTOR3& b, const VECTOR3& x) { return LERP_IMPL(a, b, x); }
FORCEINLINE VECTOR4 lerp(const VECTOR4& a, const VECTOR4& b, const VECTOR4& x) { return LERP_IMPL(a, b, x); }

// abs can conflict with std-s ones, if parameter type is not float.
// abs(3.0) - ambigious, abs(3.f) - fine
FORCEINLINE float   abs(const float v) { return ABS_IMPL(v); }
FORCEINLINE VECTOR2 abs(const VECTOR2& v) { return VECTOR2(ABS_IMPL(v.x), ABS_IMPL(v.y)); }
FORCEINLINE VECTOR3 abs(const VECTOR3& v) { return VECTOR3(ABS_IMPL(v.x), ABS_IMPL(v.y), ABS_IMPL(v.z)); }
FORCEINLINE VECTOR4 abs(const VECTOR4& v) { return VECTOR4(ABS_IMPL(v.x), ABS_IMPL(v.y), ABS_IMPL(v.z), ABS_IMPL(v.z)); }

FORCEINLINE float   min(const float a, const float b) { return MIN_IMPL(a, b); }
FORCEINLINE VECTOR2 min(const VECTOR2& a, const VECTOR2& b) { return VECTOR2(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y)); }
FORCEINLINE VECTOR3 min(const VECTOR3& a, const VECTOR3& b) { return VECTOR3(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y), MIN_IMPL(a.z, b.z)); }
FORCEINLINE VECTOR4 min(const VECTOR4& a, const VECTOR4& b) { return VECTOR4(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y), MIN_IMPL(a.z, b.z), MIN_IMPL(a.w, b.w)); }

FORCEINLINE float   max(const float a, const float b) { return MAX_IMPL(a, b); }
FORCEINLINE VECTOR2 max(const VECTOR2& a, const VECTOR2& b) { return VECTOR2(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y)); }
FORCEINLINE VECTOR3 max(const VECTOR3& a, const VECTOR3& b) { return VECTOR3(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y), MAX_IMPL(a.z, b.z)); }
FORCEINLINE VECTOR4 max(const VECTOR4& a, const VECTOR4& b) { return VECTOR4(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y), MAX_IMPL(a.z, b.z), MAX_IMPL(a.w, b.w)); }

FORCEINLINE float   clamp(const float x, const float inMin, const float inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE VECTOR2 clamp(const VECTOR2& x, const VECTOR2& inMin, const VECTOR2& inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE VECTOR3 clamp(const VECTOR3& x, const VECTOR3& inMin, const VECTOR3& inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE VECTOR4 clamp(const VECTOR4& x, const VECTOR4& inMin, const VECTOR4& inMax) { return min(inMax, max(x, inMin)); }

// template?
FORCEINLINE float   smoothstep(const float edge0, const float edge1, float t) { t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }
FORCEINLINE VECTOR2 smoothstep(const VECTOR2& edge0, const VECTOR2& edge1, VECTOR2 t) { t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }
FORCEINLINE VECTOR3 smoothstep(const VECTOR3& edge0, const VECTOR3& edge1, VECTOR3 t) { t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }
FORCEINLINE VECTOR4 smoothstep(const VECTOR4& edge0, const VECTOR4& edge1, VECTOR4 t) { t = clamp((t - edge0) / (edge1 - edge0), 0.f, 1.f); return t * t * (3.f - 2.f * t); }

// TRIGONOMETRY
// Trigonometry finctions have different implementations in CPU and GPU, and differ between GPU
// Creating noise function fract(5432.1 * sin(x*2345.6)...) can lead to different result

FORCEINLINE float   sin(const float v) { return std::sin(v); }
FORCEINLINE VECTOR2 sin(const VECTOR2& v) { return VECTOR2(std::sin(v.x), std::sin(v.y)); }
FORCEINLINE VECTOR3 sin(const VECTOR3& v) { return VECTOR3(std::sin(v.x), std::sin(v.y), std::sin(v.z)); }
FORCEINLINE VECTOR4 sin(const VECTOR4& v) { return VECTOR4(std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.z)); }

FORCEINLINE float   cos(const float v) { return std::cos(v); }
FORCEINLINE VECTOR2 cos(const VECTOR2& v) { return VECTOR2(std::cos(v.x), std::cos(v.y)); }
FORCEINLINE VECTOR3 cos(const VECTOR3& v) { return VECTOR3(std::cos(v.x), std::cos(v.y), std::cos(v.z)); }
FORCEINLINE VECTOR4 cos(const VECTOR4& v) { return VECTOR4(std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.z)); }

FORCEINLINE float  atan(float x, float y) { return std::atan2(x, y); }



struct mat3 {
    FORCEINLINE mat3(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8) {
        m[0] = vec3(f0, f1, f2), m[1] = vec3(f3, f4, f5), m[2] = vec3(f6, f7, f8);
    }

    FORCEINLINE mat3(const VECTOR3& a, const VECTOR3& b, const VECTOR3& c) {
        m[0] = a, m[1] = b, m[2] = c;
    }

    FORCEINLINE VECTOR3& operator[](int i) { return m[i]; }
    FORCEINLINE const VECTOR3& operator[](int i) const { return m[i]; }

private:
    VECTOR3 m[3];
};

FORCEINLINE VECTOR3 operator*(const mat3& m, const VECTOR3& v)
{
    return VECTOR3(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}

#include <functional>
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T> {}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hash_combine(seed, rest), ...);
}

namespace std {
template <>
struct hash<vec3> {
    std::size_t operator()(const vec3& v) const
    {
        size_t seed = 0;
        hash_combine(seed, v.x, v.y, v.z);
        return seed;
    }
};
}

// clang-format on

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PRECISION .3

#include <cstdio>
static void print(float f) { printf("%" STR(PRECISION) "f\n", f); }
static void print(VECTOR2 f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y); }
static void print(VECTOR3 f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y, f.z); }
static void print(VECTOR4 f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y, f.z, f.w); }

#endif // SHADER_LIB_H
