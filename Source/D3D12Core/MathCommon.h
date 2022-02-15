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
    //inline static float Dot(const Vector2& v1, const Vector2& v2) { return v1.Dot(v2); }
    //inline static float Dot(const Vector3& v1, const Vector3& v2) { return v1.Dot(v2); }
    //inline static float Dot(const Vector4& v1, const Vector4& v2) { return v1.Dot(v2); }

private:
    Math() {}

};

class Quaternion : public DirectX::XMFLOAT4
{
public:
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

    inline Vector3 GetEulerAngles() const noexcept
    {
        using namespace DirectX;
        // TODO
        return Vector3();
    }

    inline static Quaternion EulerAngles(float p, float y, float r)
    {
        using namespace DirectX;
        // XM标准为 ZXY
        return XMQuaternionRotationRollPitchYaw(p, y, r);
    }
    inline static Quaternion EulerAngles(Vector3 eulerAngles)
    {
        using namespace DirectX;
        return EulerAngles(eulerAngles.x, eulerAngles.y, eulerAngles.z);
    }

private:

};

class Matrix4x4 : public DirectX::XMFLOAT4X4
{
public:
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


    inline Matrix4x4 Transpose() const noexcept
    {
        using namespace DirectX;
        return XMMatrixTranspose(*this);
    }
    inline Matrix4x4 Invert() const noexcept
    {
        using namespace DirectX;
        return XMMatrixInverse(nullptr, *this);
    }
    inline float Determinant() const noexcept
    {
        using namespace DirectX;
        return XMVectorGetX(XMMatrixDeterminant(*this));
    }

    void SetTRS(const Vector3& p, const Vector3& r, const Vector3& s) noexcept;
    inline void SetTRS(const Vector3& p, const Quaternion& q, const Vector3& s) noexcept
    {
        using namespace DirectX;
        *this = XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(q) * XMMatrixTranslationFromVector(p);
    }


public:
    static const Matrix4x4 Identity;

private:

};

inline Matrix4x4 operator * (const Matrix4x4& m1, const Matrix4x4& m2) noexcept
{
    using namespace DirectX;
    return XMMatrixMultiply(m1, m2);
}
inline Vector4 operator * (const Matrix4x4& m, const Vector4& v) noexcept
{
    using namespace DirectX;
    return XMVector4Transform(v, XMMatrixTranspose(m));
}
inline Vector4 operator * (const Vector4& v, const Matrix4x4& m) noexcept
{
    using namespace DirectX;
    return XMVector4Transform(v, m);
}

