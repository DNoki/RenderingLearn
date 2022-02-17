#pragma once

typedef DirectX::SimpleMath::Vector2        Vector2;
typedef DirectX::SimpleMath::Vector3        Vector3;
typedef DirectX::SimpleMath::Vector4        Vector4;
//typedef DirectX::SimpleMath::Matrix         Matrix4x4;
//typedef DirectX::SimpleMath::Quaternion     Quaternion;
typedef DirectX::SimpleMath::Plane          Plane;
typedef DirectX::SimpleMath::Rectangle      Rect;
typedef DirectX::SimpleMath::Ray            Ray;
typedef DirectX::SimpleMath::Viewport       Viewport;
typedef DirectX::SimpleMath::Color          Color;

class Matrix4x4;
class Quaternion;


class Math
{
public:
    static const float Epsilon;
    static const float PI;
    static const float Deg2Rad; // 角度转弧度
    static const float Rad2Deg; // 弧度转角度

    inline static float Abs(float x) noexcept { return abs(x); }
    /**
     * @brief 比较两个浮点数是否近似
     * @param a
     * @param b
     * @param epsilon 容许值
     * @return
    */
    inline static bool Approximately(float a, float b, float epsilon = Epsilon) { return DirectX::XMScalarNearEqual(a, b, epsilon); }

    inline static float Sin(float x) noexcept { return DirectX::XMScalarSin(x); }
    inline static float Cos(float x) noexcept { return DirectX::XMScalarCos(x); }
    inline static float Tan(float x) noexcept { return tan(x); }
    inline static float Asin(float x) noexcept { return DirectX::XMScalarASin(x); }
    inline static float Acos(float x) noexcept { return DirectX::XMScalarACos(x); }
    inline static float Atan(float x) noexcept { return atan(x); }
    inline static float Atan2(float y, float x) noexcept { return atan2(y, x); }

    inline static float Clamp(float value, float min, float max) { return std::clamp(value, min, max); }
    inline static float Clamp01(float value, float min, float max) { return Clamp(value, 0.0f, 1.0f); }

    /**
     * @brief 将输入值按指定范围大小步进直至进入该范围
     * @param value 输入值
     * @param min 最小值
     * @param max 最大值（不包含）
     * @return
    */
    inline static int Repeat(int value, int min, int max)
    {
        if (min > max)
        {
            int temp = max;
            max = min;
            min = temp;
        }
        int range = max - min;
        if (range == 0) return min;
        value -= min;
        if (value < 0)
            value = range - (abs(value) % range);
        return (value % range) + min;
    }

private:
    Math() {}

};

class Quaternion : public DirectX::XMFLOAT4
{
public:
    // --------------------------------------------------------------------------
    Quaternion() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) {}
    Quaternion(const Quaternion&) = default;
    Quaternion(Quaternion&&) = default;

    Quaternion(const DirectX::XMFLOAT4& q) noexcept { memcpy(this, &q, sizeof(Quaternion)); }
    Quaternion(const DirectX::XMFLOAT4&& q) noexcept { memcpy(this, &q, sizeof(Quaternion)); }
    Quaternion(const DirectX::XMVECTOR& q) noexcept { DirectX::XMStoreFloat4(this, q); }
    Quaternion(const DirectX::XMVECTOR&& q) noexcept { DirectX::XMStoreFloat4(this, q); }

    Quaternion& operator =(const Quaternion&) = default;
    Quaternion& operator =(Quaternion&&) = default;

    inline operator DirectX::XMFLOAT4& () noexcept { return *this; }
    inline operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat4(this); }

    inline constexpr Quaternion(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}

    // --------------------------------------------------------------------------
    /**
     * @brief 获取欧拉角 （顺序规则为 YXZ）
     * @return
    */
    Vector3 GetEulerAngles() const noexcept;

    /**
     * @brief 四元数的逆
     * @return
    */
    Quaternion Inverse() const noexcept { return DirectX::XMQuaternionInverse(*this); }
    /**
     * @brief 四元数的共轭
     * @return
    */
    Quaternion Conjugate() const noexcept { return DirectX::XMQuaternionConjugate(*this); }


    inline static Quaternion CreateFromEulerAngles(float p, float y, float r) { return CreateFromEulerAngles(Vector3(p, y, r)); }
    inline static Quaternion CreateFromEulerAngles(Vector3 eulerAngles) { return DirectX::XMQuaternionRotationRollPitchYawFromVector(eulerAngles); }
    inline static Quaternion CreateFromAxisAngle(const Vector3& axis, float angle) noexcept { return DirectX::XMQuaternionRotationAxis(axis, angle); }

private:

};

inline Quaternion operator * (const Quaternion& q1, const Quaternion& q2)
{
#ifdef USE_COLUMN_MAJOR
    return DirectX::XMQuaternionMultiply(q2, q1);
#else
    return DirectX::XMQuaternionMultiply(q1, q2);
#endif // USE_COLUMN_MAJOR
}
inline Quaternion operator * (const Vector3& v, const Quaternion& q)
{
#ifdef USE_COLUMN_MAJOR
    return DirectX::XMVector3InverseRotate(v, q);
#else
    return DirectX::XMVector3Rotate(v, q);
#endif // USE_COLUMN_MAJOR
}
inline Quaternion operator * (const Quaternion& q, const Vector3& v) { return v * q.Inverse(); }


class Matrix4x4 : public DirectX::XMFLOAT4X4
{
public:
    // --------------------------------------------------------------------------
    Matrix4x4() : DirectX::XMFLOAT4X4(
        1.f, 0, 0, 0,
        0, 1.f, 0, 0,
        0, 0, 1.f, 0,
        0, 0, 0, 1.f) {}
    Matrix4x4(const Matrix4x4&) = default;
    Matrix4x4(Matrix4x4&&) = default;

    Matrix4x4(const DirectX::XMFLOAT4X4& m) noexcept { memcpy(this, &m, sizeof(Matrix4x4)); }
    Matrix4x4(const DirectX::XMFLOAT4X4&& m) noexcept { memcpy(this, &m, sizeof(Matrix4x4)); }
    Matrix4x4(const DirectX::XMMATRIX& m) noexcept { DirectX::XMStoreFloat4x4(this, m); }
    Matrix4x4(const DirectX::XMMATRIX&& m) noexcept { DirectX::XMStoreFloat4x4(this, m); }

    Matrix4x4& operator =(const Matrix4x4&) = default;
    Matrix4x4& operator =(Matrix4x4&&) = default;

    inline operator DirectX::XMFLOAT4X4& () noexcept { return (*this); }
    inline operator DirectX::XMMATRIX() const noexcept { return DirectX::XMLoadFloat4x4(this); }

    inline constexpr Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept : DirectX::XMFLOAT4X4(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33) {}


    // --------------------------------------------------------------------------
    inline Matrix4x4 Transpose() const noexcept { return DirectX::XMMatrixTranspose(*this); }
    inline Matrix4x4 Invert() const noexcept { return DirectX::XMMatrixInverse(nullptr, *this); }
    inline float Determinant() const noexcept { return DirectX::XMVectorGetX(XMMatrixDeterminant(*this)); }

    inline void SetTRS(const Vector3& p, const Vector3& r, const Vector3& s) noexcept { SetTRS(p, Quaternion::CreateFromEulerAngles(r), s); }
    inline void SetTRS(const Vector3& p, const Quaternion& q, const Vector3& s) noexcept
    {
        using namespace DirectX;
#ifdef USE_COLUMN_MAJOR
        * this = XMMatrixTranslationFromVector(p) * XMMatrixRotationQuaternion(q) * XMMatrixScalingFromVector(s);
#else
        * this = XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(q) * XMMatrixTranslationFromVector(p);
#endif // USE_COLUMN_MAJOR
    }

    bool GetTRS(OUT Vector3& t, OUT Quaternion& r, OUT Vector3& s) const noexcept;


public:
    static const Matrix4x4 Identity;

private:

};

inline Matrix4x4 operator * (const Matrix4x4& m1, const Matrix4x4& m2) noexcept
{
#ifdef USE_COLUMN_MAJOR
    return DirectX::XMMatrixMultiply(m2, m1);
#else
    return DirectX::XMMatrixMultiply(m1, m2);
#endif // USE_COLUMN_MAJOR
}
inline Vector4 operator * (const Matrix4x4& m, const Vector4& v) noexcept
{
#ifdef USE_COLUMN_MAJOR
    return DirectX::XMVector4Transform(v, m);
#else
    return DirectX::XMVector4Transform(v, m.Transpose());
#endif // USE_COLUMN_MAJOR
}
inline Vector4 operator * (const Vector4& v, const Matrix4x4& m) noexcept
{
#ifdef USE_COLUMN_MAJOR
    return DirectX::XMVector4Transform(v, m.Transpose());
#else
    return DirectX::XMVector4Transform(v, m);
#endif // USE_COLUMN_MAJOR
}

