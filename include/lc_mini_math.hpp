/*

Copyright (c) 2016, Libertus Code
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*

Large portions of lc_mini_math is borrowed from GLM:
    http://glm.g-truc.net/0.9.8/index.html
    https://github.com/g-truc/glm

*/

#pragma once

#if ! defined(__cplusplus)
    #error "C++ is required!"
#endif

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

#define LC_DEPTH_ZERO_TO_ONE            0x00000001
#define LC_DEPTH_NEGATIVE_ONE_TO_ONE    0x00000002

#if ! defined( LC_DEPTH_CLIP_SPACE )
    #define LC_DEPTH_CLIP_SPACE         LC_DEPTH_ZERO_TO_ONE
#endif

#if ! defined( LC_MINIMATH_NAMESPACE )
    #define LC_MINIMATH_NAMESPACE lc
#endif

namespace LC_MINIMATH_NAMESPACE {

    namespace detail {
        enum ctor{uninitialize};
    } // namespace detail

    template <typename T> struct tvec2;
    template <typename T> struct tvec3;
    template <typename T> struct tvec4;
    template <typename T> struct tmat2x2;
    template <typename T> struct tmat3x3;
    template <typename T> struct tmat4x4;

    //! \struct tvec2
    //!
    //!
    template <typename T> struct tvec2 {
        using value_type = T;

        union {
            struct { T x, y; };
            struct { T r, g; };
            struct { T s, t; };
        };

        tvec2();

        explicit tvec2(T s);
        explicit tvec2(T s1,
                       T s2);
        explicit tvec2(const tvec2<T>& v);

        template <typename X>
        tvec2(const X& s);

        template <typename X, typename Y>
        tvec2(const X& s1,
              const Y& s2);

        template <typename V>
        tvec2(const tvec2<V>& v);

        template <typename V>
        tvec2(const tvec3<V>& v);

        template <typename V>
        tvec2(const tvec4<V>& v);

        tvec2<T>&   operator=(const tvec2<T>& v);

        template <typename U>
        tvec2<T>&   operator=(const tvec2<U>& v);
        template <typename U>
        tvec2<T>&   operator=(const tvec3<U>& v);
        template <typename U>
        tvec2<T>&   operator=(const tvec4<U>& v);

        T&          operator[](std::size_t i);
        const T&    operator[](std::size_t i) const;

        tvec2<T>&   operator+=(const tvec2<T>& v);
        tvec2<T>&   operator-=(const tvec2<T>& v);
        tvec2<T>&   operator*=(const tvec2<T>& v);
        tvec2<T>&   operator/=(const tvec2<T>& v);

        template <typename S>   tvec2<T>&   operator+=(const S& s);
        template <typename S>   tvec2<T>&   operator-=(const S& s);
        template <typename S>   tvec2<T>&   operator*=(const S& s);
        template <typename S>   tvec2<T>&   operator/=(const S& s);

    };

    //! \struct tvec3
    //!
    //!
    template <typename T> struct tvec3 {
        using value_type = T;

        union {
            struct { T x, y, z; };
            struct { T r, g, b; };
            struct { T s, t, p; };
        };

        tvec3();
        explicit tvec3(T s);
        explicit tvec3(T s1,
                       T s2);
        explicit tvec3(T s1,
                       T s2,
                       T s3);
        explicit tvec3(const tvec3<T>& v);

        template <typename X>
        tvec3(const X& s);

        template <typename X, typename Y>
        tvec3(const X& s1,
              const Y& s2);

        template <typename X, typename Y, typename Z>
        tvec3(const X& s1,
              const Y& s2,
              const Z& s3);

        template <typename V, typename S>
        tvec3(const tvec2<V>& v,
              const S&        s);

        template <typename V, typename S>
        tvec3(const S&        s,
              const tvec2<V>& v);

        template <typename V>
        tvec3(const tvec2<V>& v);

        template <typename V>
        tvec3(const tvec3<V>& v);

        template <typename V>
        tvec3(const tvec4<V>& v);

        tvec3<T>&   operator=(const tvec3<T>& m);

        template <typename U>
        tvec3<T>&   operator=(const tvec2<U>& m);
        template <typename U>
        tvec3<T>&   operator=(const tvec3<U>& m);
        template <typename U>
        tvec3<T>&   operator=(const tvec4<U>& m);

        T&          operator[](std::size_t i);
        const T&    operator[](std::size_t i) const;

        tvec3<T>&   operator+=(const tvec3<T>& m);
        tvec3<T>&   operator-=(const tvec3<T>& m);
        tvec3<T>&   operator*=(const tvec3<T>& m);
        tvec3<T>&   operator/=(const tvec3<T>& m);

        template <typename S>   tvec3<T>&   operator+=(const S& s);
        template <typename S>   tvec3<T>&   operator-=(const S& s);
        template <typename S>   tvec3<T>&   operator*=(const S& s);
        template <typename S>   tvec3<T>&   operator/=(const S& s);
    };

    //! \struct tvec4
    //!
    //!
    template <typename T> struct tvec4 {
        using value_type = T;

        union {
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
            struct { T s, t, p, q; };
        };

        tvec4();
        explicit tvec4(T s);
        explicit tvec4(T s1,
                       T s2);
        explicit tvec4(T s1,
                       T s2,
                       T s3);
        explicit tvec4(T s1,
                       T s2,
                       T s3,
                       T s4);
        explicit tvec4(const tvec4<T>& v);

        template <typename X>
        tvec4(const X& s);

        template <typename X, typename Y>
        tvec4(const X& s1,
              const Y& s2);

        template <typename X, typename Y, typename Z>
        tvec4(const X& s1,
              const Y& s2,
              const Z& s3);

        template <typename X, typename Y, typename Z, typename W>
        tvec4(const X& s1,
              const Y& s2,
              const Z& s3,
              const W& s4);

        template <typename V, typename S1, typename S2>
        tvec4(const tvec2<V>& v,
              const S1&       s1,
              const S2&       s2);

        template <typename S1, typename V, typename S2>
        tvec4(const S1&       s1,
              const tvec2<V>& v,
              const S2&       s2);

        template <typename V, typename S1, typename S2>
        tvec4(const S1&       s1,
              const S2&       s2,
              const tvec2<V>& v);

        template <typename V1, typename V2>
        tvec4(const tvec2<V1>& v1,
              const tvec2<V2>& v2);

        template <typename V, typename S>
        tvec4(const tvec3<V>& v,
              const S&        s);

        template <typename S, typename V>
        tvec4(const S&        s,
              const tvec3<V>& v);

        template <typename V>
        tvec4(const tvec2<V>& v);

        template <typename V>
        tvec4(const tvec3<V>& v);

        template <typename V>
        tvec4(const tvec4<V>& v);

        tvec4<T>&   operator=(const tvec4<T>& v);

        template <typename U>
        tvec4<T>&   operator=(const tvec2<U>& v);
        template <typename U>
        tvec4<T>&   operator=(const tvec3<U>& v);
        template <typename U>
        tvec4<T>&   operator=(const tvec4<U>& v);

        T&          operator[](std::size_t i);
        const T&    operator[](std::size_t i) const;

        tvec4<T>&   operator+=(const tvec4<T>& v);
        tvec4<T>&   operator-=(const tvec4<T>& v);
        tvec4<T>&   operator*=(const tvec4<T>& v);
        tvec4<T>&   operator/=(const tvec4<T>& v);

        template <typename S>   tvec4<T>&   operator+=(const S& s);
        template <typename S>   tvec4<T>&   operator-=(const S& s);
        template <typename S>   tvec4<T>&   operator*=(const S& s);
        template <typename S>   tvec4<T>&   operator/=(const S& s);
    };

    //! \struct tmat2x2
    //!
    //!
    template <typename T> struct tmat2x2 {
        using value_type = T;
        using col_type = tvec2<T>;
        using row_type = tvec2<T>;

        col_type value[2];

        tmat2x2(detail::ctor);

        tmat2x2();

        explicit tmat2x2(const T& s);

        explicit tmat2x2(const T& x1, const T& y1,
                         const T& x2, const T& y2);

        explicit tmat2x2(const col_type& v1,
                         const col_type& v2);

        template <typename U>
        tmat2x2(const tvec2<U>& v1,
                const tvec2<U>& v2);

        template <typename U>
        tmat2x2(const tmat2x2<U>& m);

        template <typename U>
        tmat2x2(const tmat3x3<U>& m);

        template <typename U>
        tmat2x2(const tmat4x4<U>& m);

        tmat2x2<T>& operator=(const tmat2x2<T>& m);

        template <typename U>
        tmat2x2<T>& operator=(const tmat2x2<U>& m);

        col_type&           operator[](std::size_t i);
        const col_type&     operator[](std::size_t i) const;

        tmat2x2<T>& operator+=(const tmat2x2<T>& v);
        tmat2x2<T>& operator-=(const tmat2x2<T>& v);
        tmat2x2<T>& operator*=(const tmat2x2<T>& v);
        tmat2x2<T>& operator/=(const tmat2x2<T>& v);

        template <typename S>   tmat2x2<T>&   operator+=(const S& s);
        template <typename S>   tmat2x2<T>&   operator-=(const S& s);
        template <typename S>   tmat2x2<T>&   operator*=(const S& s);
        template <typename S>   tmat2x2<T>&   operator/=(const S& s);

        tmat2x2<T> _inverse() const;
        tmat2x2<T> _transpose() const;
    };

    //! \struct tmat3x3
    //!
    //!
    template <typename T> struct tmat3x3 {
        using value_type = T;
        using col_type = tvec3<T>;
        using row_type = tvec3<T>;

        col_type value[3];

        tmat3x3(detail::ctor);

        tmat3x3();

        explicit tmat3x3(const T& s);

        explicit tmat3x3(const T& x1, const T& y1, const T& z1,
                         const T& x2, const T& y2, const T& z2,
                         const T& x3, const T& y3, const T& z3);

        explicit tmat3x3(const col_type& v1,
                         const col_type& v2,
                         const col_type& v3);


        template <typename U>
        tmat3x3(const tvec3<U>& v1,
                const tvec3<U>& v2,
                const tvec3<U>& v3);

        template <typename U>
        tmat3x3(const tmat2x2<U>& m);

        template <typename U>
        tmat3x3(const tmat3x3<U>& m);

        template <typename U>
        tmat3x3(const tmat4x4<U>& m);

        tmat3x3<T>& operator=(const tmat3x3<T>& m);

        template <typename U>
        tmat3x3<T>& operator=(const tmat3x3<U>& m);

        col_type&           operator[](std::size_t i);
        const col_type&     operator[](std::size_t i) const;

        tmat3x3<T>& operator+=(const tmat3x3<T>& m);
        tmat3x3<T>& operator-=(const tmat3x3<T>& m);
        tmat3x3<T>& operator*=(const tmat3x3<T>& m);
        tmat3x3<T>& operator/=(const tmat3x3<T>& m);

        template <typename S>   tmat3x3<T>&   operator+=(const S& s);
        template <typename S>   tmat3x3<T>&   operator-=(const S& s);
        template <typename S>   tmat3x3<T>&   operator*=(const S& s);
        template <typename S>   tmat3x3<T>&   operator/=(const S& s);

        tmat3x3<T> _inverse() const;
        tmat3x3<T> _transpose() const;
    };

    //! \struct tmat4x4
    //!
    //!
    template <typename T> struct tmat4x4 {
        using value_type = T;
        using col_type = tvec4<T>;
        using row_type = tvec4<T>;

        col_type value[4];

        tmat4x4(detail::ctor);

        tmat4x4();

        explicit tmat4x4(const T& s);

        explicit tmat4x4(const T& x1, const T& y1, const T& z1, const T& w1,
                         const T& x2, const T& y2, const T& z2, const T& w2,
                         const T& x3, const T& y3, const T& z3, const T& w3,
                         const T& x4, const T& y4, const T& z4, const T& w4);

        explicit tmat4x4(const col_type& v1,
                         const col_type& v2,
                         const col_type& v3,
                         const col_type& v4);


        template <typename U>
        tmat4x4(const tvec4<U>& v1,
                const tvec4<U>& v2,
                const tvec4<U>& v3,
                const tvec4<U>& v4);

        template <typename U>
        tmat4x4(const tmat2x2<U>& m);

        template <typename U>
        tmat4x4(const tmat3x3<U>& m);

        template <typename U>
        tmat4x4(const tmat4x4<U>& m);

        tmat4x4<T>& operator=(const tmat4x4<T>& m);

        template <typename U>
        tmat4x4<T>& operator=(const tmat4x4<U>& m);

        col_type&           operator[](std::size_t i);
        const col_type&     operator[](std::size_t i) const;

        tmat4x4<T>& operator+=(const tmat4x4<T>& m);
        tmat4x4<T>& operator-=(const tmat4x4<T>& m);
        tmat4x4<T>& operator*=(const tmat4x4<T>& m);
        tmat4x4<T>& operator/=(const tmat4x4<T>& m);

        template <typename S>   tmat4x4<T>&   operator+=(const S& s);
        template <typename S>   tmat4x4<T>&   operator-=(const S& s);
        template <typename S>   tmat4x4<T>&   operator*=(const S& s);
        template <typename S>   tmat4x4<T>&   operator/=(const S& s);

        tmat4x4<T> _inverse() const;
        tmat4x4<T> _transpose() const;
    };

    template <typename T> struct tquat {
    };

    /*
    //! \struct tcoordsys2
    //!
    //!
    template <typename T> struct tcoordsys2 {
        using value_type = T;
        using vec_type = tvec2<T>;

        vec_type u;
        vec_type v;
    };

    //! \struct tcoordsys3
    //!
    //!
    template <typename T> struct tcoordsys3 {
        using value_type = T;
        using vec_type = tvec3<T>;

        vec_type u;
        vec_type v;
        vec_type w;
    };

    //! \struct tcoordsys4
    //!
    //!
    template <typename T> struct tcoordsys4 {
        using value_type = T;
        using vec_type = tvec4<T>;

        vec_type u;
        vec_type v;
        vec_type w;
    };
    */

    // Operator: v*v
    template <typename T> tvec2<T> operator*(const tvec2<T>& v1, const tvec2<T>& v2);
    template <typename T> tvec3<T> operator*(const tvec3<T>& v1, const tvec3<T>& v2);
    template <typename T> tvec4<T> operator*(const tvec4<T>& v1, const tvec4<T>& v2);

    // Operator: v/v
    template <typename T> tvec2<T> operator/(const tvec2<T>& v1, const tvec2<T>& v2);
    template <typename T> tvec3<T> operator/(const tvec3<T>& v1, const tvec3<T>& v2);
    template <typename T> tvec4<T> operator/(const tvec4<T>& v1, const tvec4<T>& v2);

    // Operator: v+v
    template <typename T> tvec2<T> operator+(const tvec2<T>& v1, const tvec2<T>& v2);
    template <typename T> tvec3<T> operator+(const tvec3<T>& v1, const tvec3<T>& v2);
    template <typename T> tvec4<T> operator+(const tvec4<T>& v1, const tvec4<T>& v2);

    // Operator: v-v
    template <typename T> tvec2<T> operator-(const tvec2<T>& v1, const tvec2<T>& v2);
    template <typename T> tvec3<T> operator-(const tvec3<T>& v1, const tvec3<T>& v2);
    template <typename T> tvec4<T> operator-(const tvec4<T>& v1, const tvec4<T>& v2);

    // Operator: v*s, s*v
    template <typename T, typename S> tvec2<T> operator*(const tvec2<T>& v, const S& s);
    template <typename T, typename S> tvec2<T> operator*(const S& s, const tvec2<T>& v);
    template <typename T, typename S> tvec3<T> operator*(const tvec3<T>& v, const S& s);
    template <typename T, typename S> tvec3<T> operator*(const S& s, const tvec3<T>& v);
    template <typename T, typename S> tvec4<T> operator*(const tvec4<T>& v, const S& s);
    template <typename T, typename S> tvec4<T> operator*(const S& s, const tvec4<T>& v);

    // Operator: m*s, s*m
    template <typename T, typename S> tmat2x2<T> operator*(const S& s, const tmat2x2<T>& m);
    template <typename T, typename S> tmat2x2<T> operator*(const tmat2x2<T>& m, const S& s);
    template <typename T, typename S> tmat3x3<T> operator*(const S& s, const tmat3x3<T>& m);
    template <typename T, typename S> tmat3x3<T> operator*(const tmat3x3<T>& m, const S& s);
    template <typename T, typename S> tmat4x4<T> operator*(const S& s, const tmat4x4<T>& m);
    template <typename T, typename S> tmat4x4<T> operator*(const tmat4x4<T>& m, const S& s);

    // Operator: m*v, v*m
    template <typename T> typename tmat2x2<T>::col_type operator*(const tmat2x2<T>& m, const tvec2<T>& v);
    template <typename T> typename tmat2x2<T>::row_type operator*(const tvec2<T>& v, const tmat2x2<T>& m);
    template <typename T> typename tmat3x3<T>::col_type operator*(const tmat3x3<T>& m, const tvec3<T>& v);
    template <typename T> typename tmat3x3<T>::row_type operator*(const tvec3<T>& v, const tmat3x3<T>& m);
    template <typename T> typename tmat4x4<T>::col_type operator*(const tmat4x4<T>& m, const tvec4<T>& v);
    template <typename T> typename tmat4x4<T>::row_type operator*(const tvec4<T>& v, const tmat4x4<T>& m);

    // Operator: m*m
    template <typename T> tmat2x2<T> operator*(const tmat2x2<T>& m1, const tmat2x2<T>& m2);
    template <typename T> tmat3x3<T> operator*(const tmat3x3<T>& m1, const tmat3x3<T>& m2);
    template <typename T> tmat4x4<T> operator*(const tmat4x4<T>& m1, const tmat4x4<T>& m2);

    // Operator: v==v
    template <typename T> bool operator==(const tvec2<T>& v1, const tvec2<T>& v2);
    template <typename T> bool operator==(const tvec3<T>& v1, const tvec3<T>& v2);
    template <typename T> bool operator==(const tvec4<T>& v1, const tvec4<T>& v2);

    // Operator: m==m
    template <typename T> bool operator==(const tmat2x2<T>& m1, const tmat2x2<T>& m2);
    template <typename T> bool operator==(const tmat3x3<T>& m1, const tmat3x3<T>& m2);
    template <typename T> bool operator==(const tmat4x4<T>& m1, const tmat4x4<T>& m2);

    // Dot
    template <typename T> T dot(const tvec2<T>& v1, const tvec2<T>& v2);
    template <typename T> T dot(const tvec3<T>& v1, const tvec3<T>& v2);
    template <typename T> T dot(const tvec4<T>& v1, const tvec4<T>& v2);

    // Cross Product
    template <typename T> tvec3<T> cross(const tvec3<T>& v1, const tvec3<T>& v2);

    // Length
    template <typename T> T length(const tvec2<T>& v);
    template <typename T> T length(const tvec3<T>& v);
    template <typename T> T length(const tvec4<T>& v);

    // LengthSquared
    template <typename T> T length2(const tvec2<T>& v);
    template <typename T> T length2(const tvec3<T>& v);
    template <typename T> T length2(const tvec4<T>& v);

    // Normalize
    template <typename T> tvec2<T> normalize(const tvec2<T>& v);
    template <typename T> tvec3<T> normalize(const tvec3<T>& v);
    template <typename T> tvec4<T> normalize(const tvec4<T>& v);

    // Distance: p0 - p1
    template <typename T> T distance(const tvec2<T>& p0, const tvec2<T>& p1);
    template <typename T> T distance(const tvec3<T>& p0, const tvec3<T>& p1);
    template <typename T> T distance(const tvec4<T>& p0, const tvec4<T>& p1);

    // DistanceSquared: p0 - p1
    template <typename T> T distance2(const tvec2<T>& p0, const tvec2<T>& p1);
    template <typename T> T distance2(const tvec3<T>& p0, const tvec3<T>& p1);
    template <typename T> T distance2(const tvec4<T>& p0, const tvec4<T>& p1);

    // Min
    template <typename T> tvec2<T> min(const tvec2<T>& p0, const tvec2<T>& p1);
    template <typename T> tvec3<T> min(const tvec3<T>& p0, const tvec3<T>& p1);
    template <typename T> tvec4<T> min(const tvec4<T>& p0, const tvec4<T>& p1);

    // Max
    template <typename T> tvec2<T> max(const tvec2<T>& p0, const tvec2<T>& p1);
    template <typename T> tvec3<T> max(const tvec3<T>& p0, const tvec3<T>& p1);
    template <typename T> tvec4<T> max(const tvec4<T>& p0, const tvec4<T>& p1);

    // Inverse
    template <typename T> tmat2x2<T> inverse(const tmat2x2<T>& m);
    template <typename T> tmat3x3<T> inverse(const tmat3x3<T>& m);
    template <typename T> tmat4x4<T> inverse(const tmat4x4<T>& m);

    // Transpose
    template <typename T> tmat2x2<T> transpose(const tmat2x2<T>& m);
    template <typename T> tmat3x3<T> transpose(const tmat3x3<T>& m);
    template <typename T> tmat4x4<T> transpose(const tmat4x4<T>& m);

    // Translate, Rotate, Scale
    template <typename T> tmat4x4<T> translate(const tvec3<T>& v);
    template <typename T> tmat4x4<T> rotate(const T& angle, const tvec3<T>& v);
    template <typename T> tmat4x4<T> scale(const tvec3<T>& v);

    // LookAt
    template <typename T> tmat4x4<T> lookAt(const tvec3<T>& eye, const tvec3<T>& center, const tvec3<T>& up);
    template <typename T> tmat4x4<T> lookAtLH(const tvec3<T>& eye, const tvec3<T>& center, const tvec3<T>& up);
    template <typename T> tmat4x4<T> lookAtRH(const tvec3<T>& eye, const tvec3<T>& center, const tvec3<T>& up);

    // Perspective
    template <typename T> tmat4x4<T> perspective(T fovy, T aspect, T zNear, T zFar);
    template <typename T> tmat4x4<T> perspectiveLH(T fovy, T aspect, T zNear, T zFar);
    template <typename T> tmat4x4<T> perspectiveRH(T fovy, T aspect, T zNear, T zFar);
    template <typename T> tmat4x4<T> perspectiveFov(T fov, T width, T height, T zNear, T zFar);
    template <typename T> tmat4x4<T> perspectiveFovLH(T fov, T width, T height, T zNear, T zFar);
    template <typename T> tmat4x4<T> perspectiveFovRH(T fov, T width, T height, T zNear, T zFar);


    // ---------------------------------------------------------------------------------------------
    // tvec2
    // ---------------------------------------------------------------------------------------------
    template <typename T>
    tvec2<T>::tvec2() : x(0),
                        y(0) {}

    template <typename T>
    tvec2<T>::tvec2(T s) : x(s),
                           y(s) {}

    template <typename T>
    tvec2<T>::tvec2(T s1,
                    T s2) : x(s1),
                            y(s2) {}

    template <typename T>
    tvec2<T>::tvec2(const tvec2<T>& v) : x(v.x),
                                         y(v.y) {}

    template <typename T>
    template <typename X>
    tvec2<T>::tvec2(const X& s) : x(static_cast<T>(s)),
                                  y(static_cast<T>(s)) {}


    template <typename T>
    template <typename X, typename Y>
    tvec2<T>::tvec2(const X& s1,
                    const Y& s2) : x(static_cast<T>(s1)),
                                   y(static_cast<T>(s2)) {}

    template <typename T>
    template <typename V>
    tvec2<T>::tvec2(const tvec2<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)) {}

    template <typename T>
    template <typename V>
    tvec2<T>::tvec2(const tvec3<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)) {}

    template <typename T>
    template <typename V>
    tvec2<T>::tvec2(const tvec4<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)) {}

    template <typename T>
    tvec2<T>& tvec2<T>::operator=(const tvec2<T>& v) {
        this->x = v.x;
        this->y = v.y;
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec2<T>& tvec2<T>::operator=(const tvec2<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec2<T>& tvec2<T>::operator=(const tvec3<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec2<T>& tvec2<T>::operator=(const tvec4<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        return *this;
    }

    template <typename T>
    T& tvec2<T>::operator[](std::size_t i) {
        return (&x)[i];
    }

    template <typename T>
    const T& tvec2<T>::operator[](std::size_t i) const {
        return (&x)[i];
    }

    template <typename T>
    tvec2<T>& tvec2<T>::operator+=(const tvec2<T>& v) {
        this->x += v.x;
        this->y += v.y;
        return *this;
    }

    template <typename T>
    tvec2<T>& tvec2<T>::operator-=(const tvec2<T>& v) {
        this->x -= v.x;
        this->y -= v.y;
        return *this;
    }

    template <typename T>
    tvec2<T>& tvec2<T>::operator*=(const tvec2<T>& v) {
        this->x *= v.x;
        this->y *= v.y;
        return *this;
    }

    template <typename T>
    tvec2<T>& tvec2<T>::operator/=(const tvec2<T>& v) {
        this->x /= v.x;
        this->y /= v.y;
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec2<T>& tvec2<T>::operator+=(const S& s) {
        this->x += static_cast<T>(s);
        this->y += static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec2<T>& tvec2<T>::operator-=(const S& s) {
        this->x -= static_cast<T>(s);
        this->y -= static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec2<T>& tvec2<T>::operator*=(const S& s) {
        this->x *= static_cast<T>(s);
        this->y *= static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec2<T>& tvec2<T>::operator/=(const S& s) {
        this->x /= static_cast<T>(s);
        this->y /= static_cast<T>(s);
        return *this;
    }

    // ---------------------------------------------------------------------------------------------
    // tvec3
    // ---------------------------------------------------------------------------------------------
    template <typename T>
    tvec3<T>::tvec3() : x(0),
                        y(0),
                        z(0) {}

    template <typename T>
    tvec3<T>::tvec3(T s) : x(s),
                           y(s),
                           z(s){}

    template <typename T>
    tvec3<T>::tvec3(T s1,
                    T s2) : x(s1),
                            y(s2),
                            z(0) {}

    template <typename T>
    tvec3<T>::tvec3(T s1,
                    T s2,
                    T s3) : x(s1),
                            y(s2),
                            z(s3) {}

    template <typename T>
    tvec3<T>::tvec3(const tvec3<T>& v) : x(v.x),
                                         y(v.y),
                                         z(v.z) {}

    template <typename T>
    template <typename X>
    tvec3<T>::tvec3(const X& s) : x(static_cast<T>(s)),
                                  y(static_cast<T>(s)),
                                  z(static_cast<T>(s)) {}

    template <typename T>
    template <typename X, typename Y>
    tvec3<T>::tvec3(const X& s1,
                    const Y& s2) : x(static_cast<T>(s1)),
                                   y(static_cast<T>(s2)),
                                   z(static_cast<T>(0)) {}

    template <typename T>
    template <typename X, typename Y, typename Z>
    tvec3<T>::tvec3(const X& s1,
                    const Y& s2,
                    const Z& s3) : x(static_cast<T>(s1)),
                                   y(static_cast<T>(s2)),
                                   z(static_cast<T>(s3)) {}

    template <typename T>
    template <typename V, typename S>
    tvec3<T>::tvec3(const tvec2<V>& v,
                    const S&        s) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(s)) {}

    template <typename T>
    template <typename V, typename S>
    tvec3<T>::tvec3(const S&        s,
                    const tvec2<V>& v) : x(static_cast<T>(s)),
                                         y(static_cast<T>(v.x)),
                                         z(static_cast<T>(v.y)) {}

    template <typename T>
    template <typename V>
    tvec3<T>::tvec3(const tvec2<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(0.0)) {}

    template <typename T>
    template <typename V>
    tvec3<T>::tvec3(const tvec3<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(v.z)) {}

    template <typename T>
    template <typename V>
    tvec3<T>::tvec3(const tvec4<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(v.z)) {}

    template <typename T>
    tvec3<T>& tvec3<T>::operator=(const tvec3<T>& v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec3<T>& tvec3<T>::operator=(const tvec2<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(0.0);
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec3<T>& tvec3<T>::operator=(const tvec3<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(v.z);
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec3<T>& tvec3<T>::operator=(const tvec4<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(v.z);
        return *this;
    }

    template <typename T>
    T& tvec3<T>::operator[](std::size_t i) {
        return (&x)[i];
    }

    template <typename T>
    const T& tvec3<T>::operator[](std::size_t i) const {
        return (&x)[i];
    }

    template <typename T>
    tvec3<T>& tvec3<T>::operator+=(const tvec3<T>& v) {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }

    template <typename T>
    tvec3<T>& tvec3<T>::operator-=(const tvec3<T>& v) {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }

    template <typename T>
    tvec3<T>& tvec3<T>::operator*=(const tvec3<T>& v) {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        return *this;
    }

    template <typename T>
    tvec3<T>& tvec3<T>::operator/=(const tvec3<T>& v) {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec3<T>& tvec3<T>::operator+=(const S& s) {
        this->x += static_cast<T>(s);
        this->y += static_cast<T>(s);
        this->z += static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec3<T>& tvec3<T>::operator-=(const S& s) {
        this->x -= static_cast<T>(s);
        this->y -= static_cast<T>(s);
        this->z -= static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec3<T>& tvec3<T>::operator*=(const S& s) {
        this->x *= static_cast<T>(s);
        this->y *= static_cast<T>(s);
        this->z *= static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec3<T>& tvec3<T>::operator/=(const S& s) {
        this->x /= static_cast<T>(s);
        this->y /= static_cast<T>(s);
        this->z /= static_cast<T>(s);
        return *this;
    }

    // ---------------------------------------------------------------------------------------------
    // tvec4
    // ---------------------------------------------------------------------------------------------
    template <typename T>
    tvec4<T>::tvec4() : x(0),
                        y(0),
                        z(0),
                        w(0) {}

    template <typename T>
    tvec4<T>::tvec4(T s) : x(s),
                           y(s),
                           z(s),
                           w(s) {}

    template <typename T>
    tvec4<T>::tvec4(T s1,
                    T s2) : x(s1),
                            y(s2),
                            z(0),
                            w(0) {}

    template <typename T>
    tvec4<T>::tvec4(T s1,
                    T s2,
                    T s3) : x(s1),
                            y(s2),
                            z(s3),
                            w(0) {}

    template <typename T>
    tvec4<T>::tvec4(T s1,
                    T s2,
                    T s3,
                    T s4) : x(s1),
                            y(s2),
                            z(s3),
                            w(s4) {}

    template <typename T>
    tvec4<T>::tvec4(const tvec4<T>& v) : x(v.x),
                                         y(v.y),
                                         z(v.z),
                                         w(v.w) {}

    template <typename T>
    template <typename X>
    tvec4<T>::tvec4(const X& s) : x(static_cast<T>(s)),
                                  y(static_cast<T>(s)),
                                  z(static_cast<T>(s)),
                                  w(static_cast<T>(s)) {}

    template <typename T>
    template <typename X, typename Y>
    tvec4<T>::tvec4(const X& s1,
                    const Y& s2): x(static_cast<T>(s1)),
                                  y(static_cast<T>(s2)),
                                  z(static_cast<T>(0)),
                                  w(static_cast<T>(0)) {}

    template <typename T>
    template <typename X, typename Y, typename Z>
    tvec4<T>::tvec4(const X& s1,
                    const Y& s2,
                    const Z& s3): x(static_cast<T>(s1)),
                                  y(static_cast<T>(s2)),
                                  z(static_cast<T>(s3)),
                                  w(static_cast<T>(0)) {}

    template <typename T>
    template <typename X, typename Y, typename Z, typename W>
    tvec4<T>::tvec4(const X& s1,
                    const Y& s2,
                    const Z& s3,
                    const W& s4) : x(static_cast<T>(s1)),
                                   y(static_cast<T>(s2)),
                                   z(static_cast<T>(s3)),
                                   w(static_cast<T>(s4)) {}

    template <typename T>
    template <typename V, typename S1, typename S2>
    tvec4<T>::tvec4(const tvec2<V>& v,
                    const S1&       s1,
                    const S2&       s2) : x(static_cast<T>(v.x)),
                                          y(static_cast<T>(v.y)),
                                          z(static_cast<T>(s1)),
                                          w(static_cast<T>(s2)) {}

    template <typename T>
    template <typename S1, typename V, typename S2>
    tvec4<T>::tvec4(const S1&       s1,
                    const tvec2<V>& v,
                    const S2&       s2) : x(static_cast<T>(s1)),
                                          y(static_cast<T>(v.x)),
                                          z(static_cast<T>(v.y)),
                                          w(static_cast<T>(s2)) {}

    template <typename T>
    template <typename V, typename S1, typename S2>
    tvec4<T>::tvec4(const S1&       s1,
                    const S2&       s2,
                    const tvec2<V>& v) : x(static_cast<T>(s1)),
                                         y(static_cast<T>(s2)),
                                         z(static_cast<T>(v.x)),
                                         w(static_cast<T>(v.y)) {}

    template <typename T>
    template <typename V1, typename V2>
    tvec4<T>::tvec4(const tvec2<V1>& v1,
                    const tvec2<V2>& v2) : x(static_cast<T>(v1.x)),
                                           y(static_cast<T>(v1.y)),
                                           z(static_cast<T>(v2.x)),
                                           w(static_cast<T>(v2.y)) {}

    template <typename T>
    template <typename V, typename S>
    tvec4<T>::tvec4(const tvec3<V>& v,
                    const S&        s) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(v.z)),
                                         w(static_cast<T>(s)) {}

    template <typename T>
    template <typename S, typename V>
    tvec4<T>::tvec4(const S&        s,
                    const tvec3<V>& v) : x(static_cast<T>(s)),
                                         y(static_cast<T>(v.x)),
                                         z(static_cast<T>(v.y)),
                                         w(static_cast<T>(v.z)) {}

    template <typename T>
    template <typename V>
    tvec4<T>::tvec4(const tvec2<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(0)),
                                         w(static_cast<T>(0)) {}

    template <typename T>
    template <typename V>
    tvec4<T>::tvec4(const tvec3<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(v.z)),
                                         w(static_cast<T>(0)) {}

    template <typename T>
    template <typename V>
    tvec4<T>::tvec4(const tvec4<V>& v) : x(static_cast<T>(v.x)),
                                         y(static_cast<T>(v.y)),
                                         z(static_cast<T>(v.z)),
                                         w(static_cast<T>(v.w)) {}

    template <typename T>
    tvec4<T>& tvec4<T>::operator=(const tvec4<T>& v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = v.w;
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec4<T>& tvec4<T>::operator=(const tvec2<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(0);
        this->w = static_cast<T>(0);
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec4<T>& tvec4<T>::operator=(const tvec3<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(v.z);
        this->w = static_cast<T>(0);
        return *this;
    }

    template <typename T>
    template <typename U>
    tvec4<T>& tvec4<T>::operator=(const tvec4<U>& v) {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(v.z);
        this->w = static_cast<T>(v.w);
        return *this;
    }

    template <typename T>
    T& tvec4<T>::operator[](std::size_t i) {
        return (&x)[i];
    }

    template <typename T>
    const T& tvec4<T>::operator[](std::size_t i) const {
        return (&x)[i];
    }

    template <typename T>
    tvec4<T>& tvec4<T>::operator+=(const tvec4<T>& v) {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        this->w += v.w;
        return *this;
    }

    template <typename T>
    tvec4<T>& tvec4<T>::operator-=(const tvec4<T>& v) {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        this->w -= v.w;
        return *this;
    }

    template <typename T>
    tvec4<T>& tvec4<T>::operator*=(const tvec4<T>& v) {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        this->w *= v.w;
        return *this;
    }

    template <typename T>
    tvec4<T>& tvec4<T>::operator/=(const tvec4<T>& v) {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        this->w /= v.w;
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec4<T>& tvec4<T>::operator+=(const S& s) {
        this->x += static_cast<T>(s);
        this->y += static_cast<T>(s);
        this->z += static_cast<T>(s);
        this->w += static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec4<T>& tvec4<T>::operator-=(const S& s) {
        this->x -= static_cast<T>(s);
        this->y -= static_cast<T>(s);
        this->z -= static_cast<T>(s);
        this->w -= static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec4<T>& tvec4<T>::operator*=(const S& s) {
        this->x *= static_cast<T>(s);
        this->y *= static_cast<T>(s);
        this->z *= static_cast<T>(s);
        this->w *= static_cast<T>(s);
        return *this;
    }

    template <typename T>
    template <typename S>
    tvec4<T>& tvec4<T>::operator/=(const S& s) {
        this->x /= static_cast<T>(s);
        this->y /= static_cast<T>(s);
        this->z /= static_cast<T>(s);
        this->w /= static_cast<T>(s);
        return *this;
    }

    // ---------------------------------------------------------------------------------------------
    // tmat2x2
    // ---------------------------------------------------------------------------------------------
    template <typename T>
    tmat2x2<T>::tmat2x2(detail::ctor) {}

    template <typename T>
    tmat2x2<T>::tmat2x2()
    {
        this->value[0] = col_type(1, 0);
        this->value[1] = col_type(0, 1);
    }

    template <typename T>
    tmat2x2<T>::tmat2x2(const T &s)
    {
      this->value[0] = col_type(s, 0);
      this->value[1] = col_type(0, s);
    }

    template <typename T>
    tmat2x2<T>::tmat2x2(const T& x1, const T& y1,
                        const T& x2, const T& y2)
    {
        this->value[0] = col_type(x1, y1);
        this->value[1] = col_type(x2, y2);
    }

    template <typename T>
    tmat2x2<T>::tmat2x2(const col_type& v1,
                        const col_type& v2)
    {
        this->value[0] = v1;
        this->value[1] = v2;
    }

    template <typename T>
    template <typename U>
    tmat2x2<T>::tmat2x2(const tvec2<U>& v1,
                        const tvec2<U>& v2)
    {
        this->value[0] = v1;
        this->value[1] = v2;
    }

    template <typename T>
    template <typename U>
    tmat2x2<T>::tmat2x2(const tmat2x2<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
    }

    template <typename T>
    template <typename U>
    tmat2x2<T>::tmat2x2(const tmat3x3<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
    }

    template <typename T>
    template <typename U>
    tmat2x2<T>::tmat2x2(const tmat4x4<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
    }

    template <typename T>
    tmat2x2<T>& tmat2x2<T>::operator=(const tmat2x2<T>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        return *this;
    }

    template <typename T>
    template <typename U>
    tmat2x2<T>& tmat2x2<T>::operator=(const tmat2x2<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        return *this;
    }

    template <typename T>
    typename tmat2x2<T>::col_type& tmat2x2<T>::operator[](std::size_t i) {
        return this->value[i];
    }

    template <typename T>
    const typename tmat2x2<T>::col_type& tmat2x2<T>::operator[](std::size_t i) const {
        return this->value[i];
    }

    template <typename T>
    tmat2x2<T>& tmat2x2<T>::operator+=(const tmat2x2<T>& m) {
        this->value[0] += m.value[0];
        this->value[1] += m.value[1];
        return *this;
    }

    template <typename T>
    tmat2x2<T>& tmat2x2<T>::operator-=(const tmat2x2<T>& m) {
        this->value[0] -= m.value[0];
        this->value[1] -= m.value[1];
        return *this;
    }

    template <typename T>
    tmat2x2<T>& tmat2x2<T>::operator*=(const tmat2x2<T>& m) {
        const auto v0 = this->value[0];
        const auto v1 = this->value[1];
        this->value[0] = v0 * m.value[0][0] + v1 * m.value[0][1];
        this->value[1] = v0 * m.value[1][0] + v1 * m.value[1][1];
        return *this;
    }

    template <typename T>
    tmat2x2<T>& tmat2x2<T>::operator/=(const tmat2x2<T>& m) {
        this->value[0] /= m.value[0];
        this->value[1] /= m.value[1];
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat2x2<T>& tmat2x2<T>::operator+=(const S& s) {
        this->value[0] += s;
        this->value[1] += s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat2x2<T>& tmat2x2<T>::operator-=(const S& s) {
        this->value[0] -= s;
        this->value[1] -= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat2x2<T>& tmat2x2<T>::operator*=(const S& s) {
        this->value[0] *= s;
        this->value[1] *= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat2x2<T>& tmat2x2<T>::operator/=(const S& s) {
        this->value[0] /= s;
        this->value[1] /= s;
        return *this;
    }

    template <typename T>
    tmat2x2<T> tmat2x2<T>::_inverse() const {
        const auto& m = this->value;

        T OneOverDeterminant = static_cast<T>(1) / (
            + m[0][0] * m[1][1]
            - m[1][0] * m[0][1]);

         tmat2x2 Inverse(
            + m[1][1] * OneOverDeterminant,
            - m[0][1] * OneOverDeterminant,
            - m[1][0] * OneOverDeterminant,
            + m[0][0] * OneOverDeterminant);

        return Inverse;
    }

    template <typename T>
    tmat2x2<T> tmat2x2<T>::_transpose() const {
        tmat2x2 Transpose(detail::uninitialize);
        const auto& m = this->value;

        Transpose[0][0] = m[0][0];
        Transpose[0][1] = m[1][0];

        Transpose[1][0] = m[0][1];
        Transpose[1][1] = m[1][1];
        return Transpose;
    }

    // ---------------------------------------------------------------------------------------------
    // tmat3x3
    // ---------------------------------------------------------------------------------------------
    template <typename T>
    tmat3x3<T>::tmat3x3(detail::ctor) {}

    template <typename T>
    tmat3x3<T>::tmat3x3()
    {
        this->value[0] = col_type(1, 0, 0);
        this->value[1] = col_type(0, 1, 0);
        this->value[2] = col_type(0, 0, 1);
    }

    template <typename T>
    tmat3x3<T>::tmat3x3(const T &s)
    {
        this->value[0] = col_type(s, 0, 0);
        this->value[1] = col_type(0, s, 0);
        this->value[2] = col_type(0, 0, s);
    }

    template <typename T>
    tmat3x3<T>::tmat3x3(const T& x1, const T& y1, const T& z1,
                        const T& x2, const T& y2, const T& z2,
                        const T& x3, const T& y3, const T& z3)
    {
        this->value[0] = col_type(x1, y1, z1);
        this->value[1] = col_type(x2, y2, z2);
        this->value[2] = col_type(x3, y3, z3);
    }

    template <typename T>
    tmat3x3<T>::tmat3x3(const col_type& v1,
                        const col_type& v2,
                        const col_type& v3)
    {
        this->value[0] = v1;
        this->value[1] = v2;
        this->value[2] = v3;
    }

    template <typename T>
    template <typename U>
    tmat3x3<T>::tmat3x3(const tvec3<U>& v1,
                        const tvec3<U>& v2,
                        const tvec3<U>& v3)
    {
        this->value[0] = v1;
        this->value[1] = v2;
        this->value[2] = v3;
    }

    template <typename T>
    template <typename U>
    tmat3x3<T>::tmat3x3(const tmat2x2<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = col_type(0, 0, 1);
    }

    template <typename T>
    template <typename U>
    tmat3x3<T>::tmat3x3(const tmat3x3<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
    }

    template <typename T>
    template <typename U>
    tmat3x3<T>::tmat3x3(const tmat4x4<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
    }

    template <typename T>
    tmat3x3<T>& tmat3x3<T>::operator=(const tmat3x3<T>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
        return *this;
    }

    template <typename T>
    template <typename U>
    tmat3x3<T>& tmat3x3<T>::operator=(const tmat3x3<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
        return *this;
    }

    template <typename T>
    typename tmat3x3<T>::col_type& tmat3x3<T>::operator[](std::size_t i) {
        return this->value[i];
    }

    template <typename T>
    const typename tmat3x3<T>::col_type& tmat3x3<T>::operator[](std::size_t i) const {
        return this->value[i];
    }

    template <typename T>
    tmat3x3<T>& tmat3x3<T>::operator+=(const tmat3x3<T>& m) {
        this->value[0] += m.value[0];
        this->value[1] += m.value[1];
        this->value[2] += m.value[2];
        return *this;
    }

    template <typename T>
    tmat3x3<T>& tmat3x3<T>::operator-=(const tmat3x3<T>& m) {
        this->value[0] -= m.value[0];
        this->value[1] -= m.value[1];
        this->value[2] -= m.value[2];
        return *this;
    }

    template <typename T>
    tmat3x3<T>& tmat3x3<T>::operator*=(const tmat3x3<T>& m) {
        const auto v0 = this->value[0];
        const auto v1 = this->value[1];
        const auto v2 = this->value[2];
        this->value[0] = v0 * m.value[0][0] + v1 * m.value[0][1] + v2 * m.value[0][2];
        this->value[1] = v0 * m.value[1][0] + v1 * m.value[1][1] + v2 * m.value[1][2];
        this->value[2] = v0 * m.value[2][0] + v1 * m.value[2][1] + v2 * m.value[2][2];
        return *this;
    }

    template <typename T>
    tmat3x3<T>& tmat3x3<T>::operator/=(const tmat3x3<T>& m) {
        this->value[0] /= m.value[0];
        this->value[1] /= m.value[1];
        this->value[2] /= m.value[2];
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat3x3<T>& tmat3x3<T>::operator+=(const S& s) {
        this->value[0] += s;
        this->value[1] += s;
        this->value[2] += s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat3x3<T>& tmat3x3<T>::operator-=(const S& s) {
        this->value[0] -= s;
        this->value[1] -= s;
        this->value[2] -= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat3x3<T>& tmat3x3<T>::operator*=(const S& s) {
        this->value[0] *= s;
        this->value[1] *= s;
        this->value[2] *= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat3x3<T>& tmat3x3<T>::operator/=(const S& s) {
        this->value[0] /= s;
        this->value[1] /= s;
        this->value[2] /= s;
        return *this;
    }

    template <typename T>
    tmat3x3<T> tmat3x3<T>::_inverse() const {
        auto& m = this->value;

        T OneOverDeterminant = static_cast<T>(1) / (
            + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
            - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
            + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

        tmat3x3 Inverse(detail::uninitialize);
        Inverse[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDeterminant;
        Inverse[1][0] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDeterminant;
        Inverse[2][0] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDeterminant;
        Inverse[0][1] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDeterminant;
        Inverse[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDeterminant;
        Inverse[2][1] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDeterminant;
        Inverse[0][2] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDeterminant;
        Inverse[1][2] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDeterminant;
        Inverse[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDeterminant;

        return Inverse;
    }

    template <typename T>
    tmat3x3<T> tmat3x3<T>::_transpose() const {
        tmat3x3 Transpose(detail::uninitialize);
        const auto& m = this->value;

        Transpose[0][0] = m[0][0];
        Transpose[0][1] = m[1][0];
        Transpose[0][2] = m[2][0];

        Transpose[1][0] = m[0][1];
        Transpose[1][1] = m[1][1];
        Transpose[1][2] = m[2][1];

        Transpose[2][0] = m[0][2];
        Transpose[2][1] = m[1][2];
        Transpose[2][2] = m[2][2];
        return Transpose;
    }

    // ---------------------------------------------------------------------------------------------
    // tmat4x4
    // ---------------------------------------------------------------------------------------------
    template <typename T>
    tmat4x4<T>::tmat4x4(detail::ctor) {}

    template <typename T>
    tmat4x4<T>::tmat4x4()
    {
        this->value[0] = col_type(1, 0, 0, 0);
        this->value[1] = col_type(0, 1, 0, 0);
        this->value[2] = col_type(0, 0, 1, 0);
        this->value[3] = col_type(0, 0, 0, 1);
    }

    template <typename T>
    tmat4x4<T>::tmat4x4(const T &s)
    {
        this->value[0] = col_type(s, 0, 0, 0);
        this->value[1] = col_type(0, s, 0, 0);
        this->value[2] = col_type(0, 0, s, 0);
        this->value[3] = col_type(0, 0, 0, s);
    }

    template <typename T>
    tmat4x4<T>::tmat4x4(const T& x1, const T& y1, const T& z1, const T& w1,
                        const T& x2, const T& y2, const T& z2, const T& w2,
                        const T& x3, const T& y3, const T& z3, const T& w3,
                        const T& x4, const T& y4, const T& z4, const T& w4)
    {
        this->value[0] = col_type(x1, y1, z1, w1);
        this->value[1] = col_type(x2, y2, z2, w2);
        this->value[2] = col_type(x3, y3, z3, w3);
        this->value[3] = col_type(x4, y4, z4, w4);
    }

    template <typename T>
    tmat4x4<T>::tmat4x4(const col_type& v1,
                        const col_type& v2,
                        const col_type& v3,
                        const col_type& v4)
    {
        this->value[0] = v1;
        this->value[1] = v2;
        this->value[2] = v3;
        this->value[3] = v4;
    }

    template <typename T>
    template <typename U>
    tmat4x4<T>::tmat4x4(const tvec4<U>& v1,
                        const tvec4<U>& v2,
                        const tvec4<U>& v3,
                        const tvec4<U>& v4)
    {
        this->value[0] = v1;
        this->value[1] = v2;
        this->value[2] = v3;
        this->value[3] = v4;
    }

    template <typename T>
    template <typename U>
    tmat4x4<T>::tmat4x4(const tmat2x2<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = col_type(0, 0, 1, 0);
        this->value[2] = col_type(0, 0, 0, 1);
    }

    template <typename T>
    template <typename U>
    tmat4x4<T>::tmat4x4(const tmat3x3<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
        this->value[3] = col_type(0, 0, 0, 1);
    }

    template <typename T>
    template <typename U>
    tmat4x4<T>::tmat4x4(const tmat4x4<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
        this->value[4] = m.value[4];
    }

    template <typename T>
    tmat4x4<T>& tmat4x4<T>::operator=(const tmat4x4<T>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
        this->value[3] = m.value[3];
        return *this;
    }

    template <typename T>
    template <typename U>
    tmat4x4<T>& tmat4x4<T>::operator=(const tmat4x4<U>& m) {
        this->value[0] = m.value[0];
        this->value[1] = m.value[1];
        this->value[2] = m.value[2];
        this->value[3] = m.value[3];
        return *this;
    }

    template <typename T>
    typename tmat4x4<T>::col_type& tmat4x4<T>::operator[](std::size_t i) {
        return this->value[i];
    }

    template <typename T>
    const typename tmat4x4<T>::col_type& tmat4x4<T>::operator[](std::size_t i) const {
        return this->value[i];
    }

    template <typename T>
    tmat4x4<T>& tmat4x4<T>::operator+=(const tmat4x4<T>& m) {
        this->value[0] += m.value[0];
        this->value[1] += m.value[1];
        this->value[2] += m.value[2];
        this->value[3] += m.value[3];
        return *this;
    }

    template <typename T>
    tmat4x4<T>& tmat4x4<T>::operator-=(const tmat4x4<T>& m) {
        this->value[0] -= m.value[0];
        this->value[1] -= m.value[1];
        this->value[2] -= m.value[2];
        this->value[3] -= m.value[3];
        return *this;
    }

    template <typename T>
    tmat4x4<T>& tmat4x4<T>::operator*=(const tmat4x4<T>& m) {
      const auto v0 = this->value[0];
      const auto v1 = this->value[1];
      const auto v2 = this->value[2];
      const auto v3 = this->value[3];
      this->value[0] = v0 * m.value[0][0] + v1 * m.value[0][1] + v2 * m.value[0][2] + v3 * m.value[0][3];
      this->value[1] = v0 * m.value[1][0] + v1 * m.value[1][1] + v2 * m.value[1][2] + v3 * m.value[1][3];
      this->value[2] = v0 * m.value[2][0] + v1 * m.value[2][1] + v2 * m.value[2][2] + v3 * m.value[2][3];
      this->value[3] = v0 * m.value[3][0] + v1 * m.value[3][1] + v2 * m.value[3][2] + v3 * m.value[3][3];
      return *this;
    }

    template <typename T>
    tmat4x4<T>& tmat4x4<T>::operator/=(const tmat4x4<T>& m) {
        this->value[0] /= m.value[0];
        this->value[1] /= m.value[1];
        this->value[2] /= m.value[2];
        this->value[3] /= m.value[3];
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat4x4<T>& tmat4x4<T>::operator+=(const S& s) {
        this->value[0] += s;
        this->value[1] += s;
        this->value[2] += s;
        this->value[3] += s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat4x4<T>& tmat4x4<T>::operator-=(const S& s) {
        this->value[0] -= s;
        this->value[1] -= s;
        this->value[2] -= s;
        this->value[3] -= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat4x4<T>& tmat4x4<T>::operator*=(const S& s) {
        this->value[0] *= s;
        this->value[1] *= s;
        this->value[2] *= s;
        this->value[3] *= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    tmat4x4<T>& tmat4x4<T>::operator/=(const S& s) {
        this->value[0] /= s;
        this->value[1] /= s;
        this->value[2] /= s;
        this->value[3] /= s;
        return *this;
    }

    template <typename T>
    tmat4x4<T> tmat4x4<T>::_inverse() const {
        auto& m = this->value;

        T Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        T Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
        T Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

        T Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        T Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
        T Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

        T Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        T Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
        T Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

        T Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        T Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
        T Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

        T Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        T Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
        T Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

        T Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
        T Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
        T Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

        tvec4<T> Fac0(Coef00, Coef00, Coef02, Coef03);
        tvec4<T> Fac1(Coef04, Coef04, Coef06, Coef07);
        tvec4<T> Fac2(Coef08, Coef08, Coef10, Coef11);
        tvec4<T> Fac3(Coef12, Coef12, Coef14, Coef15);
        tvec4<T> Fac4(Coef16, Coef16, Coef18, Coef19);
        tvec4<T> Fac5(Coef20, Coef20, Coef22, Coef23);

        tvec4<T> Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
        tvec4<T> Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
        tvec4<T> Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
        tvec4<T> Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

        tvec4<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
        tvec4<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
        tvec4<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
        tvec4<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

        tvec4<T> SignA(+1, -1, +1, -1);
        tvec4<T> SignB(-1, +1, -1, +1);
        tmat4x4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

        tvec4<T> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

        tvec4<T> Dot0(m[0] * Row0);
        T Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

        T OneOverDeterminant = static_cast<T>(1) / Dot1;

        return Inverse * OneOverDeterminant;
    }

    template <typename T>
    tmat4x4<T> tmat4x4<T>::_transpose() const {
        tmat4x4 Transpose(detail::uninitialize);
        auto& m = this->value;

        Transpose[0][0] = m[0][0];
        Transpose[0][1] = m[1][0];
        Transpose[0][2] = m[2][0];
        Transpose[0][3] = m[3][0];

        Transpose[1][0] = m[0][1];
        Transpose[1][1] = m[1][1];
        Transpose[1][2] = m[2][1];
        Transpose[1][3] = m[3][1];

        Transpose[2][0] = m[0][2];
        Transpose[2][1] = m[1][2];
        Transpose[2][2] = m[2][2];
        Transpose[2][3] = m[3][2];

        Transpose[3][0] = m[0][3];
        Transpose[3][1] = m[1][3];
        Transpose[3][2] = m[2][3];
        Transpose[3][3] = m[3][3];

        return Transpose;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: v*v
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> operator*(const tvec2<T>& v1, const tvec2<T>& v2) {
        tvec2<T> result = v1;
        result *= v2;
        return result;
    }

    template <typename T> tvec3<T> operator*(const tvec3<T>& v1, const tvec3<T>& v2) {
        tvec3<T> result = v1;
        result *= v2;
        return result;
    }

    template <typename T> tvec4<T> operator*(const tvec4<T>& v1, const tvec4<T>& v2) {
        tvec4<T> result = v1;
        result *= v2;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: v/v
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> operator/(const tvec2<T>& v1, const tvec2<T>& v2) {
        tvec2<T> result = v1;
        result /= v2;
        return result;
    }

    template <typename T> tvec3<T> operator/(const tvec3<T>& v1, const tvec3<T>& v2) {
        tvec3<T> result = v1;
        result /= v2;
        return result;
    }

    template <typename T> tvec4<T> operator/(const tvec4<T>& v1, const tvec4<T>& v2) {
        tvec4<T> result = v1;
        result /= v2;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: v+v
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> operator+(const tvec2<T>& v1, const tvec2<T>& v2) {
        tvec2<T> result = v1;
        result += v2;
        return result;
    }

    template <typename T> tvec3<T> operator+(const tvec3<T>& v1, const tvec3<T>& v2) {
        tvec3<T> result = v1;
        result += v2;
        return result;
    }

    template <typename T> tvec4<T> operator+(const tvec4<T>& v1, const tvec4<T>& v2) {
        tvec4<T> result = v1;
        result += v2;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: v-v
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> operator-(const tvec2<T>& v1, const tvec2<T>& v2) {
        tvec2<T> result = v1;
        result -= v2;
        return result;
    }

    template <typename T> tvec3<T> operator-(const tvec3<T>& v1, const tvec3<T>& v2) {
        tvec3<T> result = v1;
        result -= v2;
        return result;
    }

    template <typename T> tvec4<T> operator-(const tvec4<T>& v1, const tvec4<T>& v2) {
        tvec4<T> result = v1;
        result -= v2;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: v*s, s*v
    // ---------------------------------------------------------------------------------------------
    template <typename T, typename S> tvec2<T> operator*(const tvec2<T>& v, const S& s) {
        tvec2<T> result = v;
        result *= s;
        return result;
    }

    template <typename T, typename S> tvec2<T> operator*(const S& s, const tvec2<T>& v) {
        tvec2<T> result = v;
        result *= s;
        return result;
    }

    template <typename T, typename S> tvec3<T> operator*(const tvec3<T>& v, const S& s) {
        tvec3<T> result = v;
        result *= s;
        return result;
    }

    template <typename T, typename S> tvec3<T> operator*(const S& s, const tvec3<T>& v) {
        tvec3<T> result = v;
        result *= s;
        return result;
    }

    template <typename T, typename S> tvec4<T> operator*(const tvec4<T>& v, const S& s) {
        tvec4<T> result = v;
        result *= s;
        return result;
    }

    template <typename T, typename S> tvec4<T> operator*(const S& s, const tvec4<T>& v) {
        tvec4<T> result = v;
        result *= s;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: m*m
    // ---------------------------------------------------------------------------------------------
    template <typename T> tmat2x2<T> operator*(const tmat2x2<T>& m1, const tmat2x2<T>& m2) {
      tmat2x2<T> result = m1;
      result *= m2;
      return result;
    }

    template <typename T> tmat3x3<T> operator*(const tmat3x3<T>& m1, const tmat3x3<T>& m2) {
      tmat3x3<T> result = m1;
      result *= m2;
      return result;
    }

    template <typename T> tmat4x4<T> operator*(const tmat4x4<T>& m1, const tmat4x4<T>& m2) {
      tmat4x4<T> result = m1;
      result *= m2;
      return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: m*s, s*m
    // ---------------------------------------------------------------------------------------------
    template <typename T, typename S> tmat2x2<T> operator*(const S& s, const tmat2x2<T>& m) {
        tmat2x2<T> result = m;
        result *= s;
        return result;
    }

    template <typename T, typename S> tmat2x2<T> operator*(const tmat2x2<T>& m, const S& s) {
        tmat2x2<T> result = m;
        result *= s;
        return result;
    }

    template <typename T, typename S> tmat3x3<T> operator*(const S& s, const tmat3x3<T>& m) {
        tmat3x3<T> result = m;
        result *= s;
        return result;
    }

    template <typename T, typename S> tmat3x3<T> operator*(const tmat3x3<T>& m, const S& s) {
        tmat3x3<T> result = m;
        result *= s;
        return result;
    }

    template <typename T, typename S> tmat4x4<T> operator*(const S& s, const tmat4x4<T>& m) {
        tmat4x4<T> result = m;
        result *= s;
        return result;
    }

    template <typename T, typename S> tmat4x4<T> operator*(const tmat4x4<T>& m, const S& s) {
        tmat4x4<T> result = m;
        result *= s;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: m*v, v*m
    // ---------------------------------------------------------------------------------------------
    template <typename T> typename tmat2x2<T>::col_type operator*(const tmat2x2<T>& m, const tvec2<T>& v) {
        return typename tmat2x2<T>::col_type(
            m[0][0] * v.x + m[1][0] * v.y,
            m[0][1] * v.x + m[1][1] * v.y);
    }

    template <typename T> typename tmat2x2<T>::row_type operator*(const tvec2<T>& v, const tmat2x2<T>& m) {
        return typename tmat2x2<T>::row_type(
            m[0][0] * v.x + m[0][1] * v.y,
            m[1][0] * v.x + m[1][1] * v.y);
    }

    template <typename T> typename tmat3x3<T>::col_type operator*(const tmat3x3<T>& m, const tvec3<T>& v) {
        return typename tmat3x3<T>::col_type(
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
    }

    template <typename T> typename tmat3x3<T>::row_type operator*(const tvec3<T>& v, const tmat3x3<T>& m) {
        return typename tmat3x3<T>::row_type(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
    }

    template <typename T> typename tmat4x4<T>::col_type operator*(const tmat4x4<T>& m, const tvec4<T>& v) {
        return typename tmat4x4<T>::col_type(
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
            m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w);
    }

    template <typename T> typename tmat4x4<T>::row_type operator*(const tvec4<T>& v, const tmat4x4<T>& m) {
        return typename tmat4x4<T>::row_type(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
    }


    // ---------------------------------------------------------------------------------------------
    // Operator: v==v
    // ---------------------------------------------------------------------------------------------
    template <typename T> bool operator==(const tvec2<T>& v1, const tvec2<T>& v2) {
      return v1.x == v2.x &&
             v1.y == v2.y;
    }

    template <typename T> bool operator==(const tvec3<T>& v1, const tvec3<T>& v2) {
      return v1.x == v2.x &&
             v1.y == v2.y &&
             v1.z == v2.z;
    }

    template <typename T> bool operator==(const tvec4<T>& v1, const tvec4<T>& v2) {
      return v1.x == v2.x &&
             v1.y == v2.y &&
             v1.z == v2.z &&
             v1.w == v2.w;
    }

    // ---------------------------------------------------------------------------------------------
    // Operator: m==m
    // ---------------------------------------------------------------------------------------------
    template <typename T> bool operator==(const tmat2x2<T>& m1, const tmat2x2<T>& m2) {
      return m1[0] == m2[0] &&
             m1[1] == m2[1];
    }

    template <typename T> bool operator==(const tmat3x3<T>& m1, const tmat3x3<T>& m2) {
      return m1[0] == m2[0] &&
             m1[1] == m2[1] &&
             m1[2] == m2[2];
    }

    template <typename T> bool operator==(const tmat4x4<T>& m1, const tmat4x4<T>& m2) {
      return m1[0] == m2[0] &&
             m1[1] == m2[1] &&
             m1[2] == m2[2] &&
             m1[3] == m2[3];
    }


    // ---------------------------------------------------------------------------------------------
    // Dot
    // ---------------------------------------------------------------------------------------------
    template <typename T> T dot(const tvec2<T>& v1, const tvec2<T>& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    template <typename T> T dot(const tvec3<T>& v1, const tvec3<T>& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    template <typename T> T dot(const tvec4<T>& v1, const tvec4<T>& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    // ---------------------------------------------------------------------------------------------
    // Cross Product
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec3<T> cross(const tvec3<T>& v1, const tvec3<T>& v2) {
        return tvec3<T>(
            v1.y * v2.z - v2.y * v1.z,
            v1.z * v2.x - v2.z * v1.x,
            v1.x * v2.y - v2.x * v1.y);
    }

    // ---------------------------------------------------------------------------------------------
    // Length
    // ---------------------------------------------------------------------------------------------
    template <typename T> T length(const tvec2<T>& v) {
        return sqrt(dot(v, v));
    }

    template <typename T> T length(const tvec3<T>& v) {
        return sqrt(dot(v, v));
    }

    template <typename T> T length(const tvec4<T>& v) {
        return sqrt(dot(v, v));
    }

    // ---------------------------------------------------------------------------------------------
    // LengthSquared
    // ---------------------------------------------------------------------------------------------
    template <typename T> T length2(const tvec2<T>& v) {
        return dot(v, v);
    }

    template <typename T> T length2(const tvec3<T>& v) {
        return dot(v, v);
    }

    template <typename T> T length2(const tvec4<T>& v) {
        return dot(v, v);
    }

    // ---------------------------------------------------------------------------------------------
    // Normalize
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> normalize(const tvec2<T>& v) {
        T s = static_cast<T>(1) / length(v);
        return v * s;
    }

    template <typename T> tvec3<T> normalize(const tvec3<T>& v) {
        T s = static_cast<T>(1) / length(v);
        return v * s;
    }

    template <typename T> tvec4<T> normalize(const tvec4<T>& v) {
        T s = static_cast<T>(1) / length(v);
        return v * s;
    }

    // ---------------------------------------------------------------------------------------------
    // Distance: p0 - p1
    // ---------------------------------------------------------------------------------------------
    template <typename T> T distance(const tvec2<T>& p0, const tvec2<T>& p1) {
        return length(p1 - p0);
    }

    template <typename T> T distance(const tvec3<T>& p0, const tvec3<T>& p1) {
        return length(p1 - p0);
    }

    template <typename T> T distance(const tvec4<T>& p0, const tvec4<T>& p1) {
        return length(p1 - p0);
    }

    // ---------------------------------------------------------------------------------------------
    // DistanceSquared: p0 - p1
    // ---------------------------------------------------------------------------------------------
    template <typename T> T distance2(const tvec2<T>& p0, const tvec2<T>& p1) {
        return length2(p1 - p0);
    }

    template <typename T> T distance2(const tvec3<T>& p0, const tvec3<T>& p1) {
        return length2(p1 - p0);
    }

    template <typename T> T distance2(const tvec4<T>& p0, const tvec4<T>& p1) {
        return length2(p1 - p0);
    }

    // ---------------------------------------------------------------------------------------------
    // Component-wise Min
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> min(const tvec2<T>& p0, const tvec2<T>& p1) {
        return tvec2<T>(p0.x < p1.x ? p0.x : p1.x,
                        p0.y < p1.y ? p0.y : p1.y);
    }

    template <typename T> tvec3<T> min(const tvec3<T>& p0, const tvec3<T>& p1) {
        return tvec3<T>(p0.x < p1.x ? p0.x : p1.x,
                        p0.y < p1.y ? p0.y : p1.y,
                        p0.z < p1.z ? p0.z : p1.z);
    }

    template <typename T> tvec4<T> min(const tvec4<T>& p0, const tvec4<T>& p1) {
        return tvec4<T>(p0.x < p1.x ? p0.x : p1.x,
                        p0.y < p1.y ? p0.y : p1.y,
                        p0.z < p1.z ? p0.z : p1.z,
                        p0.w < p1.w ? p0.w : p1.w);
    }

    // ---------------------------------------------------------------------------------------------
    // Component-wise Max
    // ---------------------------------------------------------------------------------------------
    template <typename T> tvec2<T> max(const tvec2<T>& p0, const tvec2<T>& p1) {
        return tvec2<T>(p0.x > p1.x ? p0.x : p1.x,
                        p0.y > p1.y ? p0.y : p1.y);
    }

    template <typename T> tvec3<T> max(const tvec3<T>& p0, const tvec3<T>& p1) {
        return tvec3<T>(p0.x > p1.x ? p0.x : p1.x,
                        p0.y > p1.y ? p0.y : p1.y,
                        p0.z > p1.z ? p0.z : p1.z);
    }

    template <typename T> tvec4<T> max(const tvec4<T>& p0, const tvec4<T>& p1) {
        return tvec4<T>(p0.x > p1.x ? p0.x : p1.x,
                        p0.y > p1.y ? p0.y : p1.y,
                        p0.z > p1.z ? p0.z : p1.z,
                        p0.w > p1.w ? p0.w : p1.w);
    }

    // ---------------------------------------------------------------------------------------------
    // Inverse
    // ---------------------------------------------------------------------------------------------
    template <typename T> tmat2x2<T> inverse(const tmat2x2<T>& m) {
        return m._inverse();
    }

    template <typename T> tmat3x3<T> inverse(const tmat3x3<T>& m) {
        return m._inverse();
    }

    template <typename T> tmat4x4<T> inverse(const tmat4x4<T>& m) {
        return m._inverse();
    }

    // ---------------------------------------------------------------------------------------------
    // Transpose
    // ---------------------------------------------------------------------------------------------
    template <typename T> tmat2x2<T> transpose(const tmat2x2<T>& m) {
        return m._transpose();
    }

    template <typename T> tmat3x3<T> transpose(const tmat3x3<T>& m) {
        return m._transpose();
    }

    template <typename T> tmat4x4<T> transpose(const tmat4x4<T>& m) {
        return m._transpose();
    }

    // ---------------------------------------------------------------------------------------------
    // Translate, Rotate, Scale
    // ---------------------------------------------------------------------------------------------
    template <typename T> tmat4x4<T> translate(const tvec3<T>& v) {
        tmat4x4<T> Result;
        Result[3][0] = v[0];
        Result[3][1] = v[1];
        Result[3][2] = v[2];
        return Result;
    }

    template <typename T> tmat4x4<T> rotate(const T& angle, const tvec3<T>& v) {
        T const a = angle;
        T const c = cos(a);
        T const s = sin(a);

        tvec3<T> axis(normalize(v));
        tvec3<T> temp((T(1) - c) * axis);

        tmat4x4<T> Rotate(detail::uninitialize);
        Rotate[0][0] = c + temp[0] * axis[0];
        Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
        Rotate[0][2] = temp[0] * axis[2] - s * axis[1];
        Rotate[0][3] = 0.0f;

        Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
        Rotate[1][1] = c + temp[1] * axis[1];
        Rotate[1][2] = temp[1] * axis[2] + s * axis[0];
        Rotate[1][3] = 0.0f;

        Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
        Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
        Rotate[2][2] = c + temp[2] * axis[2];
        Rotate[2][3] = 0.0f;

        Rotate[3][0] = 0.0f;
        Rotate[3][1] = 0.0f;
        Rotate[3][2] = 0.0f;
        Rotate[3][3] = 1.0f;

        return Rotate;
    }

    template <typename T> tmat4x4<T> scale(const tvec3<T>& v) {
        tmat4x4<T> Result(detail::uninitialize);
        Result[0][0] = v[0];
        Result[1][1] = v[1];
        Result[2][2] = v[2];
        Result[3][3] = static_cast<T>(1);
        return Result;
    }

    // ---------------------------------------------------------------------------------------------
    // LookAt
    // ---------------------------------------------------------------------------------------------
    template <typename T> tmat4x4<T> lookAt(const tvec3<T>& eye, const tvec3<T>& center, const tvec3<T>& up) {
        return lookAtLH(eye, center, up);
    }

    template <typename T> tmat4x4<T> lookAtLH(const tvec3<T>& eye, const tvec3<T>& center, const tvec3<T>& up) {
        tvec3<T> const f(normalize(center - eye));
        tvec3<T> const s(normalize(cross(up, f)));
        tvec3<T> const u(cross(f, s));

        tmat4x4<T> Result{};
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] = f.x;
        Result[1][2] = f.y;
        Result[2][2] = f.z;
        Result[3][0] = -dot(s, eye);
        Result[3][1] = -dot(u, eye);
        Result[3][2] = -dot(f, eye);
        return Result;
    }

    template <typename T> tmat4x4<T> lookAtRH(const tvec3<T>& eye, const tvec3<T>& center, const tvec3<T>& up) {
        tvec3<T> const f(normalize(center - eye));
        tvec3<T> const s(normalize(cross(f, up)));
        tvec3<T> const u(cross(s, f));

        tmat4x4<T> Result{};
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] =-f.x;
        Result[1][2] =-f.y;
        Result[2][2] =-f.z;
        Result[3][0] =-dot(s, eye);
        Result[3][1] =-dot(u, eye);
        Result[3][2] = dot(f, eye);
        return Result;
    }

    // ---------------------------------------------------------------------------------------------
    // Perspective
    // ---------------------------------------------------------------------------------------------
    template <typename T> tmat4x4<T> perspective(T fovy, T aspect, T zNear, T zFar) {
        return perspectiveLH(fovy, aspect, zNear, zFar);
    }

    template <typename T> tmat4x4<T> perspectiveLH(T fovy, T aspect, T zNear, T zFar) {
        assert(std::abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

        T const tanHalfFovy = tan(fovy / static_cast<T>(2));

        tmat4x4<T> Result(static_cast<T>(0));
        Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
        Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
        Result[2][3] = static_cast<T>(1);

#if (LC_DEPTH_CLIP_SPACE == LC_DEPTH_ZERO_TO_ONE)
        Result[2][2] = zFar / (zFar - zNear);
        Result[3][2] = -(zFar * zNear) / (zFar - zNear);
#else
        Result[2][2] = (zFar + zNear) / (zFar - zNear);
        Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
#endif

        return Result;
    }

    template <typename T> tmat4x4<T> perspectiveRH(T fovy, T aspect, T zNear, T zFar) {
        assert(std::abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

        T const tanHalfFovy = tan(fovy / static_cast<T>(2));

        tmat4x4<T> Result(static_cast<T>(0));
        Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
        Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
        Result[2][3] = -static_cast<T>(1);

#if (LC_DEPTH_CLIP_SPACE == LC_DEPTH_ZERO_TO_ONE)
        Result[2][2] = zFar / (zNear - zFar);
        Result[3][2] = -(zFar * zNear) / (zFar - zNear);
#else
        Result[2][2] = - (zFar + zNear) / (zFar - zNear);
        Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
#endif

        return Result;
    }

    template <typename T> tmat4x4<T> perspectiveFov(T fov, T width, T height, T zNear, T zFar) {
        return perspectiveFovLH(fov, width, height, zNear, zFar);
    }

    template <typename T> tmat4x4<T> perspectiveFovLH(T fov, T width, T height, T zNear, T zFar) {
        assert(width > static_cast<T>(0));
        assert(height > static_cast<T>(0));
        assert(fov > static_cast<T>(0));

        T const rad = fov;
        T const h = std::cos(static_cast<T>(0.5) * rad) / std::sin(static_cast<T>(0.5) * rad);
        T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

        tmat4x4<T> Result(static_cast<T>(0));
        Result[0][0] = w;
        Result[1][1] = h;
        Result[2][3] = static_cast<T>(1);

#if (LC_DEPTH_CLIP_SPACE == LC_DEPTH_ZERO_TO_ONE)
        Result[2][2] = zFar / (zFar - zNear);
        Result[3][2] = -(zFar * zNear) / (zFar - zNear);
#else
        Result[2][2] = (zFar + zNear) / (zFar - zNear);
        Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
#endif

        return Result;
    }

    template <typename T> tmat4x4<T> perspectiveFovRH(T fov, T width, T height, T zNear, T zFar) {
        assert(width > static_cast<T>(0));
        assert(height > static_cast<T>(0));
        assert(fov > static_cast<T>(0));

        T const rad = fov;
        T const h = std::cos(static_cast<T>(0.5) * rad) / std::sin(static_cast<T>(0.5) * rad);
        T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

        tmat4x4<T> Result(static_cast<T>(0));
        Result[0][0] = w;
        Result[1][1] = h;
        Result[2][3] = - static_cast<T>(1);

#if (LC_DEPTH_CLIP_SPACE == LC_DEPTH_ZERO_TO_ONE)
        Result[2][2] = zFar / (zNear - zFar);
        Result[3][2] = -(zFar * zNear) / (zFar - zNear);
#else
        Result[2][2] = - (zFar + zNear) / (zFar - zNear);
        Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
#endif

        return Result;
    }

    // ---------------------------------------------------------------------------------------------
    // Typedefs
    // ---------------------------------------------------------------------------------------------
    using vec2 = tvec2<float>;
    using vec3 = tvec3<float>;
    using vec4 = tvec4<float>;
    using mat2 = tmat2x2<float>;
    using mat3 = tmat3x3<float>;
    using mat4 = tmat4x4<float>;

} // namespace LC_MINIMATH_NAMESPACE
