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

class Matrix4x4 : public DirectX::SimpleMath::Matrix
{
public:
    Matrix4x4() : DirectX::SimpleMath::Matrix() {}
    Matrix4x4(const DirectX::SimpleMath::Matrix& m) noexcept { memcpy(this, &m, sizeof(DirectX::XMFLOAT4X4)); }
    Matrix4x4(const DirectX::SimpleMath::Matrix&& m) noexcept { memcpy(this, &m, sizeof(DirectX::XMFLOAT4X4)); }
    Matrix4x4(const DirectX::XMFLOAT4X4& m) noexcept { memcpy(this, &m, sizeof(DirectX::XMFLOAT4X4)); }
    Matrix4x4(const DirectX::XMFLOAT4X4&& m) noexcept { memcpy(this, &m, sizeof(DirectX::XMFLOAT4X4)); }
    Matrix4x4(const DirectX::XMMATRIX& m) noexcept { DirectX::XMStoreFloat4x4(this, m); }
    Matrix4x4(const DirectX::XMMATRIX&& m) noexcept { DirectX::XMStoreFloat4x4(this, m); }

    inline operator DirectX::SimpleMath::Matrix& () noexcept { return *this; }

    inline static Matrix4x4 CreateFromEulerAngles(const Vector3& eulerAngles) { return CreateFromEulerAngles(eulerAngles.x, eulerAngles.y, eulerAngles.z); }
    inline static Matrix4x4 CreateFromEulerAngles(float x, float y, float z)
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(x, y, z));
        return R;
        //return CreateFromYawPitchRoll(y, x, z);
    }
    inline static Matrix4x4 CreateFromTransform(const Vector3& pos, const Vector3& rot, const Vector3& scale)
    {
        using namespace DirectX;
        Matrix4x4 R;
        R = XMMatrixMultiply(XMMatrixRotationRollPitchYawFromVector(rot), XMMatrixScalingFromVector(scale));
        R.Translation(pos);
        return R;
    }

private:

};

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
