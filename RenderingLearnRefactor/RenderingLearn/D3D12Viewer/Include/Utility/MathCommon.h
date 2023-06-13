#pragma once

namespace D3D12Viewer
{

    typedef DirectX::SimpleMath::Vector2        Vector2;
    //typedef DirectX::SimpleMath::Vector3        Vector3;
    //typedef DirectX::SimpleMath::Vector4        Vector4;
    //typedef DirectX::SimpleMath::Matrix         Matrix4x4;
    //typedef DirectX::SimpleMath::Quaternion     Quaternion;
    typedef DirectX::SimpleMath::Plane          Plane;
    typedef DirectX::SimpleMath::Rectangle      Rect;
    typedef DirectX::SimpleMath::Ray            Ray;
    typedef DirectX::SimpleMath::Viewport       Viewport;
    //typedef DirectX::SimpleMath::Color          Color;

    class Matrix4x4;
    class Quaternion;


    class Math
    {
    public:
        inline static const float Epsilon = std::numeric_limits<float>::epsilon();
        inline static const float PI = DirectX::XM_PI;
        inline static const float TAU = DirectX::XM_2PI;
        inline static const float Deg2Rad = 0.017453292f; // 角度转弧度
        inline static const float Rad2Deg = 57.295779f; // 弧度转角度

        inline static float Abs(float x) noexcept { return abs(x); }
        inline static float Mod(float x, float y) noexcept { return fmod(x, y); }

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

    class Vector3 : public DirectX::XMFLOAT3
    {
    public:
        using DirectX::XMFLOAT3::XMFLOAT3;
        Vector3(const Vector3&) = default;
        Vector3(Vector3&&) = default;

        Vector3& operator =(const Vector3&) = default;
        Vector3& operator =(Vector3&&) = default;

    public:
        Vector3(const XMFLOAT3& v) { CopyMemory(this, &v, sizeof(Vector3)); }
        Vector3(XMFLOAT3&& v) { CopyMemory(this, &v, sizeof(Vector3)); }
        Vector3(const DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat3(this, v); }
        Vector3(const DirectX::XMVECTOR&& v) noexcept { DirectX::XMStoreFloat3(this, v); }

        operator DirectX::XMFLOAT3& () noexcept { return *this; }
        operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat3(this); }

    public:
        Vector3 operator+ () const noexcept { return *this; }
        Vector3 operator- () const noexcept { return DirectX::XMVectorNegate(*this); }

        Vector3& operator+= (const Vector3& V) noexcept { return *this = *this + V; }
        Vector3& operator-= (const Vector3& V) noexcept { return *this = *this - V; }
        Vector3& operator*= (const Vector3& V) noexcept { return *this = *this * V; }
        Vector3& operator/= (const Vector3& V) noexcept { return *this = *this / V; }
        Vector3& operator*= (const float& s) noexcept { return *this = *this * s; }
        Vector3& operator/= (const float& s) noexcept { return *this = *this / s; }

    public:
        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 Left;
        static const Vector3 Right;
        static const Vector3 Up;
        static const Vector3 Down;
        static const Vector3 Forward;
        static const Vector3 Backward;

    private:

    };

    inline Vector3 operator+ (const Vector3& v1, const Vector3& v2) noexcept { return DirectX::XMVectorAdd(v1, v2); }
    inline Vector3 operator- (const Vector3& v1, const Vector3& v2) noexcept { return DirectX::XMVectorSubtract(v1, v2); }
    inline Vector3 operator* (const Vector3& v1, const Vector3& v2) noexcept { return DirectX::XMVectorMultiply(v1, v2); }
    inline Vector3 operator/ (const Vector3& v1, const Vector3& v2) noexcept { return DirectX::XMVectorDivide(v1, v2); }
    inline Vector3 operator* (const Vector3& v, float s) noexcept { return DirectX::XMVectorScale(v, s); }
    inline Vector3 operator* (float s, const Vector3& v) noexcept { return DirectX::XMVectorScale(v, s); }
    inline Vector3 operator/ (const Vector3& v, float s) noexcept { return DirectX::XMVectorScale(v, 1.0f / s); }


    class Vector4 : public DirectX::XMFLOAT4
    {
    public:
        using DirectX::XMFLOAT4::XMFLOAT4;
        Vector4(const Vector4&) = default;
        Vector4(Vector4&&) = default;

        Vector4& operator =(const Vector4&) = default;
        Vector4& operator =(Vector4&&) = default;

        inline Vector4(const DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat4(this, v); }
        inline Vector4(const DirectX::XMVECTOR&& v) noexcept { DirectX::XMStoreFloat4(this, v); }
        inline constexpr Vector4(Vector3 v, float w) : DirectX::XMFLOAT4(v.x, v.y, v.z, w) {}

        inline operator DirectX::XMFLOAT4& () noexcept { return *this; }
        inline operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat4(this); }
        inline operator Vector3 () const noexcept { return Vector3(x, y, z); }

    public:
        static const Vector4 Zero;
        static const Vector4 One;

    private:

    };

    class Color
    {
    public:
        float r{};
        float g{};
        float b{};
        float a{};

        Color() = default;

        Color(const DirectX::XMFLOAT4& c) noexcept { CopyMemory(this, &c, sizeof(Color)); }
        Color(DirectX::XMFLOAT4&& c) noexcept { CopyMemory(this, &c, sizeof(Color)); }
        Color(const DirectX::XMVECTOR& c) noexcept { DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(this), c); }
        Color(DirectX::XMVECTOR&& c) noexcept { DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(this), c); }

        Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}

        operator DirectX::XMFLOAT4& () noexcept { return *reinterpret_cast<DirectX::XMFLOAT4*>(this); }
        operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(this)); }
        operator const float* () const noexcept { return reinterpret_cast<const float*>(this); }
        operator float* () const noexcept { return const_cast<float*>(reinterpret_cast<const float*>(this)); }

        /**
         * @brief 从 HSV 模型创建颜色(0.0 ~ 1.0)
         * @param hue 色调
         * @param saturation 饱和度
         * @param value 明度
         * @return
        */
        static Color CreateFromHSV(float hue, float saturation, float value);

        String ToString() { return FORMAT(TEXT("(%.2f, %.2f, %.2f, %.2f)"), r, g, b, a); }

    public:
        static const Color Black;
        static const Color White;
        static const Color Gray;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Yellow;

    private:

    };


    class Quaternion : public DirectX::XMFLOAT4
    {
    public:
        // --------------------------------------------------------------------------
        using DirectX::XMFLOAT4::XMFLOAT4;
        Quaternion() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) {}
        Quaternion(const Quaternion&) = default;
        Quaternion(Quaternion&&) = default;

        Quaternion& operator =(const Quaternion&) = default;
        Quaternion& operator =(Quaternion&&) = default;

        Quaternion(const DirectX::XMVECTOR& q) noexcept { DirectX::XMStoreFloat4(this, q); }
        Quaternion(DirectX::XMVECTOR&& q) noexcept { DirectX::XMStoreFloat4(this, q); }
        constexpr Quaternion(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}

        operator DirectX::XMFLOAT4& () noexcept { return *this; }
        operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat4(this); }

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
        inline Quaternion Inverse() const noexcept { return DirectX::XMQuaternionInverse(*this); }
        /**
         * @brief 四元数的共轭
         * @return
        */
        inline Quaternion Conjugate() const noexcept { return DirectX::XMQuaternionConjugate(*this); }

        // --------------------------------------------------------------------------
        inline static Quaternion CreateFromEulerAngles(float p, float y, float r) { return CreateFromEulerAngles(Vector3(p, y, r)); }
        inline static Quaternion CreateFromEulerAngles(const Vector3& eulerAngles) { return DirectX::XMQuaternionRotationRollPitchYawFromVector(eulerAngles * Math::Deg2Rad); }
        inline static Quaternion CreateFromAxisAngle(const Vector3& axis, float angle) noexcept { return DirectX::XMQuaternionRotationAxis(axis, angle * Math::Deg2Rad); }
        static Quaternion LookRotationLH(const Vector3& forward, const Vector3& upwards = Vector3::Up) noexcept
        {
            using namespace DirectX;
            return XMQuaternionRotationMatrix(XMMatrixTranspose(XMMatrixLookToLH(Vector3::Zero, forward, upwards)));
        }
        static Quaternion LookRotationRH(const Vector3& forward, const Vector3& upwards = Vector3::Up) noexcept
        {
            using namespace DirectX;
            return XMQuaternionRotationMatrix(XMMatrixTranspose(XMMatrixLookToRH(Vector3::Zero, forward, upwards)));
        }

    public:
        static const Quaternion Identity;

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
#ifdef USE_COLUMN_MAJOR
    inline Vector3 operator * (const Quaternion& q, const Vector3& v) { return DirectX::XMVector3Rotate(v, q); }
#else
    inline Vector3 operator * (const Vector3& v, const Quaternion& q) { return DirectX::XMVector3Rotate(v, q); }
#endif // USE_COLUMN_MAJOR


    class Matrix4x4 : public DirectX::XMFLOAT4X4
    {
    public:
        // --------------------------------------------------------------------------
        using DirectX::XMFLOAT4X4::XMFLOAT4X4;
        Matrix4x4() : DirectX::XMFLOAT4X4(
            1.f, 0, 0, 0,
            0, 1.f, 0, 0,
            0, 0, 1.f, 0,
            0, 0, 0, 1.f) {}
        Matrix4x4(const Matrix4x4&) = default;
        Matrix4x4(Matrix4x4&&) = default;

        Matrix4x4& operator =(const Matrix4x4&) = default;
        Matrix4x4& operator =(Matrix4x4&&) = default;

        Matrix4x4(const DirectX::XMMATRIX& m) noexcept { DirectX::XMStoreFloat4x4(this, m); }
        Matrix4x4(const DirectX::XMMATRIX&& m) noexcept { DirectX::XMStoreFloat4x4(this, m); }

        operator DirectX::XMFLOAT4X4& () noexcept { return (*this); }
        operator DirectX::XMMATRIX() const noexcept { return DirectX::XMLoadFloat4x4(this); }


        // --------------------------------------------------------------------------
        /**
         * @brief 矩阵的转置
         * @return
        */
        inline Matrix4x4 Transpose() const noexcept { return DirectX::XMMatrixTranspose(*this); }
        /**
         * @brief 矩阵的逆
         * @return
        */
        inline Matrix4x4 Inverse() const noexcept { return DirectX::XMMatrixInverse(nullptr, *this); }
        /**
         * @brief 行列式
         * @return
        */
        inline float Determinant() const noexcept { return DirectX::XMVectorGetX(XMMatrixDeterminant(*this)); }

        inline void SetTRS(const Vector3& p, const Vector3& r, const Vector3& s) noexcept { *this = CreateFromTRS(p, r, s); }
        inline void SetTRS(const Vector3& p, const Quaternion& q, const Vector3& s) noexcept { *this = CreateFromTRS(p, q, s); }

        bool GetTRS(OUT Vector3& t, OUT Quaternion& r, OUT Vector3& s) const noexcept;


        // --------------------------------------------------------------------------
        inline static Matrix4x4 CreateFromTRS(const Vector3& p, const Vector3& r, const Vector3& s) noexcept { return CreateFromTRS(p, Quaternion::CreateFromEulerAngles(r), s); }
        inline static Matrix4x4 CreateFromTRS(const Vector3& p, const Quaternion& q, const Vector3& s) noexcept
        {
            using namespace DirectX;
            return XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(q) * XMMatrixTranslationFromVector(p);
        }
        inline static Matrix4x4 CreateFromTranslation(const Vector3& t) noexcept { return DirectX::XMMatrixTranslationFromVector(t); }
        inline static Matrix4x4 CreateFromRotation(const Vector3& euler) noexcept { return DirectX::XMMatrixRotationRollPitchYawFromVector(euler); }
        inline static Matrix4x4 CreateFromRotation(const Quaternion& quat) noexcept { return DirectX::XMMatrixRotationQuaternion(quat); }
        inline static Matrix4x4 CreateFromScale(const Vector3& scale) noexcept { return DirectX::XMMatrixScalingFromVector(scale); }
        inline static Matrix4x4 CreateFromPerspectiveLH(float fovAngleY, float aspectRatio, float nearZ, float farZ) noexcept
        {
            return DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
        }
        inline static Matrix4x4 CreateFromPerspectiveRH(float fovAngleY, float aspectRatio, float nearZ, float farZ) noexcept
        {
            return DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, nearZ, farZ);
        }
        inline static Matrix4x4 CreateFromOrthographicLH(float viewWidth, float viewHeight, float nearZ, float farZ) noexcept
        {
            return DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
        }
        inline static Matrix4x4 CreateFromOrthographicRH(float viewWidth, float viewHeight, float nearZ, float farZ) noexcept
        {
            return DirectX::XMMatrixOrthographicRH(viewWidth, viewHeight, nearZ, farZ);
        }


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

}