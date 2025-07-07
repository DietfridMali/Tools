# pragma once

#include <cstddef>
#include <array>
#include <utility>
#include <stdexcept>

#include <math.h>
#include <string.h>

// =================================================================================================
// Vector math for 3D rendering

#include <math.h>
#include <string.h>

template <typename DATA_T, size_t componentCount>
class Vector {
    static_assert(componentCount >= 2, "Vector requires at least 2 components");
    static_assert(componentCount <= 4, "Vector supports at most 4 components");

public:
	DATA_T m_data[componentCount];
	DATA_T m_none = (DATA_T) 0;

    Vector() {
        memset(m_data, 0, sizeof(m_data));
    }

    Vector(const Vector& other) {
        memcpy(m_data, other.m_data, sizeof(m_data));
    }
#if 0
    template <typename... Args, typename = std::enable_if_t<sizeof...(Args) <= componentCount and (std::is_same_v<std::decay_t<Args>, DATA_T> and ...) >>
        explicit Vector(Args&&... args) {
        memset(m_data, 0, sizeof(m_data));
        size_t i = 0;
        ((m_data[i++] = std::forward<Args>(args)), ...);
        for (; i < componentCount; i++)
            *Data(i++) = (DATA_T)0;
        }
#endif

    Vector(std::initializer_list<DATA_T> data) {
        static_assert(std::is_same_v<DATA_T, DATA_T>, "DATA_T must be a concrete type");
#if 0
        if (data.size() > componentCount)
            throw std::invalid_argument("Too many elements in initializer list");
#endif
        size_t i = 0;
        for (auto it = data.begin(); (it != data.end()) and (i < componentCount); it++)
            m_data[i++] = *it;
        for (; i < componentCount; i++)
            m_data[i++] = (DATA_T)0;
    }

    Vector(const DATA_T* data, uint32_t size = componentCount) {
        memcpy(m_data, data, size * sizeof(DATA_T));
        if (size < componentCount)
            memset(m_data + size, (componentCount - size) * sizeof(DATA_T), 0);
    }

    static const Vector<DATA_T, componentCount> NONE;
    static const Vector<DATA_T, componentCount> ZERO;
    static const Vector<DATA_T, componentCount> ONE;

    inline DATA_T* Data(void) {
        return m_data;
    }

    inline const DATA_T* Data(void) const {
        return m_data;
    }

    inline DATA_T& operator[](size_t i) {
		return (i < componentCount) ? m_data[i] : m_none;
	}

    inline const DATA_T& operator[](size_t i) const {
        return (i < componentCount) ? m_data[i] : m_none;
    }    

    inline DATA_T X(void) const {
        return m_data[0];
    }

    inline DATA_T Y(void) const {
        return m_data[1];
    }

    inline DATA_T Z(void) const {
        if constexpr (componentCount < 3)
            return DATA_T(0);
        else
            return m_data[2];
    }

    inline DATA_T W(void) const {
        if constexpr (componentCount < 4)
            return DATA_T(0);
        else
            return m_data[3];
    }

    inline DATA_T R(void) const {
        return m_data[0];
    }

    inline DATA_T G(void) const {
        return m_data[1];
    }

    inline DATA_T B(void) const {
        if constexpr (componentCount < 3)
            return DATA_T(0);
        else
            return m_data[2];
    }

    inline DATA_T A(void) const {
        if constexpr (componentCount < 3)
            return DATA_T(0);
        else
            return m_data[3];
    }

    inline DATA_T& X(void) {
        return m_data[0];
    }

    inline DATA_T& Y(void) {
        return m_data[1];
    }

    inline DATA_T& Z(void) {
        static_assert(componentCount >= 3, "Z() is not available for vectors with less than 3 components.");
        return m_data[2];
    }

    inline DATA_T& W(void) {
        static_assert(componentCount >= 4, "W() is not available for vectors with less than 3 components.");
        return m_data[3];
    }

    inline DATA_T& R(void) {
        return m_data[0];
    }

    inline DATA_T& G(void) {
        return m_data[1];
    }

    inline DATA_T& B(void) {
        static_assert(componentCount >= 3, "B() is not available for vectors with less than 3 components.");
        return m_data[2];
    }

    inline DATA_T& A(void) {
        static_assert(componentCount >= 4, "A() is not available for vectors with less than 4 components.");
        return m_data[3];
    }

    inline DATA_T U(void) const {
        return m_data[0];
    }

    inline DATA_T V(void) const {
        return m_data[1];
    }

    inline DATA_T& U(void) {
        return m_data[0];
    }

    inline DATA_T& V(void) {
        return m_data[1];
    }

    inline Vector& operator= (const Vector& other) {
        if (this != &other)
            memcpy(m_data, other.m_data, sizeof(m_data));
        return *this;
    }

    inline Vector& operator= (std::initializer_list<float> values) {
        int i = 0;
        for (auto it = values.begin(); (it != values.end()) and (i < componentCount); it++)
            m_data[i++] = *it;
        for (; i < componentCount; i++)
            m_data[i] = 0.0f;
        return *this;
    }

    inline Vector& operator= (Vector&& other) noexcept {
        if (this != &other)
            memcpy(m_data, other.m_data, sizeof(m_data));
        return *this;
    }

    inline Vector& operator+= (const Vector& other) {
		for (int i = 0; i < componentCount; ++i) 
			m_data[i] += other[i];
        return *this;
    }


    inline Vector& operator-= (const Vector& other) {
        for (int i = 0; i < componentCount; ++i)
            m_data[i] -= other[i];
        return *this;
    }


    inline Vector& operator*= (const DATA_T n) {
        for (int i = 0; i < componentCount; ++i)
            m_data[i] *= n;
        return *this;
    }


    inline Vector& operator/= (const DATA_T n) {
        for (int i = 0; i < componentCount; ++i)
            m_data[i] /= n;
        return *this;
    }


    inline Vector operator* (const DATA_T& n) const {
        Vector v;
        for (int i = 0; i < componentCount; ++i)
			v[i] = m_data[i] * n;
		return v;
    }


    inline Vector operator* (const Vector& other) const {
        Vector v;
        for (int i = 0; i < componentCount; ++i)
            v[i] = m_data[i] * other[i];
        return v;
    }


    inline Vector operator+ (const Vector& other) const {
        Vector v;
        for (int i = 0; i < componentCount; ++i)
            v[i] = m_data[i] + other[i];
        return v;
    }


    inline Vector operator- (const Vector& other) const {
        Vector v;
        for (int i = 0; i < componentCount; ++i)
            v[i] = m_data[i] - other[i];
        return v;
    }


    inline Vector operator/ (const DATA_T n) const {
        Vector v;
        for (int i = 0; i < componentCount; ++i)
            v[i] = m_data[i] / n;
        return v;
    }


    inline Vector operator- (void) {
        Vector v;
        for (int i = 0; i < componentCount; ++i)
            v[i] = -m_data[i];
        return v;
    }


    inline Vector& Negate(void) {
        for (int i = 0; i < componentCount; ++i)
            m_data[i] = -m_data[i];
        return *this;
    }


    inline const bool operator== (const Vector& other) const {
        for (int i = 0; i < componentCount; ++i)
            if (m_data[i] != other[i])
				return false;
        return true;
    }


    inline const bool operator!= (const Vector& other) const {
        for (int i = 0; i < componentCount; ++i)
            if (m_data[i] == other[i])
                return false;
        return true;
    }

    inline DATA_T Dot(const Vector& other, int range = componentCount) const {
        DATA_T dot = (DATA_T)0;
        for (int i = 0; i < range; i++)
            dot += m_data[i] * other[i];
        return dot;
    }


    inline DATA_T Dot(const float* other, int range = componentCount) const {
        DATA_T dot = (DATA_T)0;
        for (int i = 0; i < range; ++i)
            dot += m_data[i] * other[i];
        return dot;
    }


    inline const Vector Cross(const Vector& other) const {
        auto& a = m_data;
        auto& o = other.m_data;
        return Vector({ a[1] * o[2] - a[2] * o[1],
                        a[2] * o[0] - a[0] * o[2],
                        a[0] * o[1] - a[1] * o[0] });
    }


    inline const DATA_T Length(void) {
        return DATA_T(sqrt(this->Dot(*this)));
    }


    inline Vector& Normalize(void) {
        DATA_T l = this->Length();
        if ((l != 0.0) and (l != 1.0))
            *this /= l;
        return *this;
    }


    inline Vector& Normal(const Vector& v) {
        DATA_T l = v.Length();
        return ((l != 0.0) and (l != 1.0)) ? v / l : v;
    }


    inline Vector& Normal(Vector&& v) {
        DATA_T l = v.Length();
        return ((l != 0.0) and (l != 1.0)) ? v / l : v;
    }


    inline const DATA_T Min(void) const {
		DATA_T v = m_data[0];
        for (size_t i = 1; i < componentCount; i++)
			if (m_data[i] < v)
				v = m_data[i];
		return v;
    }


    inline const DATA_T Max(void) const {
        DATA_T v = m_data[0];
        for (size_t i = 1; i < componentCount; i++)
            if (m_data[i] > v)
                v = m_data[i];
        return v;
    }


    inline const Vector& Minimize(const Vector& other) {
        for (size_t i = 0; i < componentCount; i++)
            if (m_data[i] > other[i])
				m_data[i] = other[i];
        return *this;
    }


    inline const Vector& Maximize(const Vector& other) {
        for (size_t i = 0; i < componentCount; i++)
            if (m_data[i] < other[i])
                m_data[i] = other[i];
        return *this;
    }


    inline const bool IsValid(void) const {
        for (size_t i = 0; i < componentCount; i++)
			if (m_data[i] != m_data[i]) // NaN == NaN should always yield false
				return false;
		return true;
    }

    static inline Vector Perp(const Vector& v0, const Vector& v1, const Vector& v2) {
        return (v1 - v0).Cross(v2 - v0);
    }

    static inline Vector Normal(const Vector& v0, const Vector& v1, const Vector& v2) {
        return Perp(v0, v1, v2).Normalize();
    }

    static int Compare(Vector& v0, Vector& v1) {
        for (size_t i = 0; i < componentCount; i++) {
            if (v0[i] < v1[i])
                return -1;
            if (v0[i] > v1[i])
                return 1;
        }
        return 0;
    }


    inline operator const float*() const {
        return m_data;
    }


    inline DATA_T* AsArray(void) {
        return m_data;
    }


    static DATA_T Dot(float* v1, float* v2, int componentCount) {
        DATA_T dot = (DATA_T)0;
        for (int i = 0; i < componentCount; i++)
            dot += v1[i] * v2[i];
        return dot;
    }
};

// =================================================================================================
// Vector4f math for 3D rendering

using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;

// =================================================================================================
