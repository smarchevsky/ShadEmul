#ifndef SHADER_LIB_H
#define SHADER_LIB_H

#include <algorithm> // std::clamp
#include <cmath> // floorf
#include <cstdint>
#include <cstdio>

void WriteBMP(const char* filename, int width, int height, const uint8_t* pixelData);

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
#else
#define FORCEINLINE inline
#endif

#define CLAMP_IMPL(x, a, b) std::clamp(x, a, b)
#define FLOORF_IMPL(x) floorf(x)
float FRAC_IMPL(float x) { return x - floorf(x); }
template <typename T>
T LERP_IMPL(T a, T b, T x) { return a + (b - a) * x; }
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

// clang-format off
struct VECTOR2;

template<int Size, int X, int Y>
struct Swiz2 {
    Swiz2(const VECTOR2& v);
    Swiz2() = delete;
private:
    float m[Size]; // size is templated to fit inside vec union
    friend struct VECTOR2;
};

struct VECTOR3;
template<int Size, int X, int Y, int Z>
struct Swiz3 {
    Swiz3(const VECTOR3& v);
private:
    float m[Size];
    friend struct VECTOR3;
};

struct VECTOR2 {
    union {
        struct { float x, y; }; struct { float r, g; };
        Swiz2<2, 0, 0> xx, rr;
        Swiz2<2, 1, 1> yy, gg;
        Swiz2<2, 0, 1> xy, rg;
        Swiz2<2, 1, 0> yx, gr;

        Swiz3<2, 0, 1, 0> xyx;
    };

    constexpr VECTOR2(float f) : x(f), y(f) {}
    constexpr VECTOR2(float x, float y) : x(x), y(y) {}
    template<int X, int Y> VECTOR2(const Swiz2<2, X, Y>& s) : VECTOR2(s.m[X], s.m[Y]) {}
#if LIB_UNREAL
    VECTOR2(const FVector2f& u) : x(u.X), y(u.Y) {}
#endif

    // operator Swiz2<0, 1>() { return xy; }
    bool operator==(const VECTOR2& rhs) const { return x == rhs.x && y == rhs.y; }

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

template<int Size, int X, int Y> FORCEINLINE Swiz2<Size, X, Y>::Swiz2(const VECTOR2 & v) { static_assert(X != Y); m[X] = v.x, m[Y] = v.y; }

#define SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(op) \
template<int Size, int X, int Y> FORCEINLINE VECTOR2 operator op(const Swiz2<Size, X, Y>& s, const VECTOR2& f) { return VECTOR2(s) op f; } \
template<int Size, int X, int Y> FORCEINLINE VECTOR2 operator op(float f, const Swiz2<Size, X, Y>& s) { return VECTOR2(f) op VECTOR2(s); } \
template<int Size, int X, int Y> Swiz2<Size, X, Y>& operator op##=(Swiz2<Size, X, Y>& s, const VECTOR2& v) \
{ static_assert(X != Y); s = Swiz2<Size, X, Y>(VECTOR2(s) op v); return s; }
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(+)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(-)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(*)
SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2(/)
#undef SHADER_MATH_DECLARE_SWIZZLE_OPERATOR_VECTOR2




struct VECTOR3 {
    union {
        struct { float x, y, z; }; struct { float r, g, b; };

        Swiz2<3, 0, 0> xx, rr;
        Swiz2<3, 1, 1> yy, gg;
        Swiz2<3, 0, 1> xy, rg;
        Swiz2<3, 1, 0> yx, gr;

        Swiz3<3, 0, 0, 0> xxx, rrr;
        Swiz3<3, 0, 1, 2> xyz, rgb;
    };

    constexpr VECTOR3(float f) : x(f), y(f), z(f) {}
    constexpr VECTOR3(float f, const VECTOR2& v2) : x(f), y(v2.x), z(v2.y) {}
    constexpr VECTOR3(const VECTOR2& v2, float f) : x(v2.x), y(v2.y), z(f) {}
    constexpr VECTOR3(float x, float y, float z) : x(x), y(y), z(z) {}
    template<int Size, int X, int Y, int Z>
    VECTOR3(const Swiz3<Size, X, Y, Z>& s) : VECTOR3(s.m[X], s.m[Y], s.m[Z]) {}

#if LIB_CURRENT_CONTEXT == LIB_UNREAL
    explicit VECTOR3(const FVector3f& u) : x(u.X), y(u.Y), z(u.Z) {}
#endif

    bool operator==(const VECTOR3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

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

template<int Size, int X, int Y, int Z> FORCEINLINE Swiz3<Size, X, Y, Z>::Swiz3(const VECTOR3 & v) { m[X] = v.x, m[Y] = v.y, m[Z] = v.z; }
template<int Size, int X, int Y, int Z> FORCEINLINE VECTOR3 operator+(const Swiz3<Size, X, Y, Z>& s, const VECTOR3& f) { return VECTOR3(s) + f; }
template<int Size, int X, int Y, int Z> FORCEINLINE VECTOR3 operator+(const VECTOR3& f, const Swiz3<Size, X, Y, Z>& s) { return VECTOR3(s) + f; }

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
    bool operator==(const VECTOR4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }

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

// clang-format on

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

FORCEINLINE float FRAC(const float a) { return FRAC_IMPL(a); }
FORCEINLINE VECTOR2 FRAC(const VECTOR2& a) { return VECTOR2(FRAC_IMPL(a.x), FRAC_IMPL(a.y)); }
FORCEINLINE VECTOR3 FRAC(const VECTOR3& a) { return VECTOR3(FRAC_IMPL(a.x), FRAC_IMPL(a.y), FRAC_IMPL(a.z)); }
FORCEINLINE VECTOR4 FRAC(const VECTOR4& a) { return VECTOR4(FRAC_IMPL(a.x), FRAC_IMPL(a.y), FRAC_IMPL(a.z), FRAC_IMPL(a.w)); }

FORCEINLINE float floor(const float a) { return FLOORF_IMPL(a); }
FORCEINLINE VECTOR2 floor(const VECTOR2& a) { return VECTOR2(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y)); }
FORCEINLINE VECTOR3 floor(const VECTOR3& a) { return VECTOR3(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y), FLOORF_IMPL(a.z)); }
FORCEINLINE VECTOR4 floor(const VECTOR4& a) { return VECTOR4(FLOORF_IMPL(a.x), FLOORF_IMPL(a.y), FLOORF_IMPL(a.z), FLOORF_IMPL(a.w)); }

FORCEINLINE float lerp(const float a, const float b, const float x) { return LERP_IMPL(a, b, x); }
FORCEINLINE VECTOR2 lerp(const VECTOR2& a, const VECTOR2& b, const VECTOR2& x) { return LERP_IMPL(a, b, x); }
FORCEINLINE VECTOR3 lerp(const VECTOR3& a, const VECTOR3& b, const VECTOR3& x) { return LERP_IMPL(a, b, x); }
FORCEINLINE VECTOR4 lerp(const VECTOR4& a, const VECTOR4& b, const VECTOR4& x) { return LERP_IMPL(a, b, x); }

FORCEINLINE float abs(const float v) { return ABS_IMPL(v); }
FORCEINLINE VECTOR2 abs(const VECTOR2& v) { return VECTOR2(ABS_IMPL(v.x), ABS_IMPL(v.y)); }
FORCEINLINE VECTOR3 abs(const VECTOR3& v) { return VECTOR3(ABS_IMPL(v.x), ABS_IMPL(v.y), ABS_IMPL(v.z)); }
FORCEINLINE VECTOR4 abs(const VECTOR4& v) { return VECTOR4(ABS_IMPL(v.x), ABS_IMPL(v.y), ABS_IMPL(v.z), ABS_IMPL(v.z)); }

FORCEINLINE float min(const float a, const float b) { return MIN_IMPL(a, b); }
FORCEINLINE VECTOR2 min(const VECTOR2& a, const VECTOR2& b) { return VECTOR2(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y)); }
FORCEINLINE VECTOR3 min(const VECTOR3& a, const VECTOR3& b) { return VECTOR3(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y), MIN_IMPL(a.z, b.z)); }
FORCEINLINE VECTOR4 min(const VECTOR4& a, const VECTOR4& b) { return VECTOR4(MIN_IMPL(a.x, b.x), MIN_IMPL(a.y, b.y), MIN_IMPL(a.z, b.z), MIN_IMPL(a.w, b.w)); }

FORCEINLINE float max(const float a, const float b) { return MAX_IMPL(a, b); }
FORCEINLINE VECTOR2 max(const VECTOR2& a, const VECTOR2& b) { return VECTOR2(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y)); }
FORCEINLINE VECTOR3 max(const VECTOR3& a, const VECTOR3& b) { return VECTOR3(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y), MAX_IMPL(a.z, b.z)); }
FORCEINLINE VECTOR4 max(const VECTOR4& a, const VECTOR4& b) { return VECTOR4(MAX_IMPL(a.x, b.x), MAX_IMPL(a.y, b.y), MAX_IMPL(a.z, b.z), MAX_IMPL(a.w, b.w)); }

FORCEINLINE float clamp(const float x, const float inMin, const float inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE VECTOR2 clamp(const VECTOR2& x, const VECTOR2& inMin, const VECTOR2& inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE VECTOR3 clamp(const VECTOR3& x, const VECTOR3& inMin, const VECTOR3& inMax) { return min(inMax, max(x, inMin)); }
FORCEINLINE VECTOR4 clamp(const VECTOR4& x, const VECTOR4& inMin, const VECTOR4& inMax) { return min(inMax, max(x, inMin)); }

// TRIGONOMETRY

FORCEINLINE float sin(const float v) { return std::sin(v); }
FORCEINLINE VECTOR2 sin(const VECTOR2& v) { return VECTOR2(std::sin(v.x), std::sin(v.y)); }
FORCEINLINE VECTOR3 sin(const VECTOR3& v) { return VECTOR3(std::sin(v.x), std::sin(v.y), std::sin(v.z)); }
FORCEINLINE VECTOR4 sin(const VECTOR4& v) { return VECTOR4(std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.z)); }

FORCEINLINE float cos(const float v) { return std::cos(v); }
FORCEINLINE VECTOR2 cos(const VECTOR2& v) { return VECTOR2(std::cos(v.x), std::cos(v.y)); }
FORCEINLINE VECTOR3 cos(const VECTOR3& v) { return VECTOR3(std::cos(v.x), std::cos(v.y), std::cos(v.z)); }
FORCEINLINE VECTOR4 cos(const VECTOR4& v) { return VECTOR4(std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.z)); }

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PRECISION .3
void print(float f) { printf("%" STR(PRECISION) "f\n", f); }
void print(VECTOR2 f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y); }
void print(VECTOR3 f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y, f.z); }
void print(VECTOR4 f) { printf("%" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f, %" STR(PRECISION) "f\n", f.x, f.y, f.z, f.w); }

#endif // SHADER_LIB_H
