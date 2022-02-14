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

    operator DirectX::XMFLOAT4X4& () noexcept { return (*this); }
    operator DirectX::XMMATRIX() const noexcept { return DirectX::XMLoadFloat4x4(this); }

    inline constexpr Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept : DirectX::XMFLOAT4X4(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33) {}


    inline Matrix4x4 Transpose() noexcept
    {
        using namespace DirectX;
        return XMMatrixTranspose(*this);
    }

    inline void Translation(float x, float y, float z) noexcept
    {
        Translation(Vector3(x, y, z));
    }
    inline void Translation(const Vector3& v) noexcept
    {
        _41 = v.x; _42 = v.y; _43 = v.z;
    }


    //inline static Matrix4x4 CreateFromEulerAngles(const Vector3& eulerAngles) { return CreateFromEulerAngles(eulerAngles.x, eulerAngles.y, eulerAngles.z); }
    //inline static Matrix4x4 CreateFromEulerAngles(float x, float y, float z)
    //{
    //    using namespace DirectX;
    //    Matrix R;
    //    XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(x, y, z));
    //    return R;
    //    //return CreateFromYawPitchRoll(y, x, z);
    //}
    //inline static Matrix4x4 CreateFromTransform(const Vector3& pos, const Vector3& rot, const Vector3& scale)
    //{
    //    using namespace DirectX;
    //    Matrix4x4 R;
    //    R = XMMatrixMultiply(XMMatrixRotationRollPitchYawFromVector(rot), XMMatrixScalingFromVector(scale));
    //    R.Translation(pos);
    //    return R;
    //}

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


class Quaternion : public DirectX::SimpleMath::Quaternion
{
public:
    Quaternion() : DirectX::SimpleMath::Quaternion() {}
    Quaternion(const DirectX::SimpleMath::Quaternion& q) noexcept { memcpy(this, &q, sizeof(DirectX::XMFLOAT4)); }
    Quaternion(const DirectX::SimpleMath::Quaternion&& q) noexcept { memcpy(this, &q, sizeof(DirectX::XMFLOAT4)); }
    Quaternion(const DirectX::XMVECTOR& q) noexcept { DirectX::XMStoreFloat4(this, q); }
    Quaternion(const DirectX::XMVECTOR&& q) noexcept { DirectX::XMStoreFloat4(this, q); }

    inline operator DirectX::SimpleMath::Quaternion& () noexcept { return *this; }

    //inline Vector3 GetEulerAngles() const noexcept
    //{

    //}

private:

};
