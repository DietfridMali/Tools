# pragma once

#include <string.h>
//#include <stdlib.h>
#include <stdint.h>
#include "conversions.hpp"
#include "array.hpp"
#include "vector.hpp"
#include "list.hpp"

// =================================================================================================
// Matrix4f math.Just the usual 3D stuff.

#define ISCOLMAJOR true

class Matrix4f {

public:
    Vector4f        m_data[4];
    float           m_array[16];
    bool            m_isColMajor;
    bool            m_hasChanged;

    static const    Matrix4f IDENTITY;
    static List<Matrix4f> matrixStack;

    Matrix4f(bool isColMajor = ISCOLMAJOR) 
        : m_isColMajor(isColMajor), m_hasChanged(true)
    {
        auto& a = m_data;
        a[0] = Vector4f({ 1.0f, 0.0f, 0.0f, 0.0f });
        a[1] = Vector4f({ 0.0f, 1.0f, 0.0f, 0.0f });
        a[2] = Vector4f({ 0.0f, 0.0f, 1.0f, 0.0f });
        a[3] = Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f });
        m_hasChanged = true;
    }

#if 0
    Matrix4f(Vector4f r, Vector4f u, Vector4f f, Vector4f t = Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }), isColMajor = ISCOLMAJOR) {
        auto& a = m_data;
        a[0] = r;
        a[1] = u;
        a[2] = f;
        a[3] = t;
        m_hasChanged = true;
    }
#endif

    Matrix4f(std::initializer_list<Vector4f> data, bool isColMajor = ISCOLMAJOR) 
        : m_isColMajor(isColMajor), m_hasChanged(true)
    {
#if 1
        *this = data;
#else
        auto& a = m_data;
        size_t i = 0;
        for (auto it = data.begin(); (it != data.end()) and (i < 4); it++)
            a[i++] = *it;
        for (; i < 4; i++) {
            a[i] = Vector4f::ZERO;
            a[i][i] = 1.0f;
        }
        m_hasChanged = true;
#endif
    }


    Matrix4f(float* data, bool isColMajor = ISCOLMAJOR)
        : m_isColMajor(isColMajor), m_hasChanged(true)
    {
        FromArray(data);
    }


    Matrix4f(Vector4f* data, bool isColMajor = ISCOLMAJOR) 
        : m_isColMajor(isColMajor), m_hasChanged(true)
    {
        memcpy(m_data, data, sizeof(m_data));
        m_hasChanged = true;
    }


    inline bool IsColMajor(void) const {
        return m_isColMajor;
    }


    inline Vector4f* GetData(void) {
        return m_data;
    }


    inline float* GetArray(void) {
        return m_array;
    }


    float* AsArray(void);


    Matrix4f& FromArray(const float* data = nullptr);


    Matrix4f& EulerComputeZYX(float sinX, float cosX, float sinY, float cosY, float sinZ, float cosZ, bool colMajor);


    inline const Vector4f& operator[] (const uint32_t i) const {
        return m_data[i];
    }


    inline Vector4f& operator[] (const uint32_t i) {
        return m_data[i];
    }


    inline Matrix4f& operator= (const Matrix4f& other) {
        if (this != &other) {
            memcpy(m_data, other.m_data, sizeof(m_data));
            m_hasChanged = true;
        }
        return *this;
    }

    
    inline Matrix4f& operator= (const float* data) {
        FromArray(data);
        return *this;
    }

    
    inline Matrix4f& operator= (std::initializer_list<Vector4f> data) {
        auto& a = m_data;
        int i = 0;
        for (auto it = data.begin(); (it != data.end()) and (i < 4); it++)
            a[i++] = *it;
        for (; i < 4; i++) {
            a[i] = Vector4f::ZERO;
            a[i][i] = 1.0f;
        }
        m_hasChanged = true;
        return *this;
    }

    
    inline Matrix4f& operator= (const ::Array<float>& data) {
        FromArray(data.Data());
        return *this;
    }

    
    inline Matrix4f Identity(void) {
        return Matrix4f({ Vector4f{ 1.0f, 0.0f, 0.0f, 0.0f }, Vector4f{ 0.0f, 1.0f, 0.0f, 0.0f }, Vector4f{ 0.0f, 0.0f, 1.0f, 0.0f }, Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f } });
    }

    
    inline Vector4f& R(void) {
        return m_data[0];
    }

    
    inline Vector4f& U(void) {
        return m_data[1];
    }

    
    inline Vector4f& F(void) {
        return m_data[2];
    }

    
    inline Vector4f& T(void) {
        return m_data[3];
    }

    
    template<typename T>
    Matrix4f operator* (T&& other) {
        Matrix4f m;
        Mul(m.m_data, m_data, std::forward<T>(other).m_data, this->IsColMajor());
        m.m_hasChanged = true;
        return m;
    }

    
    Matrix4f& operator*= (Matrix4f& other);

    
    static Vector4f* Mul(Vector4f* r, Vector4f* a, Vector4f* o, bool colMajor, int dimensions = 4);

    
    static void Transpose(Vector4f* a, const Vector4f* o, int dimensions = 4);

    
    Matrix4f& operator*= (Matrix4f&& other) {
        Matrix4f::Mul(m_data, m_data, other.m_data, IsColMajor());
        m_hasChanged = true;
        return *this;
    }

    
    Vector3f operator* (Vector3f& v) {
#if 0
        Matrix4f t;
        Vector4f* a;
        if (IsColMajor()) {
            Transpose(t);
            a = t.m_data;
        }
        else
#endif
            Vector4f* a = m_data;
        return Vector3f{ v.Dot(a[0].Data(), 3), v.Dot(a[1].Data(), 3), v.Dot(a[2].Data(), 3) };
    }

    
    Vector4f operator* (Vector4f& v) {
#if 0
        Matrix4f t;
        Vector4f* a;
        if (IsColMajor()) {
            Transpose(t);
            a = t.m_data;
        }
        else
#endif
            Vector4f* a = m_data;
        return Vector4f{ v.Dot(a[0]), v.Dot(a[1]), v.Dot(a[2]), v.Dot(a[3]) };
    }

    
    float Det(void);

    
    Matrix4f AffineInverse(void);

    
    Matrix4f Inverse(void);

    
    Matrix4f Transpose(int dimensions = 4);

    
    // t is destination of transpose result; this call saves an assignment copy compared to t = m.Transpose()
    Matrix4f& Transpose(Matrix4f& t, int dimensions = 4) const;


    static Vector3f Rotate(const Matrix4f& m, const Vector3f& v) {
        auto& a = m.m_data;
        Vector4f r = { v[0], v[1], v[2] };
        return Vector3f{ a[0].Dot(r), a[1].Dot(r), a[2].Dot(r) };
    }

    
    inline Vector3f Rotate(const Vector3f& v) const {
        return Rotate(*this, v);
    }


    inline Vector3f Rotate(Vector3f v) {
#if 1
        return Rotate(*this, v);
#else
        auto& a = m_data;
        return Vector3f{ a[0].Dot(v), a[1].Dot(v), a[2].Dot(v) };
#endif
    }

#if 0
    template<typename VT, typename MT>
    static inline Vector3f Rotate(VT&& v, MT&& r) {
        return std::forwardMT>(r).Rotate(std::forward<VT>(v));
        //return Matrix4f::Rotate(v, static_cast<Matrix4f&>(t));
    }
#endif

    inline Vector3f Unrotate(const Vector3f v) {
        Matrix4f t;
        Transpose(t, 3);
        return Rotate(t, v);
    }


    inline operator const float* () const {
        return m_array;
    }


    static Matrix4f Scaling(float sx, float sy, float sz);

    static Matrix4f Translation(float dx, float dy, float dz, bool colMajor = true);

    static Matrix4f& Rotation(Matrix4f& rotation, float angle, float x, float y, float z);
        
    static Matrix4f Rotation(float angle, float x, float y, float z);

    static inline Matrix4f Rotation(Matrix4f& rotation, float x, float y, float z) {
        float radX = Conversions::DegToRad(x);
        float radY = Conversions::DegToRad(y);
        float radZ = Conversions::DegToRad(z);
        return rotation.EulerComputeZYX(sin(radX), cos(radX), sin(radY), cos(radY), sin(radZ), cos(radZ), rotation.IsColMajor());
    }

    static inline Matrix4f Rotation(Matrix4f& rotation, Vector3f angles) {
        return Rotation(rotation, angles.X(), angles.Y(), angles.Z());
    }

    static inline Matrix4f Rotation(Vector3f angles, bool colMajor) {
        Matrix4f rotation(colMajor);
        return Rotation(rotation, angles.X(), angles.Y(), angles.Z());
    }

    static Matrix4f Translation(const Vector3f& v, bool colMajor) {
        return Matrix4f::Translation(v.X(), v.Y(), v.Z(), colMajor);
    }


    inline Matrix4f& Rotate(float angle, float x, float y, float z) {
        *this *= Matrix4f::Rotation(angle, x, y, z);
        return *this;
    }


    template<typename T>
    Matrix4f& Rotate(T&& r) {
        if (m_isColMajor)
            Mul(this->m_data, std::forward<T>(r).m_data, this->m_data, true); // *this = r * *this
        else
            Mul(this->m_data, this->m_data, std::forward<T>(r).m_data, false); // *this *= r
        m_hasChanged = true;
        return *this;
    }


    inline Matrix4f& Scale(float sx, float sy, float sz) {
        if (IsColMajor())
            Mul(this->m_data, Scaling(sx, sy, sz).m_data, this->m_data, true, 3);
        else
            Mul(this->m_data, this->m_data, Scaling(sx, sy, sz).m_data, false, 3);
        m_hasChanged = true;
        return *this;
    }


    inline Matrix4f& Scale(Vector3f s) {
        return Scale(s.X(), s.Y(), s.Z());
    }


    inline Matrix4f& Translate(float dx, float dy, float dz) {
        Matrix4f t = Matrix4f::Translation(dx, dy, dz);
#if 0
        if (likeOpenGL) {
            Matrix4f r;
            t *= Transpose(r, 3); // transpose *this into r and multiply t with it
            return Transpose(*this, 3); // transpose *this into itself
        }
        else 
#endif
        {
            *this *= t;
            return *this;
        }
    }


    inline Matrix4f& Translate(const Vector3f& offset) {
        return Translate(offset.X(), offset.Y(), offset.Z());
    }


    inline Matrix4f& Translate(Vector3f&& offset) {
        return Translate(offset.X(), offset.Y(), offset.Z());
    }


    static void PushMatrix(Matrix4f& m) {
        matrixStack.Append(m);
    }


    static Matrix4f& PopMatrix(Matrix4f& m) {
        m = matrixStack.Extract(-1);
        return m;
    }

    inline bool IsValid(void)
    {
        for (const auto& v : m_data)
            if (not v.IsValid())
                return false;
        return true;
    }

};

// =================================================================================================
